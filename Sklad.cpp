//
//  Sklad.cpp
//  Test App
//
//  Created by Sergei Makarov on 23.05.16.
//  Copyright © 2016 Sergei Makarov. All rights reserved.
//

#include "Sklad.hpp"

#include <string>
#include <iostream>

int GetRandom(int first_value, int last_value)
{
    return first_value + rand() % last_value;
}

const int elementSize = 1;

///////////////////////////////////////////////////////////////////////////////////////////

#define SHELVING_DEF_CAPACITY 3

Shelving::Shelving()
:m_id(0),
m_isFull(false)
{
    
}

Shelving::~Shelving()
{
    for (int i = 0; i < m_size.x; ++i)
        delete [] m_cells[i];
    delete [] m_cells;
}

void Shelving::SetSize(int _x, int _y)
{
    m_size = vec2(_x, _y);
    m_cells = new short*[_x];
    for (int i = 0; i < _x; ++i)
    {
        m_cells[i] = new short[_y];
        memset(m_cells[i], 0, sizeof(short)*_y);
    }
}

bool Shelving::FindPositionOfFreeCell(vec2& pos)
{
    bool result = false;
    for (int i = 0; i < m_size.x; ++i)
    {
        for (int j = 0; j < m_size.y; ++j)
        {
            if (m_cells[i][j] == 0)
            {
                result = true;
                pos.x = m_position.x + i;
                pos.y = m_position.y;
                return result;
            }
        }
    }
    return result;
}

bool Shelving::FindPositionOfLastElement(vec2& pos)
{
    bool result = false;
    for (int i = m_size.x - 1; i >= 0; --i)
    {
        for (int j = m_size.y-1; j >= 0; --j)
        {
            if (m_cells[i][j] == 0)
            {
                result = true;
                pos.x = m_position.x + i;
                pos.y = m_position.y;
                return result;
            }
        }
    }
    return result;
}

void Shelving::AddElement(const vec2& cranePosition)
{
    for (int i = 0; i < m_size.x; ++i)    // находим позицию на стеллаже по X где стоит кран
    {
        if (i == cranePosition.x - m_position.x)
        {
            for (int j = 0; j < m_size.y; ++j)  // ищем там свободную ячейку по координате Y
            {
                if (m_cells[i][j] == 0)
                {
                    // запихиваем элемент
                    m_cells[i][j] = 1;
                    if (i == m_size.x - 1 && j == m_size.y - 1)
                        m_isFull = true;
                    return;
                }
            }
        }
    }
}

void Shelving::RemoveElement(const vec2& cranePosition)
{
    for (int i = 0; i < m_size.x; ++i)    // находим позицию на стеллаже по X где стоит кран
    {
        if (i == cranePosition.x - m_position.x)
        {
            for (int j = 0; j < m_size.y; ++j)  // ищем там свободную ячейку по координате Y
            {
                if (m_cells[i][j] == 1)
                {
                    // вытаскиваем элемент
                    m_cells[i][j] = 0;
                    m_isFull = false;
                    return;
                }
            }
        }
    }
}

short Shelving::GetCellState(int _x)
{
    short result = 0;
    for (int i = 0; i < m_size.x; ++i)    // находим позицию на стеллаже по X
    {
        if (i == _x)
        {
            for (int j = 0; j < m_size.y; ++j)  // считаем кол-во занятых ячеек по координате Y
            {
                if (m_cells[i][j] == 1)
                    result++;
            }
            break;
        }
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////

#define CRANE_DEF_CAPACITY 3

StackerCrane::StackerCrane()
:m_state(eCS_waiting),
m_currentTask(eTT_none),
m_moveToShelving(NULL)
{
    
}

#define CRANE_MOVE_Y_ON_X 1 // кран двигается по оси Y только по координате Х = 1

vec2 StackerCrane::MoveToAim()
{
    vec2 result;
    if (m_position.y != m_aim.y)
    {
        if (m_position.x != CRANE_MOVE_Y_ON_X)
        {
            if (m_position.x > CRANE_MOVE_Y_ON_X)
                result.x--;
            else if (m_position.x < CRANE_MOVE_Y_ON_X)
                result.x++;
        }
        else
        {
            if (m_position.y > m_aim.y)
                result.y--;
            else if (m_position.y < m_aim.y)
                result.y++;
        }
    }
    else if(m_position.x != m_aim.x)
    {
        if (m_position.x > m_aim.x)
            result.x--;
        else if (m_position.x < m_aim.x)
            result.x++;
    }
    return result;
}

void StackerCrane::Simulate()
{
    if (m_state == eCS_moveToDock)
    {
        vec2 moveVec = MoveToAim();
        if (moveVec.x == 0 && moveVec.y == 0) // приехали
        {
            if (m_currentTask == eTT_loading)
                m_state = eCS_loadingItem;
            else
                m_state = eCS_unloadingItem;
        }
        else
            m_position += moveVec;
        std::cout << "Crane move to dock" << std::endl;
    }
    else if(m_state == eCS_loadingItem)
    {
        if (m_currentTask == eTT_loading)
        {
            vec2 aimPos;
            if (Shelving* pShelving = Warehouse::Get()->FindPositionOfFreeCell(aimPos))
            {
                m_moveToShelving = pShelving;
                m_state = eCS_moveToShelving;
                SetAim(aimPos);
            }
        }
        std::cout << "Crane loading item" << std::endl;
    }
    else if(m_state == eCS_moveToShelving)
    {
        vec2 moveVec = MoveToAim();
        if (moveVec.x == 0 && moveVec.y == 0) // приехали
        {
            if (m_currentTask == eTT_loading)
                m_state = eCS_unloadingItem;
            else
                m_state = eCS_loadingItem;
        }
        else
            m_position += moveVec;
        std::cout << "Crane move to shelving" << std::endl;
    }
    else if(m_state == eCS_unloadingItem)
    {
        if (m_currentTask == eTT_loading)
        {
            if (m_moveToShelving)
            {
                m_moveToShelving->AddElement(m_position);
                m_state = eCS_waiting;
                m_currentTask = eTT_none;
            }
        }
        std::cout << "Crane unloading item" << std::endl;
    }
}

void StackerCrane::AddTask(eTaskType type)
{
    if (m_currentTask == eTT_none)
    {
        m_currentTask = type;
        if (m_currentTask == eTT_loading)
        {
            vec2 inputDockPos = Warehouse::Get()->m_inputDock.GetPosition();
            inputDockPos.x += 1;
            if (m_position.x != inputDockPos.x || m_position.y != inputDockPos.y)
            {
                SetAim(inputDockPos);
                m_state = eCS_moveToDock;
            }
            else
                m_state = eCS_loadingItem;
        }
        else if (m_currentTask == eTT_unloading)
        {
            ;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////

Dock::Dock(int _x, int _y)
:m_position(_x, _y)
{
    ;
}

///////////////////////////////////////////////////////////////////////////////////////////
Warehouse* Warehouse::s_warehouse = NULL;
Warehouse* Warehouse::Get()
{
    return s_warehouse;
}

Warehouse::Warehouse(int _x, int _y)
:m_size(_x, _y),
m_inputDock(0,2),
m_outputDock(0,3)
{
    s_warehouse = this;
    m_crane.SetPosition(vec2(1,4));
    int yPos = 1;
    for (int i = 0; i < m_numShelvings; ++i)
    {
        m_shelvings[i].SetId(i);
        m_shelvings[i].SetSize(20, 6);
        m_shelvings[i].SetPosition(3 + m_shelvings[i].GetSize().x*(i%2) + (i%2), yPos);
        if (i%2)
            yPos+=2;
    }
    m_crane.AddTask(StackerCrane::eTT_loading);
}

Warehouse::~Warehouse()
{
    s_warehouse = NULL;
}

void Warehouse::Simulate()
{
    long long step = 0; // шаг симуляции
    while (m_crane.GetState() != StackerCrane::eCS_waiting)
    {
        std::cout << "Step " << step << std::endl;
        m_crane.Simulate();
        Draw();
        ++step;
        std::cout << std::endl;
    }
}

Shelving* Warehouse::FindPositionOfFreeCell(vec2& pos)
{
    Shelving* result = NULL;
    for (int i = 0; i < m_numShelvings; ++i)
    {
        if (m_shelvings[i].FindPositionOfFreeCell(pos))
        {
            pos.y += 1;
            result = &m_shelvings[i];
            break;
        }
    }
    return result;
}

bool Warehouse::IsInAnyShelving(const vec2& pos, short& cellState)
{
    bool result = false;
    
    for (int i = 0; i < m_numShelvings; ++i)
    {
        if (pos.y == m_shelvings[i].GetPosition().y)
        {
            if(pos.x >= m_shelvings[i].GetPosition().x && pos.x < m_shelvings[i].GetPosition().x + m_shelvings[i].GetSize().x)
            {
                result = true;
                cellState = m_shelvings[i].GetCellState(pos.x - m_shelvings[i].GetPosition().x);
            }
        }
    }
    
    return result;
}

void Warehouse::Draw()
{
    std::cout << "Draw warehouse: " << std::endl;
    short cellState = 0;
    for (int j = 0; j < m_size.y; ++j)
    {
        for (int i = 0; i < m_size.x; ++i)
        {
            vec2 pos(i,j);
            if (pos.x == m_inputDock.GetPosition().x && pos.y == m_inputDock.GetPosition().y)
                std::cout << "I";
            else if (pos.x == m_outputDock.GetPosition().x && pos.y == m_outputDock.GetPosition().y)
                std::cout << "P";
            else if (pos.x == m_crane.GetPosition().x && pos.y == m_crane.GetPosition().y)
                std::cout << "C";
            else if (IsInAnyShelving(pos, cellState))
                std::cout << cellState;
            else
                std::cout << "-";
            
            std::cout << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "Draw warehouse end." << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////

void sklad_main()
{
    Warehouse warehouse(45, 7);
    warehouse.Draw();
    warehouse.Simulate();
}
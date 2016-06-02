//
//  Sklad.cpp
//  Test App
//
//  Created by Sergei Makarov on 23.05.16.
//  Copyright © 2016 Sergei Makarov. All rights reserved.
//

#include "Sklad.hpp"


int GetRandom(int first_value, int last_value)
{
    return first_value + rand() % last_value;
}

int operator==(const vec2 &a, const vec2 &b)
{
    return (a.x == b.x) && (a.y == b.y);
}

const int elementSize = 1;

///////////////////////////////////////////////////////////////////////////////////////////

unsigned int Item::s_itemIdCounter = 0;

Item* Item::CreateItem(eItemGroup group, unsigned int type)
{
    return new Item(group, type);
}

Item::Item(eItemGroup group, unsigned int type)
:m_id(s_itemIdCounter++),
m_group(group),
m_type(type)
{
    
}

///////////////////////////////////////////////////////////////////////////////////////////

#define SHELVING_DEF_CAPACITY 3

Shelving::Shelving()
:m_id(0)
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
    m_cells = new Item**[_x];
    for (int i = 0; i < _x; ++i)
    {
        m_cells[i] = new Item*[_y];
        for (int j = 0; j < _y; ++j)
            m_cells[i][j] = GetRandom(0, 10) > 5 ? NULL : Item::CreateItem((Item::eItemGroup)GetRandom(1, 3), GetRandom(1, 5));
    }
}

bool Shelving::FindPositionOfFreeCell(vec2& pos)
{
    bool result = false;
    for (int i = 0; i < m_size.x; ++i)
    {
        for (int j = 0; j < m_size.y; ++j)
        {
            if (m_cells[i][j] == NULL)
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
            if (m_cells[i][j] == NULL)
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

void Shelving::AddElement(const vec2& cranePosition, Item* pItem)
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
                    m_cells[i][j] = pItem;
                    return;
                }
            }
        }
    }
}

Item* Shelving::RemoveElement(const vec2& cranePosition)
{
    Item* result = NULL;
    for (int i = 0; i < m_size.x; ++i)    // находим позицию на стеллаже по X где стоит кран
    {
        if (i == cranePosition.x - m_position.x)
        {
            for (int j = 0; j < m_size.y; ++j)  // ищем там свободную ячейку по координате Y
            {
                if (m_cells[i][j])
                {
                    // вытаскиваем элемент
                    result = m_cells[i][j];
                    m_cells[i][j] = NULL;
                    return result;
                }
            }
        }
    }
    return result;
}

Item* Shelving::GetItemInCell(int _x, int _y)
{
    return m_cells[_x][_y];
}

///////////////////////////////////////////////////////////////////////////////////////////

#define CRANE_DEF_CAPACITY 3

StackerCrane::StackerCrane()
:m_state(eCS_waiting),
m_currentTask(eTT_none),
m_moveToShelving(NULL),
m_item(NULL)
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
        {
            m_position += moveVec;
            if (m_position == m_aim) // приехали
            {
                if (m_currentTask == eTT_loading)
                    m_state = eCS_loadingItem;
                else
                    m_state = eCS_unloadingItem;
            }
        }
        std::cout << "Crane move to dock" << std::endl;
    }
    else if(m_state == eCS_loadingItem)
    {
        if (m_currentTask == eTT_loading)
        {
            if(!m_item)
                m_item = Item::CreateItem((Item::eItemGroup)GetRandom(1, 3), GetRandom(1, 5));
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
        {
            m_position += moveVec;
            if (m_position == m_aim) // приехали
            {
                if (m_currentTask == eTT_loading)
                    m_state = eCS_unloadingItem;
                else
                    m_state = eCS_loadingItem;
            }
        }
        std::cout << "Crane move to shelving" << std::endl;
    }
    else if(m_state == eCS_unloadingItem)
    {
        if (m_currentTask == eTT_loading)
        {
            if (m_moveToShelving)
            {
                m_moveToShelving->AddElement(m_position, m_item);
                m_item = NULL;
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
            vec2 inputDockPos = Warehouse::Get()->m_dock.GetPosition();
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

void Dock::Simulate()
{
    // на каждом шаге с вероятностью 20% добавляется новая заявка
    if (GetRandom(0, 100) < 20)
    {
        if (GetRandom(0, 100) > 50)
            AddInputOrder();
        else
            AddOutputOrder();
    }
}

void Dock::AddInputOrder()
{
    if (m_inputOrders.size() < m_maxNumOrders)
    {
        sOrderInput newOrder;
        newOrder.m_item = Item::CreateItem((Item::eItemGroup)GetRandom(1, 3), GetRandom(1, 5));
        m_inputOrders.push(newOrder);
    }
}

void Dock::AddOutputOrder()
{
    if (m_outputOrders.size() < m_maxNumOrders)
    {
        sOrderOutput newOrder;
        newOrder.itemGroup = (Item::eItemGroup)GetRandom(1, 3);
        newOrder.itemType = GetRandom(1, 5);
        m_outputOrders.push(newOrder);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
Warehouse* Warehouse::s_warehouse = NULL;
Warehouse* Warehouse::Get()
{
    return s_warehouse;
}

#define SHELVING_SIZE_X 20
#define SHELVING_SIZE_Y 4

Warehouse::Warehouse(int _x, int _y)
:m_size(_x, _y),
m_dock(0,2)
{
    s_warehouse = this;
    m_crane.SetPosition(vec2(5,4));
    int yPos = 1;
    for (int i = 0; i < m_numShelvings; ++i)
    {
        m_shelvings[i].SetId(i);
        m_shelvings[i].SetSize(SHELVING_SIZE_X, SHELVING_SIZE_Y);
        m_shelvings[i].SetPosition(3 + m_shelvings[i].GetSize().x*(i%2) + (i%2), yPos);
        if (i%2)
            yPos+=2;
    }
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
        m_dock.Simulate();
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

Shelving* Warehouse::IsInAnyShelving(const vec2& pos)
{
    Shelving* result = NULL;
    
    for (int i = 0; i < m_numShelvings; ++i)
    {
        if (pos.y == m_shelvings[i].GetPosition().y)
        {
            if(pos.x >= m_shelvings[i].GetPosition().x && pos.x < m_shelvings[i].GetPosition().x + m_shelvings[i].GetSize().x)
            {
                result = &m_shelvings[i];
                break;
            }
        }
    }
    return result;
}

std::string Warehouse::DrawShelvings(vec2 pos)
{
    std::string output;
    
    for (int j = 0; j < SHELVING_SIZE_Y; ++j)
    {
        if (j != SHELVING_SIZE_Y-1)
            output += "  | ";
        else
            output += std::to_string(pos.y) + " " + "|" + " ";
            
        for (int i = 0; i < m_size.x; ++i)
        {
            vec2 posInShelving(i, pos.y);
            if (Shelving* sh = IsInAnyShelving(posInShelving))
            {
                Item* pItem = sh->GetItemInCell(i - sh->GetPosition().x, j);
                if (pItem)
                    output += "0" + std::to_string(pItem->GetGroup());
                else
                    output += "00";
            }
            else
            {
                if (j != SHELVING_SIZE_Y-1)
                    output += "  ";
                else
                    output += "--";
            }
            output += " ";
        }
        if (j != SHELVING_SIZE_Y-1)
            output += " |\n";
    }
    
    return output;
}


void Warehouse::Draw()
{
    std::cout << "Draw warehouse: " << std::endl;
    
    std::cout << "  | ";
    for (int i = 0; i < m_size.x; ++i)
        std::cout << std::setw(2) << std::setfill('0') << i << " ";
    std::cout << " |" << std::endl;
    
    std::cout << "__|";
    for (int i = 0; i < m_size.x; ++i)
        std::cout << "___";
    std::cout << "__|" << std::endl;
    
    for (int j = 0; j < m_size.y; ++j)
    {
        std::string output;
        output += std::to_string(j) + " " + "|" + " ";
        for (int i = 0; i < m_size.x; ++i)
        {
            vec2 pos(i,j);
            if (pos.x == m_dock.GetPosition().x && pos.y == m_dock.GetPosition().y)
                output += "DD";
            else if (pos.x == m_crane.GetPosition().x && pos.y == m_crane.GetPosition().y)
                output += "CC";
            else if (IsInAnyShelving(pos))
            {
                output = DrawShelvings(pos);
                break;
            }
            else
                output += "--";
            
            output += " ";
        }
        std::cout << output << " |" << std::endl;
    }
    
    std::cout << "__|";
    for (int i = 0; i < m_size.x; ++i)
        std::cout << "___";
    std::cout << "__|" << std::endl;
    
    std::cout << "Draw warehouse end." << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////

void sklad_main()
{
    Warehouse warehouse(45, 7);
    warehouse.Draw();
//    warehouse.Simulate();
}
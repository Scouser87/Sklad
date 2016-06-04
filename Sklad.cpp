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
            m_cells[i][j] = NULL; //GetRandom(0, 10) > 5 ? NULL : Item::CreateItem((Item::eItemGroup)GetRandom(1, 3), GetRandom(1, 5));
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

Item* Shelving::FindPositionOfItem(vec2& pos, Item::eItemGroup itemGroup, unsigned int itemType)
{
    Item* result = NULL;
    for (int i = 0; i < m_size.x; ++i)
    {
        for (int j = 0; j < m_size.y; ++j)
        {
            Item* pItem = m_cells[i][j];
            if (pItem != NULL)
            {
                if (pItem->GetGroup() == itemGroup && pItem->GetType() == itemType)
                {
                    pos.x = m_position.x + i;
                    pos.y = m_position.y;
                    result = pItem;
                    return result;
                }
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

bool Shelving::GetItem(const vec2& cranePosition, Item* pItem)
{
    for (int i = 0; i < m_size.x; ++i)    // находим позицию на стеллаже по X где стоит кран
    {
        if (i == cranePosition.x - m_position.x)
        {
            for (int j = 0; j < m_size.y; ++j)  // ищем там свободную ячейку по координате Y
            {
                if (m_cells[i][j] == pItem)
                {
                    // забираем элемент
                    m_cells[i][j] = NULL;
                    return true;
                }
            }
        }
    }
    return false;
}

Item* Shelving::GetItemInCell(int _x, int _y)
{
    return m_cells[_x][_y];
}

///////////////////////////////////////////////////////////////////////////////////////////

#define CRANE_DEF_CAPACITY 3

StackerCrane::StackerCrane()
:m_state(eCS_waiting),
m_moveToShelving(NULL),
m_item(NULL),
m_currentOrder(NULL),
m_itemToCatch(NULL)
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

void StackerCrane::SetWaitingState()
{
    m_state = eCS_waiting;
    if (m_currentOrder)
    {
        delete m_currentOrder;
        m_currentOrder = NULL;
    }
    if (m_item)
    {
        delete m_item;
        m_item = NULL;
    }
    m_moveToShelving = NULL;
    m_itemToCatch = NULL;
}

void StackerCrane::WaitingLogic()
{
    std::cout << "Crane waiting" << std::endl;
    m_currentOrder = Warehouse::Get()->m_dock.GetOrder();
    if (m_currentOrder)
    {
        if (m_currentOrder->m_taskType == eTT_input)
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
        else if (m_currentOrder->m_taskType == eTT_output)
        {
            sOrderOutput* order = (sOrderOutput*)m_currentOrder;
            vec2 shelvingPos;
            if (Warehouse::Get()->FindPositionOfItem(shelvingPos, this, order->itemGroup, order->itemType))
            {
                if (m_position.x != shelvingPos.x || m_position.y != shelvingPos.y)
                {
                    SetAim(shelvingPos);
                    m_state = eCS_moveToShelving;
                }
                else
                    m_state = eCS_loadingItem;
            }
        }
    }
}

void StackerCrane::CatchItemOnShelving(Item* pItem, Shelving* pShelving)
{
    m_itemToCatch = pItem;
    m_moveToShelving = pShelving;
}

void StackerCrane::MoveToDockLogic()
{
    std::cout << "Crane move to dock" << std::endl;
    vec2 moveVec = MoveToAim();
    if (moveVec.x == 0 && moveVec.y == 0) // приехали
    {
        if (m_currentOrder->m_taskType == eTT_input)
            m_state = eCS_loadingItem;
        else
            m_state = eCS_unloadingItem;
    }
    else
    {
        m_position += moveVec;
        if (m_position == m_aim) // приехали
        {
            if (m_currentOrder->m_taskType == eTT_input)
                m_state = eCS_loadingItem;
            else
                m_state = eCS_unloadingItem;
        }
    }
}

void StackerCrane::LoadingItemLogic()
{
    std::cout << "Crane loading item" << std::endl;
    if (m_currentOrder->m_taskType == eTT_input)
    {
        if(!m_item)
        {
            sOrderInput* order = (sOrderInput*)m_currentOrder;
            m_item = order->m_item;
        }
        vec2 aimPos;
        if (Shelving* pShelving = Warehouse::Get()->FindPositionOfFreeCell(aimPos))
        {
            m_moveToShelving = pShelving;
            m_state = eCS_moveToShelving;
            SetAim(aimPos);
        }
        else
            SetWaitingState();
    }
    else    // eTT_output
    {
        if (m_moveToShelving)
        {
            if (m_moveToShelving->GetItem(m_position, m_itemToCatch))
            {
                m_item = m_itemToCatch;
                m_itemToCatch = NULL;
                m_moveToShelving = NULL;
                
                vec2 docPos = Warehouse::Get()->m_dock.GetPosition();
                m_state = eCS_moveToDock;
                SetAim(docPos);
            }
            else
                SetWaitingState();
        }
        else
            SetWaitingState();
    }
}

void StackerCrane::MoveToShelvingLogic()
{
    std::cout << "Crane move to shelving" << std::endl;
    vec2 moveVec = MoveToAim();
    if (moveVec.x == 0 && moveVec.y == 0) // приехали
    {
        if (m_currentOrder->m_taskType == eTT_input)
            m_state = eCS_unloadingItem;
        else
            m_state = eCS_loadingItem;
    }
    else
    {
        m_position += moveVec;
        if (m_position == m_aim) // приехали
        {
            if (m_currentOrder->m_taskType == eTT_input)
                m_state = eCS_unloadingItem;
            else
                m_state = eCS_loadingItem;
        }
    }
}

void StackerCrane::UnloadingItemLogic()
{
    std::cout << "Crane unloading item" << std::endl;
    if (m_currentOrder->m_taskType == eTT_input)
    {
        if (m_moveToShelving)
        {
            m_moveToShelving->AddElement(m_position, m_item);
            m_item = NULL;
            SetWaitingState();
        }
    }
    else    // eTT_output
    {
        SetWaitingState();
    }
}

void StackerCrane::Simulate()
{
    if (m_state == eCS_waiting)
    {
        WaitingLogic();
    }
    else if (m_state == eCS_moveToDock)
    {
        MoveToDockLogic();
    }
    else if(m_state == eCS_loadingItem)
    {
        LoadingItemLogic();
    }
    else if(m_state == eCS_moveToShelving)
    {
        MoveToShelvingLogic();
    }
    else if(m_state == eCS_unloadingItem)
    {
        UnloadingItemLogic();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////

Dock::Dock(int _x, int _y)
:m_position(_x, _y)
{
    ;
}

sOrder* Dock::GetOrder()
{
    sOrder* result = NULL;
    if (!m_orders.empty())
    {
        result = m_orders.front();
        m_orders.pop();
    }
    return result;
}

int Dock::GetNumOfInputOrders()
{
    int counter = 0;
    return counter;
}

int Dock::GetNumOfOutputOrders()
{
    int counter = 0;
    return counter;
}

void Dock::Simulate()
{
    // на каждом шаге с вероятностью 20% добавляется новая заявка
    if (m_orders.size() < m_maxNumOrders)
    {
        static int s_inputCount = 0;
        if (s_inputCount < 4)
        {
            ++s_inputCount;
            AddInputOrder();
        }
        else
        {
            s_inputCount = 0;
            AddOutputOrder();
        }
    }
}

void Dock::AddInputOrder()
{
    if (m_orders.size() < m_maxNumOrders)
    {
        sOrderInput* newOrder = new sOrderInput;
        newOrder->m_item = Item::CreateItem((Item::eItemGroup)GetRandom(1, 3), GetRandom(1, 5));
        m_orders.push(newOrder);
    }
}

void Dock::AddOutputOrder()
{
    if (m_orders.size() < m_maxNumOrders)
    {
        sOrderOutput* newOrder = new sOrderOutput;
        newOrder->itemGroup = (Item::eItemGroup)GetRandom(1, 3);
        newOrder->itemType = GetRandom(1, 5);
        m_orders.push(newOrder);
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
    while (step < 10000)
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

bool Warehouse::FindPositionOfItem(vec2& pos, StackerCrane* crane, Item::eItemGroup itemGroup, unsigned int itemType)
{
    bool result = false;
    for (int i = 0; i < m_numShelvings; ++i)
    {
        if (Item *pItem = m_shelvings[i].FindPositionOfItem(pos, itemGroup, itemType))
        {
            pos.y += 1;
            crane->CatchItemOnShelving(pItem, &m_shelvings[i]);
            result = true;
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
    warehouse.Simulate();
}
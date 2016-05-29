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

Item* Item::CreateItem(eItemType type)
{
    Item* result = NULL;
    switch (type) {
        case eIT_generator:
            result = new Item(type, 2, 500);
            break;
        case eIT_stabilizator:
            result = new Item(type, 3, 1000);
            break;
        case eIT_transformator:
            result = new Item(type, 4, 1500);
            break;
        default:
            break;
    }
    return result;
}

const char* itemNames[] = { "generator", "stabilizator", "transformator" };

Item::Item(eItemType type, int space, int price)
:m_type(type),
m_space(space),
m_price(price)
{
    std::cout << "Create item " << itemNames[m_type] << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////

#define SHELVING_DEF_CAPACITY 3

Shelving::Shelving()
:m_capacity(SHELVING_DEF_CAPACITY),
m_freeSpace(m_capacity)
{
    
}

Shelving::Shelving(int capacity)
:m_capacity(capacity),
m_freeSpace(m_capacity)
{
    ;
}

void Shelving::SetCapacity(int capacity)
{
    if (capacity < m_items.size())
    {
        std::cout << "Shelving Error! Can't set capacity" << std::endl;
        return;
    }
    std::cout << "Shelving " << m_id << " SetCapacity " << capacity << std::endl;
    m_capacity = capacity;
    m_freeSpace = m_capacity - m_items.size();
}

bool Shelving::AddItem(Item* pItem)
{
    std::set<Item*>::iterator it = m_itemsToAdd.find(pItem);
    if (it != m_itemsToAdd.end())   // если нашли элемент в списке "забронированных" элементов
    {
        m_items.push_back(pItem);
        return true;
    }
    else
        std::cout << "Shelving Error! Can't add item" << std::endl;
    return false;
}

void Shelving::RemoveItem(Item* pItem)
{
    bool foundItem = false;
    for (std::list<Item*>::iterator it = m_items.begin(); it != m_items.end(); ++it)
    {
        if (*it == pItem)
        {
            foundItem = true;
            m_freeSpace += pItem->GetSpace();
            m_items.erase(it);
            break;
        }
    }
    if (!foundItem)
        std::cout << "Shelving Error! Can't fint item to remove" << std::endl;
}

const std::list<Item*>& Shelving::GetItemsList()
{
    return m_items;
}

bool Shelving::TryToBlockSpaceForItem(Item* pItem)
{
    if (pItem->GetSpace() <= m_freeSpace)
    {
        m_freeSpace -= pItem->GetSpace();
        m_itemsToAdd.insert(pItem);
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////

#define CRANE_DEF_CAPACITY 3

StackerCrane::StackerCrane()
:m_item(NULL),
m_state(eCS_waitOnDock)
{
    
}

bool StackerCrane::CatchItem(Item* pItem)
{
    if (!m_item)
    {
        m_item = pItem;
        return true;
    }
    else
        std::cout << "StackerCrane Error! Can't catch item" << std::endl;
    return false;
}

void StackerCrane::PutItemOnShelving(Shelving* pShelving)
{
    // если получается положить первый элемент на стеллаж, удаляем его из списка элементов крана 
    if (m_item && pShelving->AddItem(m_item))
        m_item = NULL;
    else
        std::cout << "StackerCrane Error! Can't put item on shelving" << std::endl;
}

void StackerCrane::MoveToShelving(Shelving* pShelving)
{
    m_state = eCS_moveToShelving;
    m_shelvingToMove = pShelving;
}

void StackerCrane::MoveToDock(Dock* dock)
{
    m_state = eCS_moveToDock;
    m_dockToMove = dock;
}

void StackerCrane::Simulate()
{
    switch (m_state)
    {
        case eCS_waitOnDock:
            if (m_item)
            {
                if (Warehouse* wh = Warehouse::Get())
                {
                    if (Shelving* freeShelving = wh->GetShelvingForItem(m_item))
                        MoveToShelving(freeShelving);
                }
            }
            break;
            
        case eCS_moveToShelving:
            if(m_shelvingToMove->AddItem(m_item))
            {
                m_item = NULL;
                
            }
            break;
            
        default:
            break;
    }
    
    
}

///////////////////////////////////////////////////////////////////////////////////////////

Dock::Dock(int capacity)
:m_capacity(capacity),
m_freeSpace(capacity)
{
    ;
}

bool Dock::AddItem(Item* pItem)
{
    if (pItem->GetSpace() <= m_freeSpace)
    {
        m_freeSpace -= pItem->GetSpace();
        m_items.push_back(pItem);
        return true;
    }
    else
        std::cout << "Dock Error! Can't add item, not enough free space" << std::endl;
    return false;
}

Item* Dock::GetFirstItem()
{
    if (m_items.size() > 0)
        return m_items.front();
    return NULL;
}

void Dock::RemoveItem(Item* pItem)
{
    bool foundItem = false;
    for (std::list<Item*>::iterator it = m_items.begin(); it != m_items.end(); ++it)
    {
        if (*it == pItem)
        {
            foundItem = true;
            m_freeSpace += pItem->GetSpace();
            m_items.erase(it);
            break;
        }
    }
    if (!foundItem)
        std::cout << "Dock Error! Can't fint item to remove" << std::endl;
}

const std::list<Item*>& Dock::GetItemsList()
{
    return m_items;
}

///////////////////////////////////////////////////////////////////////////////////////////

Warehouse* Warehouse::Get()
{
    return s_warehouse;
}

Warehouse::Warehouse()
:m_inputDock(50),
m_outputDock(50),
m_revenue(0)
{
    s_warehouse = this;
    for (int i = 0; i < m_numCranes; ++i)
    {
        m_cranes[i].SetId(i);
    }
    for (int i = 0; i < m_numShelvings; ++i)
    {
        m_shelvings[i].SetId(i);
        m_shelvings[i].SetCapacity(10);
    }
}

Warehouse::~Warehouse()
{
    s_warehouse = NULL;
}

void Warehouse::Simulate()
{
    long long step = 0; // шаг симуляции
    while (m_revenue < 100000)
    {
        SimulateReceivingItems();
        SimulateCranesLogic();
        ++step;
    }
}

void Warehouse::TryAddItemToDock(Item* pItem)
{
    m_inputDock.AddItem(pItem);
}

void Warehouse::SimulateReceivingItems()
{
    if (GetRandom(0, 100) > 30)
    {
        Item* pItem = Item::CreateItem((Item::eItemType)GetRandom(0, 2));
        if (pItem)
            TryAddItemToDock(pItem);
    }
}

void Warehouse::SimulateCranesLogic()
{
    for (int i = 0; i < m_numCranes; ++i)
        m_cranes[i].Simulate();
    ChooseCraneForReceivedItems();
}

StackerCrane* Warehouse::GetFreeCraneAndCatchItem(Item* pItem)
{
    for (int i = 0; i < m_numCranes; ++i)
    {
        StackerCrane& crane = m_cranes[i];
        if (crane.GetState() == StackerCrane::eCS_waitOnDock && crane.CatchItem(pItem))
            return &crane;
    }
    return NULL;
}

void Warehouse::ChooseCraneForReceivedItems()
{
    // если в доке есть свободный элемент, пытаемся найти свободный кран чтобы этот элемент отвезти на склад
    if (Item* pItem = m_inputDock.GetFirstItem())
    {
        if (StackerCrane* freeCrane = GetFreeCraneAndCatchItem(pItem))
        {
            if (Shelving* freeShelving = GetShelvingForItem(pItem))
            {
                freeCrane->MoveToShelving(freeShelving);
            }
        }
    }
}

Shelving* Warehouse::GetShelvingForItem(Item* pItem)
{
    for (int i = 0; i < m_numShelvings; ++i)
    {
        if (m_shelvings[i].TryToBlockSpaceForItem(pItem))
            return &m_shelvings[i];
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////

void sklad_main()
{
    Warehouse warehouse;
    warehouse.Simulate();
}
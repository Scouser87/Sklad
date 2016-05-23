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

Item::Item(int space)
:m_space(space)
{
    ;
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
    if (pItem->GetSpace() <= m_freeSpace)
    {
        m_freeSpace -= pItem->GetSpace();
        m_items.push_back(pItem);
        return true;
    }
    else
        std::cout << "Shelving Error! Can't add item, not enough free space" << std::endl;
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

///////////////////////////////////////////////////////////////////////////////////////////

#define CRANE_DEF_CAPACITY 3

StackerCrane::StackerCrane()
:m_capacity(CRANE_DEF_CAPACITY)
{
    
}

StackerCrane::StackerCrane(int capacity)
:m_capacity(capacity)
{
    
}

void StackerCrane::SetCapacity(int capacity)
{
    if (capacity < m_items.size())
    {
        std::cout << "StackerCrane Error! Can't set capacity" << std::endl;
        return;
    }
    std::cout << "StackerCrane " << m_id << " SetCapacity " << capacity << std::endl;
    m_capacity = capacity;
}

void StackerCrane::CatchItem(Item* pItem)
{
    if (m_items.size() < m_capacity)
        m_items.push_back(pItem);
    else
        std::cout << "StackerCrane Error! Can't catch item, not enough space" << std::endl;
}

void StackerCrane::PutItemOnShelving(Shelving* pShelving)
{
    // если получается положить первый элемент на стеллаж, удаляем его из списка элементов крана 
    if (!m_items.empty() && pShelving->AddItem(m_items.front()))
        m_items.erase(m_items.begin());
    else
        std::cout << "StackerCrane Error! Can't put item on shelving" << std::endl;
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

Warehouse::Warehouse()
:m_inputDock(50),
m_outputDock(50)
{
    for (int i = 0; i < m_numCranes; ++i)
    {
        m_cranes[i].SetId(i);
        m_cranes[i].SetCapacity(i+2);
    }
    for (int i = 0; i < m_numShelvings; ++i)
    {
        m_shelvings[i].SetId(i);
        m_shelvings[i].SetCapacity(10);
    }
}

void Warehouse::AddItemToDock(int itemSpace)
{
    Item* pItem = new Item(itemSpace);
    m_inputDock.AddItem(pItem);
}

void Warehouse::Simulate()
{
    ;
}

///////////////////////////////////////////////////////////////////////////////////////////

void sklad_main()
{
    Warehouse warehouse;
}
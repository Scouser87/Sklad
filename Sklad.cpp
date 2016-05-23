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

Item::Item(int space)
:m_space(space)
{
    ;
}

Shelving::Shelving(int capacity)
:m_capacity(capacity),
m_freeSpace(capacity)
{
    ;
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

StackerCrane::StackerCrane(int capacity)
:m_capacity(capacity)
{
    
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
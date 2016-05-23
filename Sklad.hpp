//
//  Sklad.hpp
//  Test App
//
//  Created by Sergei Makarov on 23.05.16.
//  Copyright © 2016 Sergei Makarov. All rights reserved.
//

#ifndef Sklad_hpp
#define Sklad_hpp

#include <stdio.h>
#include <list>

class Item // хранимый элемент
{
public:
    Item(int space = 1);
    
    int GetSpace() { return m_space; }
private:
    int m_space;    // сколько места занимает этот элемент на стеллаже
};

class Shelving  // стеллаж
{
public:
    Shelving();
    Shelving(int capacity);
    
    void SetId(int value) { m_id = value; }
    void SetCapacity(int capacity);
    bool AddItem(Item* pItem);
    void RemoveItem(Item* pItem);
    const std::list<Item*>& GetItemsList();
    
private:
    int     m_id;
    int     m_capacity; // вместительность стеллажа
    int     m_freeSpace;    // свободное место
    std::list<Item*> m_items;   // список элементов
};

class StackerCrane // кран-штабеллер
{
public:
    StackerCrane();
    StackerCrane(int capacity);
    
    void SetId(int value) { m_id = value; }
    void SetCapacity(int capacity);
    void CatchItem(Item* pItem);
    void PutItemOnShelving(Shelving* pShelving);
    
private:
    int m_id;
    int m_capacity; // кол-во одновременно перевозимых элементов
    std::list<Item*> m_items;   // список перевозимых элементов
};

class Dock  // док для приема/выдачи элементов со склада
{
public:
    Dock(int capacity);
    
    bool AddItem(Item* pItem);
    void RemoveItem(Item* pItem);
    const std::list<Item*>& GetItemsList();
    
private:
    int     m_capacity; // вместительность дока
    int     m_freeSpace;    // свободное место
    std::list<Item*> m_items;   // список элементов
};

class Warehouse // склад
{
public:
    Warehouse();
    
    void AddItemToDock(int itemSpace);
    
    void Simulate();
    
private:
    static const int m_numCranes = 3;
    static const int m_numShelvings = 10;
    
    Dock m_inputDock;   // док для приема элементов со склада
    Dock m_outputDock;  // док для выдачи элементов со склада
    StackerCrane    m_cranes[m_numCranes];
    Shelving        m_shelvings[m_numShelvings];
    
};

void sklad_main();

#endif /* Sklad_hpp */

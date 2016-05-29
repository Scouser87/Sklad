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
#include <set>

class Item // хранимый элемент
{
public:
    enum eItemType
    {
        eIT_transformator,
        eIT_generator,
        eIT_stabilizator
    };
    
    static Item* CreateItem(eItemType type);
    
    Item(eItemType type, int space = 1, int price = 0);
    
    int GetSpace() { return m_space; }
private:
    eItemType m_type;
    int m_space;    // сколько места занимает этот элемент на стеллаже
    int m_price;    // цена элемента в рублях
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
    
    int GetFreeSpace() { return m_freeSpace; }
    bool TryToBlockSpaceForItem(Item* pItem); // попытка "забронировать" место для элемента на стеллаже
    
private:
    int     m_id;
    int     m_capacity; // вместительность стеллажа
    int     m_freeSpace;    // свободное место
    
    std::list<Item*> m_items;   // список элементов на стеллаже
    std::set<Item*> m_itemsToAdd;  // список элементов, для которых забронировано место на стеллаже
};

class Dock;

class StackerCrane // кран-штабеллер
{
public:
    enum eCraneState
    {
        eCS_waitOnDock,
        eCS_loadingItem,
        eCS_moveToShelving,
        eCS_moveToDock
    };
    
    StackerCrane();
    
    void SetId(int value) { m_id = value; }
    bool CatchItem(Item* pItem);
    void PutItemOnShelving(Shelving* pShelving);
    
    void MoveToShelving(Shelving* pShelving);
    void MoveToDock(Dock* dock);
    
    void Simulate();
    
    eCraneState GetState() { return m_state; }
    
private:
    int m_id;
    Item* m_item;   // перевозимый элемент
    eCraneState m_state;
    
    Shelving* m_shelvingToMove; // стеллаж, к которому поедет кран
    Dock* m_dockToMove; // док, к которому поедет кран
};

class Dock  // док для приема/выдачи элементов со склада
{
public:
    Dock(int capacity);
    
    bool AddItem(Item* pItem);
    Item* GetFirstItem();
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
    static Warehouse* s_warehouse;
    static Warehouse* Get();
    Warehouse();
    ~Warehouse();
    
    void Simulate();
    
private:
    void TryAddItemToDock(Item* pItem); // попытка положить элемент в приемный док
    void SimulateReceivingItems();  // симуляция поступления элементов на склад
    void SimulateCranesLogic(); // симуляция логики кранов
    StackerCrane* GetFreeCraneAndCatchItem(Item* pItem);   // возвращает указатель на кран, свободный в данный момент, который сможет отвезти элемент
    void ChooseCraneForReceivedItems();     // пытаемся найти свободный кран чтобы этот элемент отвезти на склад
    
public:
    Shelving* GetShelvingForItem(Item* pItem);      // пытаемся найти свободный стеллаж, чтобы положить в него элемент
    
    
private:
    static const int m_numCranes = 3;
    static const int m_numShelvings = 10;
    
    Dock m_inputDock;   // док для приема элементов со склада
    Dock m_outputDock;  // док для выдачи элементов со склада
    StackerCrane    m_cranes[m_numCranes];
    Shelving        m_shelvings[m_numShelvings];
    
    int             m_revenue;  // сумма денег полученная с реализации элементов со склада
};

void sklad_main();

#endif /* Sklad_hpp */

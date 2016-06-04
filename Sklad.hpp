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
#include <queue>
#include <iomanip>
#include <string>
#include <iostream>

class vec2
{
public:
    vec2() { x = 0; y = 0; }
    
    vec2(int _x, int _y)
    :x(_x), y(_y)
    {
        
    }
    
    vec2 & operator+=(const vec2 &v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }
    
    friend int   operator == (const vec2 &a, const vec2 &b);
    
    int x;
    int y;
    
};

class Item
{
    static unsigned int s_itemIdCounter;
public:
    enum eItemGroup
    {
        eIG_zagotovka = 1,
        eIG_readyProduct,
        eIG_instrument
    };
    
    static Item* CreateItem(eItemGroup group, unsigned int type);
    
    Item(eItemGroup group, unsigned int type);
    
    int GetGroup() { return m_group; }
    int GetType() { return m_type; }
    
private:
    unsigned int    m_id;
    eItemGroup      m_group;
    unsigned int    m_type;
};

class Shelving  // стеллаж
{
public:
    Shelving();
    ~Shelving();
    
    void SetId(int value) { m_id = value; }
    void SetPosition(int _x, int _y) { m_position = vec2(_x, _y); }
    void SetSize(int _x, int _y);
    
    bool FindPositionOfFreeCell(vec2& pos); // возвращает позицию свободной ячейки в координатах склада
    Item* FindPositionOfItem(vec2& pos, Item::eItemGroup itemGroup, unsigned int itemType);
    
    void AddElement(const vec2& cranePosition, Item* pItem);
    bool GetItem(const vec2& cranePosition, Item* pItem);
    
    Item* GetItemInCell(int _x, int _y);
    
    const vec2& GetSize() { return m_size; }    
    const vec2& GetPosition() { return m_position; }
private:
    int     m_id;       // идентификатор
    vec2    m_position; // позиция стеллажа в координатах склада
    vec2    m_size;     // x - длина, y - высота
    Item*** m_cells;    // ячейки стеллажа
};

class Dock;
class sOrder;

class StackerCrane // кран-штабеллер
{
public:
    enum eTaskType
    {
        eTT_none,
        eTT_input,    // получение элементов из окна приема и отгрузка на склад
        eTT_output   // получение элементов со склада и отгрузка в окно приема
    };
    
    enum eCraneState
    {
        eCS_waiting,
        eCS_loadingItem,
        eCS_moveToShelving,
        eCS_moveToDock,
        eCS_unloadingItem
    };
    
    StackerCrane();
    
    vec2 MoveToAim();
    
    void SetWaitingState();
    void WaitingLogic();
    void CatchItemOnShelving(Item* pItem, Shelving* pShelving);
    void MoveToDockLogic();
    void LoadingItemLogic();
    void MoveToShelvingLogic();
    void UnloadingItemLogic();
    
    void Simulate();
    
    eCraneState GetState() { return m_state; }
    void SetAim(const vec2& pos) { m_aim = pos; }
    
    void SetPosition(const vec2& pos) { m_position = pos; }
    const vec2& GetPosition() { return m_position; }
private:
    eCraneState     m_state;    // текущее состояние
    vec2            m_position; // позиция в координатах склада
    vec2            m_aim;      // координаты цели
    
    Item*           m_item;             // груз, который кран везет в данный момент
    Shelving*       m_moveToShelving;   // стеллаж, к которому крану нужно проехать
    Item*           m_itemToCatch;      // груз, который кран собирается взять на стеллаже
    sOrder*         m_currentOrder;     // заявка, которую обрабатывает кран
};


class sOrder
{
public:
    sOrder():m_taskType(StackerCrane::eTT_none) { }
    
    StackerCrane::eTaskType m_taskType;
};

class sOrderInput : public sOrder
{
public:
    sOrderInput()
    {
        m_taskType = StackerCrane::eTT_input;
    }
    Item* m_item;
};

class sOrderOutput : public sOrder
{
public:
    sOrderOutput()
    {
        m_taskType = StackerCrane::eTT_output;
    }
    Item::eItemGroup itemGroup;
    unsigned int itemType;
};


class Dock  // окно для приема/выдачи элементов со склада
{
public:
    static const int m_maxNumOrders = 50;
    
    Dock(int _x, int _y);
    
    void Simulate();
    sOrder* GetOrder();
    
    int GetNumOfInputOrders();
    int GetNumOfOutputOrders();
    
    const vec2& GetPosition() { return m_position; }
    
private:
    void AddInputOrder();
    void AddOutputOrder();
    
private:
    vec2 m_position;
    
    std::queue<sOrder*> m_orders;
};

class Warehouse // склад
{
public:
    static Warehouse* s_warehouse;
    static Warehouse* Get();
    Warehouse(int _x, int _y);
    ~Warehouse();
    
    void Simulate();
    Shelving* FindPositionOfFreeCell(vec2& pos);
    bool FindPositionOfItem(vec2& pos, StackerCrane* crane, Item::eItemGroup itemGroup, unsigned int itemType);
    
    Shelving* IsInAnyShelving(const vec2& pos);
    std::string DrawShelvings(vec2 pos);
    void Draw();
    
private:
    
public:
    static const int m_numShelvings = 6;
    Dock m_dock;   // док для приема / выдачи элементов со склада
private:
    StackerCrane    m_crane;
    Shelving        m_shelvings[m_numShelvings];
    vec2            m_size;
};

void sklad_main();

#endif /* Sklad_hpp */

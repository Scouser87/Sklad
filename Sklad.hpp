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
    
    int x;
    int y;
};

class StackerCrane;

class Shelving  // стеллаж
{
public:
    Shelving();
    ~Shelving();
    
    void SetId(int value) { m_id = value; }
    void SetPosition(int _x, int _y) { m_position = vec2(_x, _y); }
    void SetSize(int _x, int _y);
    
    bool FindPositionOfFreeCell(vec2& pos); // возвращает позицию свободной ячейки в координатах склада
    bool FindPositionOfLastElement(vec2& pos); // возвращает позицию последней занятой ячейки в координатах склада
    
    void AddElement(const vec2& cranePosition);
    void RemoveElement(const vec2& cranePosition);
    
    const vec2& GetSize() { return m_size; }    
    const vec2& GetPosition() { return m_position; }
    short GetCellState(int _x); // кол-во элементов в указанной по координате X ячейке
private:
    int     m_id;       // идентификатор
    vec2    m_position; // позиция стеллажа в координатах склада
    vec2    m_size;     // x - длина, y - высота
    short** m_cells;    // ячейки стеллажа
    bool    m_isFull;   // индикатор заполненности
};

class Dock;

class StackerCrane // кран-штабеллер
{
public:
    enum eTaskType
    {
        eTT_none,
        eTT_loading,    // получение элементов из окна приема и отгрузка на склад
        eTT_unloading   // получение элементов со склада и отгрузка в окно приема
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
    void Simulate();
    void AddTask(eTaskType type);
    
    eCraneState GetState() { return m_state; }
    void SetAim(const vec2& pos) { m_aim = pos; }
    
    void SetPosition(const vec2& pos) { m_position = pos; }
    const vec2& GetPosition() { return m_position; }
private:
    eCraneState     m_state;    // текущее состояние
    eTaskType       m_currentTask;
    vec2            m_position; // позиция в координатах склада
    vec2            m_aim;  // координаты цели
    
    Shelving*       m_moveToShelving;
};

class Dock  // окно для приема/выдачи элементов со склада
{
public:
    Dock(int _x, int _y);
    
    const vec2& GetPosition() { return m_position; }
private:
    vec2 m_position;
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
    
    bool IsInAnyShelving(const vec2& pos, short& cellState);
    void Draw();
    
private:
    
private:
    static const int m_numShelvings = 6;
public:
    Dock m_inputDock;   // док для приема элементов со склада
    Dock m_outputDock;  // док для выдачи элементов со склада
private:
    StackerCrane    m_crane;
    Shelving        m_shelvings[m_numShelvings];
    vec2            m_size;
};

void sklad_main();

#endif /* Sklad_hpp */

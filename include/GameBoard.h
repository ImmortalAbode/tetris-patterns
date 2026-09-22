#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "Config.h"
#include "Piece.h"

// =====================================================================
// ПАТТЕРН: SINGLETON (Одиночка) - GameBoard
// =====================================================================
// Зачем здесь: игровое поле в программе может быть только одно, и к нему
// нужен доступ из разных мест (ввод, гравитация, отрисовка) без передачи
// ссылки через все функции. Singleton гарантирует один экземпляр класса и
// дает глобальную точку доступа к нему: GameBoard::Instance().
//
// Как реализовано:
//   - конструктор private: снаружи создать второй объект нельзя;
//   - копирование запрещено (= delete), иначе можно было бы получить копию;
//   - Instance() создает объект при первом вызове (static внутри функции,
//     "одиночка Майерса") и потокобезопасно с C++11.
// =====================================================================
class GameBoard
{
public:
    static GameBoard& Instance();

    GameBoard(const GameBoard&) = delete;
    GameBoard& operator=(const GameBoard&) = delete;

    // Что лежит в клетке: 0 - пусто, 1..7 - цвет осевшей фигуры.
    int Get(int row, int col) const 
    { 
        return m_cells[row][col]; 
    }

    // Пересекается ли фигура со стенами, полом или осевшими клетками.
    bool Collides(const Piece& piece) const;

    // Вписать фигуру в поле (она приземлилась).
    void Lock(const Piece& piece);

    // Удалить заполненные строки, вернуть их количество.
    int ClearLines();

    // Очистить поле (рестарт игры).
    void Reset();

private:
    GameBoard() = default;

    int m_cells[ROWS][COLS]{};
};

#endif // GAMEBOARD_H

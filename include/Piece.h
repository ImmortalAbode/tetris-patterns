#ifndef PIECE_H
#define PIECE_H

#include <memory>

// Падающая фигура: форма (сетка до 4x4), цвет и позиция на поле.
// Раньше все это было набором глобальных переменных (piece, pieceX, pieceY...).
//
// ПАТТЕРН: PROTOTYPE (Прототип) - см. также PieceFactory.h.
// У Piece нет указателей на другие объекты, поэтому клонирование поверхностное:
// подходит копирующий конструктор, который компилятор генерирует сам.
class Piece
{
public:
    // size - сторона квадрата фигуры, cells - клетки построчно ('X' = занято),
    // colorIndex - номер цвета (1..7).
    Piece(int size, const char* cells, int colorIndex);

    // Клонирование прототипа (Prototype::Clone).
    std::unique_ptr<Piece> Clone() const 
    { 
        return std::make_unique<Piece>(*this); 
    }

    void RotateClockwise();

    void SetPosition(int x, int y) 
    { 
        m_x = x; 
        m_y = y; 
    }
    void Move(int dx, int dy) 
    { 
        m_x += dx; 
        m_y += dy; 
    }

    bool IsFilled(int row, int col) const 
    { 
        return m_cells[row][col]; 
    }
    int Size() const 
    { 
        return m_size; 
    }
    int ColorIndex() const 
    { 
        return m_colorIndex; 
    }
    int X() const 
    { 
        return m_x; 
    }
    int Y() const 
    { 
        return m_y; 
    }

private:
    int m_size{};
    int m_colorIndex{};
    int m_x{};
    int m_y{};
    bool m_cells[4][4]{};
};

#endif // PIECE_H
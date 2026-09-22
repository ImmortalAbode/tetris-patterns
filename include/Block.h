#ifndef BLOCK_H
#define BLOCK_H

#include "GameObject.h"

// Leaf: одна клетка поля - минимальный неделимый компонент композиции.
// Хранит абсолютные координаты на поле (не смещение внутри фигуры).
class Block : public GameObject
{
public:
    Block(int col, int row, int colorIndex) : m_col{col}, m_row{row}, m_colorIndex{colorIndex} {}

    void Draw(sf::RenderTarget& target, const BlockStyle& style) const override
    {
        style.Draw(target, m_col, m_row, m_colorIndex);
    }

    bool Occupies(int col, int row) const override 
    { 
        return m_col == col && m_row == row; 
    }

    int Col() const 
    { 
        return m_col; 
    }
    int Row() const 
    { 
        return m_row; 
    }
    int ColorIndex() const 
    { 
        return m_colorIndex; 
    }

private:
    int m_col;
    int m_row;
    int m_colorIndex;
};

#endif // BLOCK_H
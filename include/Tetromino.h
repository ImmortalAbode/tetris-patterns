#ifndef TETROMINO_H
#define TETROMINO_H

#include <memory>
#include <vector>

#include "Block.h"
#include "GameObject.h"

// Composite: падающая фигура - контейнер из Block (Leaf). Заменяет прежний
// Piece с фиксированной сеткой bool[4][4]: Draw() и Occupies()
// теперь просто делегируют работу каждому потомку по очереди, а не
// перебирают клетки вручную в каждом месте, где фигура используется.
//
// ПАТТЕРН: PROTOTYPE - Clone() работает так же, как раньше у Piece.
class Tetromino : public GameObject
{
public:
    // size - сторона квадрата, в котором фигура вращается (нужна для формулы
    // поворота); cells - клетки построчно ('X' = занято), colorIndex - 1..7.
    Tetromino(int size, const char* cells, int colorIndex);

    void Draw(sf::RenderTarget& target, const BlockStyle& style) const override;
    void DrawGhost(sf::RenderTarget& target, const BlockStyle& style) const override;
    bool Occupies(int col, int row) const override;

    std::unique_ptr<Tetromino> Clone() const 
    { 
        return std::make_unique<Tetromino>(*this); 
    }

    void SetPosition(int col, int row);
    void Move(int dx, int dy);
    void RotateClockwise();

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

    // Потомки (Leaf) - то, что Composite реально рисует и опрашивает.
    const std::vector<Block>& Blocks() const 
    { 
        return m_blocks; 
    }

private:
    // Пересобрать m_blocks (абсолютные координаты) из m_shape (локальные
    // смещения внутри квадрата size x size) и текущей позиции (m_x, m_y).
    void RebuildBlocks();

    int m_size;
    int m_colorIndex;
    int m_x{0};
    int m_y{0};
    bool m_shape[4][4]{};        // та же роль, что раньше играл Piece::m_cells
    std::vector<Block> m_blocks; // потомки композиции, пересобираются при любом изменении
};

#endif // TETROMINO_H
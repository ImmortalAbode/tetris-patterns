#include "Tetromino.h"

Tetromino::Tetromino(int size, const char* cells, int colorIndex)
    : m_size{size}, m_colorIndex{colorIndex}
{
    for (int r{0}; r < m_size; ++r)
        for (int c{0}; c < m_size; ++c)
            m_shape[r][c] = (cells[r * m_size + c] == 'X');

    RebuildBlocks();
}

// Composite: нарисовать себя = попросить каждого потомка нарисовать себя.
void Tetromino::Draw(sf::RenderTarget& target, const BlockStyle& style) const
{
    for (const Block& block : m_blocks)
        block.Draw(target, style);
}

void Tetromino::DrawGhost(sf::RenderTarget& target, const BlockStyle& style) const
{
    for (const Block& block : m_blocks)
        block.DrawGhost(target, style);
}

// Composite: занимаю ли клетку (col, row) = занимает ли ее хоть один из потомков.
bool Tetromino::Occupies(int col, int row) const
{
    for (const Block& block : m_blocks)
        if (block.Occupies(col, row))
            return true;
    return false;
}

void Tetromino::SetPosition(int col, int row)
{
    m_x = col;
    m_y = row;
    RebuildBlocks();
}

void Tetromino::Move(int dx, int dy)
{
    m_x += dx;
    m_y += dy;
    RebuildBlocks();
}

// Поворот на 90° по часовой стрелке внутри квадрата m_size x m_size.
// Формула та же, что была у Piece::RotateClockwise (шаг 4.1) - применяется
// к локальным смещениям m_shape, а не напрямую к клеткам поля.
void Tetromino::RotateClockwise()
{
    bool old[4][4]{};
    for (int r{0}; r < 4; ++r)
        for (int c{0}; c < 4; ++c)
            old[r][c] = m_shape[r][c];

    for (int r{0}; r < m_size; ++r)
        for (int c{0}; c < m_size; ++c)
            m_shape[r][c] = old[m_size - 1 - c][r];

    RebuildBlocks();
}

void Tetromino::RebuildBlocks()
{
    m_blocks.clear();
    for (int r{0}; r < m_size; ++r)
        for (int c{0}; c < m_size; ++c)
            if (m_shape[r][c])
                m_blocks.emplace_back(m_x + c, m_y + r, m_colorIndex);
}
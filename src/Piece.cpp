#include "Piece.h"

Piece::Piece(int size, const char* cells, int colorIndex)
    : m_size{size}, m_colorIndex{colorIndex}
{
    for (int r{0}; r < m_size; ++r)
        for (int c{0}; c < m_size; ++c)
            m_cells[r][c] = (cells[r * m_size + c] == 'X');
}

// Поворот на 90° по часовой стрелке внутри квадрата m_size x m_size.
void Piece::RotateClockwise()
{
    bool old[4][4]{};
    for (int r{0}; r < 4; ++r)
        for (int c{0}; c < 4; ++c)
            old[r][c] = m_cells[r][c];

    for (int r{0}; r < m_size; ++r)
        for (int c{0}; c < m_size; ++c)
            m_cells[r][c] = old[m_size - 1 - c][r];
}

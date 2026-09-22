#include "GameBoard.h"

#include <cstring>

GameBoard& GameBoard::Instance()
{
    static GameBoard instance;
    return instance;
}

bool GameBoard::Collides(const Tetromino& piece) const
{
    // Composite: перебираем потомков (Blocks()) - уже готовые занятые клетки,
    // а не квадрат size x size с проверкой "занята ли эта клетка вообще".
    for (const Block& block : piece.Blocks())
    {
        int bx{ block.Col() };
        int by{ block.Row() };
        // Стены и пол.
        if (bx < 0 || bx >= COLS || by >= ROWS)
            return true;
        // Другие блоки (осевшие).
        if (by >= 0 && m_cells[by][bx])
            return true;
    }
    return false;
}

void GameBoard::Lock(const Tetromino& piece)
{
    for (const Block& block : piece.Blocks())
        m_cells[block.Row()][block.Col()] = block.ColorIndex();
}


// Удалить заполненные строки: все, что выше, сдвигается вниз.
int GameBoard::ClearLines()
{
    int cleared{0};
    int r{ ROWS - 1 };
    while (r >= 0)
    {
        bool full{true};
        for (int c{0}; c < COLS; ++c)
            if (!m_cells[r][c])
                full = false;

        if (full)
        {
            for (int rr{r}; rr > 0; --rr)
                for (int c{0}; c < COLS; ++c)
                    m_cells[rr][c] = m_cells[rr - 1][c];
            for (int c{0}; c < COLS; ++c)
                m_cells[0][c] = 0;
            ++cleared;
            // r не уменьшается: на этом месте теперь другая строка, ее тоже нужно проверить.
        }
        else
        {
            --r;
        }
    }
    return cleared;
}

void GameBoard::Reset()
{
    std::memset(m_cells, 0, sizeof(m_cells));
}

#include "ScoreManager.h"

ScoreManager& ScoreManager::Instance()
{
    static ScoreManager instance;
    return instance;
}

void ScoreManager::AddLines(int count)
{
    // Очки за 0, 1, 2, 3 и 4 линии сразу.
    static const int points[5] = {0, 100, 300, 500, 800};
    if (count > 4)
        count = 4;

    m_lines += count;
    m_score += points[count];
}

void ScoreManager::AddPiece(int colorIndex)
{
    ++m_pieces[colorIndex];
}

int ScoreManager::TotalPieces() const
{
    int total{};
    for (int i{1}; i < 8; ++i)
        total += m_pieces[i];
    return total;
}

void ScoreManager::Reset()
{
    m_score = 0;
    m_lines = 0;
    for (int& count : m_pieces)
        count = 0;
}

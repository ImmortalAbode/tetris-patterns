#ifndef SCOREMANAGER_H
#define SCOREMANAGER_H

#include <string>

// =====================================================================
// ПАТТЕРН: SINGLETON (Одиночка) - ScoreManager
// =====================================================================
// Зачем здесь: счет и статистика игры одни, а менять и читать их нужно из
// разных мест (логика падения фигур, заголовок окна, потом отчет об окончании
// игры). Singleton дает единый экземпляр и глобальный доступ:
// ScoreManager::Instance().
//
// Как реализовано: конструктор приватный, копирование запрещено (= delete),
// объект создается при первом вызове Instance() ("одиночка Майерса":
// static внутри функции; порядок инициализации между файлами не важен).
// =====================================================================
class ScoreManager
{
public:
    static ScoreManager& Instance();

    ScoreManager(const ScoreManager&) = delete;
    ScoreManager& operator=(const ScoreManager&) = delete;

    // Добавить очки за count линий, убранных одной фигурой.
    void AddLines(int count);

    // Учесть приземлившуюся фигуру (colorIndex: 1..7, как в GameBoard).
    void AddPiece(int colorIndex);

    // Сбросить счет и статистику (рестарт игры или смена режима). Режим
    // сюда не входит - его меняет отдельный вызов SetMode().
    void Reset();

    // Режим задается по аргументу командной строки при запуске, а также
    // может меняться на лету клавишей M (InputAction::SwitchMode в main.cpp) -
    // смена режима сбрасывает игру целиком, SetMode() вызывается перед Reset().
    void SetMode(std::string mode) 
    { 
        m_mode = std::move(mode); 
    }
    const std::string& Mode() const 
    { 
        return m_mode; 
    }

    int Score() const
    {
        return m_score;
    }
    int Lines() const
    {
        return m_lines;
    }
    // Уровень растет каждые 10 убранных линий (обычное соглашение Тетриса).
    // Отдельного поля не заводим - значение всегда однозначно следует из m_lines.
    int Level() const
    {
        return m_lines / 10 + 1;
    }
    int PieceCount(int colorIndex) const 
    { 
        return m_pieces[colorIndex]; 
    }
    int TotalPieces() const;

private:
    ScoreManager() = default;

    std::string m_mode{"Normal"};
    int m_score{};
    int m_lines{};
    int m_pieces[8]{};      // индекс 0 не используется, 1..7 - I, O, T, S, Z, J, L
};

#endif // SCOREMANAGER_H

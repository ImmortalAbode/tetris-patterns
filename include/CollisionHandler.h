#ifndef COLLISIONHANDLER_H
#define COLLISIONHANDLER_H

#include <memory>

#include "Tetromino.h"

// =====================================================================
// ПАТТЕРН: CHAIN OF RESPONSIBILITY (Цепочка обязанностей)
// =====================================================================
// Задание методички (лаба 6) - валидация полей ввода в диалоговых окнах -
// неприменимо буквально (в проекте нет диалоговых окон). Применяем ту же
// идею к реальной задаче: раньше GameBoard::Collides проверяла сразу все
// причины столкновения (стены, пол, другие блоки) в одном методе одним
// комбинированным условием. Теперь запрос "допустим ли ход?" идет по
// цепочке независимых обработчиков, каждый отвечает только за одну причину.
//
// Участники:
//   - Handler (CollisionHandler)                         - этот файл
//   - ConcreteHandler (Wall/Floor/BlockCollisionHandler)  - CollisionHandlers.h
//   - Client (GameBoard::Collides)                        - отправляет запрос
//     первому обработчику в готовой цепочке
// =====================================================================
class CollisionHandler
{
public:
    virtual ~CollisionHandler() = default;

    void SetSuccessor(std::unique_ptr<CollisionHandler> successor)
    {
        m_successor = std::move(successor);
    }

    // true - фигура здесь сталкивается (со стеной/полом/блоком). Если этот
    // обработчик не нашел проблему, запрос передается дальше по цепочке.
    bool HandleRequest(const Tetromino& piece) const
    {
        if (CheckCollision(piece))
            return true;
        return m_successor ? m_successor->HandleRequest(piece) : false;
    }

protected:
    // Проверка, за которую отвечает конкретный обработчик.
    virtual bool CheckCollision(const Tetromino& piece) const = 0;

private:
    std::unique_ptr<CollisionHandler> m_successor;
};

#endif // COLLISIONHANDLER_H
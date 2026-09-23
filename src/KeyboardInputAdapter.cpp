#include "KeyboardInputAdapter.h"

InputAction KeyboardInputAdapter::PollAction()
{
    while (const std::optional<sf::Event> event = m_window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            return InputAction::Quit;

        if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            switch (key->code)
            {
                case sf::Keyboard::Key::Left:      return InputAction::MoveLeft;
                case sf::Keyboard::Key::Right:     return InputAction::MoveRight;
                case sf::Keyboard::Key::Down:      return InputAction::SoftDrop;
                case sf::Keyboard::Key::Space:     return InputAction::HardDrop;
                case sf::Keyboard::Key::Up:        return InputAction::RotateCW;
                case sf::Keyboard::Key::Backspace: return InputAction::Undo;
                case sf::Keyboard::Key::T:         return InputAction::SwitchTheme;
                default: break;   // прочие клавиши игнорируем, читаем следующее событие
            }
        }
        // прочие события (изменение размера окна и т.п.) пропускаем
    }
    return InputAction::None;   // очередь событий на этом кадре пуста
}
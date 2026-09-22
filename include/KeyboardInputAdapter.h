#ifndef KEYBOARDINPUTADAPTER_H
#define KEYBOARDINPUTADAPTER_H

#include <SFML/Graphics.hpp>

#include "IInputHandler.h"

// Adapter: приводит интерфейс SFML (sf::RenderWindow::pollEvent(), sf::Event,
// коды клавиш sf::Keyboard::Key) к целевому интерфейсу IInputHandler. main
// ничего не знает про SFML-события - только про InputAction.
class KeyboardInputAdapter : public IInputHandler
{
public:
    explicit KeyboardInputAdapter(sf::RenderWindow& window) : m_window{window} {}

    InputAction PollAction() override;

private:
    sf::RenderWindow& m_window;   // адаптируемый объект (Adaptee) - окно SFML
};

#endif // KEYBOARDINPUTADAPTER_H
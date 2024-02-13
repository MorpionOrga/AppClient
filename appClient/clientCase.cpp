#include "clientCase.h"

Case::Case() : value(' ')
{
    font.loadFromFile("font/arial.ttf");

}

void Case::draw(sf::RenderWindow& window, int x, int y)
{
    letter.setFont(font);
    letter.setCharacterSize(50);
    letter.setPosition(x * 100 + 32, y * 100 + 20 + 50);
    letter.setOutlineColor(sf::Color::Black);
    letter.setOutlineThickness(2);

    if (value == 'X')
    {
        letter.setString("X");

    }
    else if (value == 'O')
    {
        letter.setString("O");
    }

    window.draw(letter);
}

char Case::getValue()
{
    return value;
}

void Case::setValue(char value)
{
    this->value = value;
}
#include "clientCase.h"

Case::Case() : pvalue(0)
{
    font.loadFromFile("font/arial.ttf");

}

void Case::draw(sf::RenderWindow& window, int x, int y , int value)
{
    letter.setFont(font);
    letter.setCharacterSize(50);
    letter.setPosition(x * 100 + 32, y * 100 + 20 + 50);
    letter.setOutlineColor(sf::Color::Black);
    letter.setOutlineThickness(2);

    if (value == 1)
    {
        letter.setString("X");
    }
    else if (value == 2)
    {
        letter.setString("O");
    }

    window.draw(letter);
}

int Case::getValue()
{
    return pvalue;
}

void Case::setValue(int value)
{
    pvalue = value;
}
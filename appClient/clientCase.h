#pragma once
#include "framework.h"

class Case {
public:
    Case();
    sf::Font font;
    sf::Text letter;
    void draw(sf::RenderWindow& window, int x, int y , int value);
    int getValue();
    void setValue(int val);
    int pvalue;
};

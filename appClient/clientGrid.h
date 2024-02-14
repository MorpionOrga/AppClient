#pragma once
#include "clientCase.h"
#include "valueSend.h"
const int gridSize = 3;

class Grid {
public:
    Grid();

    void handleEvent(Message* sendMSG, sf::Event event, SOCKET& hsocket);
    void handleMessage(int x, int y, int value);
    void drawMenu(sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    void update();
    bool checkWin(char player);
    bool isFull();
    sf::Text whoPlay;
    sf::Font font;
    sf::Text inputText;
    sf::Text play;
    sf::RectangleShape playButton;
    int whoWin;
    bool egalite;
    bool Xplay;

private:
    Case grid[gridSize][gridSize];
};

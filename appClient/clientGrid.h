#include "clientCase.h"
#include "valueSend.h"
const int gridSize = 3;

class Grid {
public:
    Grid();

    void handleEvent(sf::Event event , SOCKET& hsocket);
    void drawMenu(sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    void update();
    bool checkWin(char player);
    bool isFull();
    sf::Text whoPlay;
    sf::Font font;
    sf::Text inputText;
    sf::RectangleShape playButton;

private:
    Case grid[gridSize][gridSize];
    bool Xplay;
    Message sendMSG;
};

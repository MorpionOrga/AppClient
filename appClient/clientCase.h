#include <SFML/Graphics.hpp>

class Case {
public:
    Case();
    sf::Font font;
    sf::Text letter;
    void draw(sf::RenderWindow& window, int x, int y);
    char getValue();
    void setValue(char val);

private:
    char value;
};

#include "clientGrid.h"


Grid::Grid() : Xplay(true)
{
    for (int i = 0; i < gridSize; ++i)
    {
        for (int j = 0; j < gridSize; ++j)
        {
            grid[i][j] = Case();
        }
    }
}

void Grid::handleEvent(Message* sendMSG, sf::Event event, SOCKET& hsocket)
{
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        sendMSG->move(sendMSG->namePlayer , ((event.mouseButton.y - 50) / 100), (event.mouseButton.x / 100) , hsocket);
    }
}

void Grid::handleMessage(int x, int y, int value)
{ 
    grid[x][y].setValue(value);
}

void Grid::update()
{
    font.loadFromFile("font/arial.ttf");
    whoPlay.setFont(font);
    whoPlay.setCharacterSize(20);
    whoPlay.setString(Xplay ? "Tour de X" : "Tour de O");
    whoPlay.setPosition(110, 10);
}

void Grid::drawMenu(sf::RenderWindow& window)
{
    font.loadFromFile("font/arial.ttf");

    inputText.setFont(font);
    inputText.setCharacterSize(20);
    inputText.setFillColor(sf::Color::White);
    inputText.setPosition(50, 50);

    window.draw(inputText);

    playButton.setSize(sf::Vector2f(100, 50));
    playButton.setFillColor(sf::Color::Magenta);
    playButton.setPosition(100, 250);

    window.draw(playButton);

    play.setFont(font);
    play.setCharacterSize(20);
    play.setFillColor(sf::Color::Red);
    play.setString("Play");
    play.setPosition(120, 220);

    window.draw(play);
}

void Grid::draw(sf::RenderWindow& window)
{
    window.draw(whoPlay);

    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            sf::RectangleShape cellRect(sf::Vector2f(100.0f, 100.0f));
            cellRect.setPosition(i * 100, j * 100 + 50); // Ajoutez un décalage vertical pour éviter la superposition
            cellRect.setOutlineColor(sf::Color::Black);
            cellRect.setOutlineThickness(2.0f);

            window.draw(cellRect);
            grid[i][j].draw(window, i, j , grid[i][j].getValue());
        }
    }
}

bool Grid::checkWin(char player)
{
    // Vérification des lignes
    for (int i = 0; i < gridSize; ++i)
    {
        if (grid[i][0].getValue() == player && grid[i][1].getValue() == player && grid[i][2].getValue() == player)
        {
            return true;
        }
    }

    // Vérification des colonnes
    for (int j = 0; j < gridSize; ++j)
    {
        if (grid[0][j].getValue() == player && grid[1][j].getValue() == player && grid[2][j].getValue() == player)
        {
            return true;
        }
    }

    // Vérification des diagonales
    if (grid[0][0].getValue() == player && grid[1][1].getValue() == player && grid[2][2].getValue() == player)
    {
        return true;
    }

    if (grid[0][2].getValue() == player && grid[1][1].getValue() == player && grid[2][0].getValue() == player)
    {
        return true;
    }

    return false;
}

bool Grid::isFull()
{
    for (int i = 0; i < gridSize; ++i)
    {
        for (int j = 0; j < gridSize; ++j)
        {
            if (grid[i][j].getValue() == ' ')
            {
                return false;
            }
        }
    }
    return true;
}
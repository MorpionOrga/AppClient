#pragma once
#include "framework.h"

class Message {
public:
    Message();

    void player(std::string name, SOCKET& socket);
    void move(int x, int y, SOCKET& socket);

private:
    std::string namePlayer;
};
#pragma once
#include "framework.h"

class Message {
public:
    Message();

    void player(std::string name, SOCKET& socket);
    void move(std::string name, int x, int y, SOCKET& socket);
    std::string namePlayer;
    
};
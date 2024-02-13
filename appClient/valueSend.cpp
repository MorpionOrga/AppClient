#include "valueSend.h"

Message::Message()
{

}

void Message::player(std::string name, SOCKET& socket)
{
    rapidjson::Document message;

    message.SetObject();

    message.AddMember("type", "player", message.GetAllocator());

    rapidjson::Value Name;
    Name.SetString(name.c_str(), message.GetAllocator());
    message.AddMember("name", Name, message.GetAllocator());
    namePlayer = name;

    //Permet de stocker le message en type json
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    message.Accept(writer);

    // Convertit le JSON en cha�ne de caract�res
    send(socket, buffer.GetString(), buffer.GetLength(), 0);
}

void Message::move(std::string name , int x, int y, SOCKET& socket)
{
    rapidjson::Document message;

    message.SetObject();

    message.AddMember("type", "move", message.GetAllocator());

    rapidjson::Value X;
    X.SetInt(x);
    message.AddMember("x", X, message.GetAllocator());

    rapidjson::Value Y;
    Y.SetInt(y);
    message.AddMember("y", Y, message.GetAllocator());

    rapidjson::Value Name;
    Name.SetString(name.c_str(), message.GetAllocator());
    message.AddMember("name", Name, message.GetAllocator());

    std::cout << "name" << name << std::endl;
    std::cout << "name.cstr" << name.c_str() << std::endl;
    //Permet de stocker le message en type json
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    message.Accept(writer);

    // Convertit le JSON en cha�ne de caract�res
    send(socket, buffer.GetString(), buffer.GetLength(), 0);
}
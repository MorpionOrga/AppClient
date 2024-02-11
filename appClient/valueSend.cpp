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


    //Permet de stocker le message en type json
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    message.Accept(writer);

    // Convertit le JSON en chaîne de caractères
    send(socket, buffer.GetString(), buffer.GetLength(), 0);
}

void Message::move(int x, int y, SOCKET& socket)
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


    //Permet de stocker le message en type json
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    message.Accept(writer);

    // Convertit le JSON en chaîne de caractères
    send(socket, buffer.GetString(), buffer.GetLength(), 0);
}
// WindowsProject1.cpp : Définit le point d'entrée de l'application.
//
#pragma comment(lib, "ws2_32.lib")

#include "framework.h"
#include "appClient.h"
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#define MAX_LOADSTRING 100
#define _CRT_SECURE_NO_WARNINGS

// Variables globales :
HINSTANCE hInst;                                // instance actuelle
WCHAR szTitle[MAX_LOADSTRING];                  // Texte de la barre de titre
WCHAR szWindowClass[MAX_LOADSTRING];            // nom de la classe de fenêtre principale

// Déclarations anticipées des fonctions incluses dans ce module de code :
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void createMessage(std::string type, int x, int y, SOCKET& socket)
{
    rapidjson::Document message;

    message.SetObject();

    rapidjson::Value Type;
    Type.SetString(type.c_str(), message.GetAllocator());
    message.AddMember("type", Type, message.GetAllocator());

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

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Placez le code ici.

    // Initialise les chaînes globales
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_APPCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);


    int iResult;
    // initialisation de winsock
    WSADATA wsaData;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // CrÃ©ation de la socket
    SOCKET hsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hsocket == INVALID_SOCKET) {
        printf("socket failed\n");
        WSACleanup();
        return 1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");;


    // Connexion au serveur
    if (connect(hsocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        printf("Connect failed\n");
        closesocket(hsocket);
        WSACleanup();
        return 1;
    }

    if (WSAAsyncSelect(hsocket, hWnd, WM_RESPONCE, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR) {
        printf("WSAAsyncSelect failed for clientSocket\n");
        closesocket(hsocket);
        WSACleanup();
        return 1;
    }
    else {
        std::cout << "WSAAsyncSelect successful for clientSocket\n";
    }


    createMessage("move", 0, 0, hsocket);

    std::cout << "Responce successful\n";
    char buffer[4096];
    int bytesRead = recv(hsocket, buffer, sizeof(buffer), 0);

    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::cout << "Message du serveur : " << buffer << std::endl;
    }
    else if (bytesRead == 0) {
        // La connexion a été fermée par le client
        std::cout << "Client disconnected." << std::endl;
        closesocket(hsocket);
        hsocket = INVALID_SOCKET;
    }
    else {
        std::cout << "Erreur lors de la reception des donnees du client." << std::endl;
        closesocket(hsocket);
        hsocket = INVALID_SOCKET;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_APPCLIENT));

    MSG msg;

    // Boucle de messages principale :
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Nettoyage de Winsock
    WSACleanup();

    return (int)msg.wParam;
}


//
//  FONCTION : MyRegisterClass()
//
//  OBJECTIF : Inscrit la classe de fenêtre.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_APPCLIENT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_APPCLIENT);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FONCTION : InitInstance(HINSTANCE, int)
//
//   OBJECTIF : enregistre le handle d'instance et crée une fenêtre principale
//
//   COMMENTAIRES :
//
//        Dans cette fonction, nous enregistrons le handle de l'instance dans une variable globale, puis
//        nous créons et affichons la fenêtre principale du programme.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Stocke le handle d'instance dans la variable globale

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FONCTION : WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  OBJECTIF : Traite les messages pour la fenêtre principale.
//
//  WM_COMMAND  - traite le menu de l'application
//  WM_PAINT    - Dessine la fenêtre principale
//  WM_DESTROY  - génère un message d'arrêt et retourne
//
//

SOCKET Accept;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_RESPONCE:
    {
        // connexion des cleitn 
        std::cout << "TOTO successful\n";

        if (WSAAsyncSelect(Accept, hWnd, WM_READ, FD_READ | FD_CLOSE) == SOCKET_ERROR) {
            printf("WSAAsyncSelect failed for clientSocket\n");
            closesocket(Accept);
            WSACleanup();
            return 1;
        }
        else {
            std::cout << "WSAAsyncSelect successful for clientSocket\n";
        }
        break;
    }
    case WM_READ:
    {
        // message des clients
        std::cout << "TATA successful\n";
        char buffer[4096];
        int bytesRead = recv(Accept, buffer, sizeof(buffer), 0);
        const char* message = "Test! Again";
        send(Accept, message, strlen(message), 0);
        std::cout << "Message envoyé au client: " << message << std::endl;

        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << "Message du client : " << buffer << std::endl;
        }
        else if (bytesRead == 0) {
            // La connexion a été fermée par le client
            std::cout << "Client disconnected." << std::endl;
            closesocket(Accept);
            Accept = INVALID_SOCKET;
        }
        else {
            std::cout << "Erreur lors de la réception des données du client." << std::endl;
            closesocket(Accept);
            Accept = INVALID_SOCKET;
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

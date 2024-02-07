//Empeche d'avoir certains avertissements liÃ©s Ã  l'utilisation de fonctions risquÃ©es
#define _CRT_SECURE_NO_WARNINGS

//On se lie Ã  la bibliothÃ¨que ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

//Include que nous allons utiliser
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <iostream>
#include "framework.h"
#include "appClient.h"

//DÃ©finie le port
//#define PORT 80
#define PORT 14843
#define MAX_LOADSTRING 100

// Variables globales :
HINSTANCE hInst;                                // instance actuelle
WCHAR szTitle[MAX_LOADSTRING];                  // Texte de la barre de titre
WCHAR szWindowClass[MAX_LOADSTRING];            // nom de la classe de fenÃªtre principale

// DÃ©clarations anticipÃ©es des fonctions incluses dans ce module de code :
ATOM                MyRegisterClass(HINSTANCE hInstance);
//BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HWND InitInstanceW(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd = CreateWindowW(L"WNDCLASS", L"", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    return hWnd;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Placez le code ici.
    hInstance = GetModuleHandle(0);
    MyRegisterClass(hInstance);
    HWND hWnd = InitInstanceW(hInstance, 0);

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

    if (WSAAsyncSelect(hsocket, hWnd, WM_RESPONCE, FD_READ | FD_CLOSE) == SOCKET_ERROR) {
        printf("WSAAsyncSelect failed for clientSocket\n");
        closesocket(hsocket);
        WSACleanup();
        return 1;
    }
    else {
        std::cout << "WSAAsyncSelect successful for clientSocket\n";
    }

    const char* message = "Test!";
    send(hsocket, message, strlen(message), 0);
    std::cout << "Message envoyé au serveur : " << message << std::endl;

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
        std::cout << "Erreur lors de la réception des données du client." << std::endl;
        closesocket(hsocket);
        hsocket = INVALID_SOCKET;
    }

    MSG msg;
    // Boucle de messages principale :
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


    // Fermeture du socket du client
    closesocket(hsocket);

    // Nettoyage de Winsock
    WSACleanup();
    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = 0;
    wcex.hCursor = 0;
    wcex.hbrBackground = 0;
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = L"WNDCLASS";
    wcex.hIconSm = 0;

    return RegisterClassExW(&wcex);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_RESPONCE:
    {
        // message des clients
        std::cout << "Responce receive" << std::endl;
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

// Gestionnaire de messages pour la boÃ®te de dialogue Ã€ propos de.
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

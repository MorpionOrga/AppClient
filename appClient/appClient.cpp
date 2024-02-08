//Empeche d'avoir certains avertissements liÃ©s Ã  l'utilisation de fonctions risquÃ©es
#define _CRT_SECURE_NO_WARNINGS
// Empeche erreurs SFML/Rect.inl
#define NOMINMAX
//On se lie Ã  la bibliothÃ¨que ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

//Include que nous allons utiliser
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "framework.h"
#include "appClient.h"


//#define PORT 80
#define PORT 14843  //Définie le port
#define MAX_LOADSTRING 100 

// Variables globales :
HINSTANCE hInst;                                // Instance actuelle
WCHAR szTitle[MAX_LOADSTRING];                  // Texte de la barre de titre
WCHAR szWindowClass[MAX_LOADSTRING];            // Nom de la classe de fenetre principale

// Déclarations anticipées des fonctions incluses dans ce module de code :
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Placez le code ici.
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_APPCLIENT));
    MSG msg = { 0 };

    int iResult;
    // Initialisation de winsock
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


    // Liaison du socket / Connexion au serveur
    if (connect(hsocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        printf("Connect failed\n");
        closesocket(hsocket);
        WSACleanup();
        return 1;
    }
    else {
        std::cout << "Connecté au serveur.\n";

        // Envoie d'un message pour indiquer que le client est prêt à recevoir des messages
        const char* readyMessage = "Ready to receive messages";
        send(hsocket, readyMessage, strlen(readyMessage), 0);
    }


    // Envoie d'un message
    const char* message = "Test!";
    send(hsocket, message, strlen(message), 0);
    std::cout << "Message envoyÃ© au serveur : " << message << std::endl;

    //Reception du message
    char buffer[4096];
    int bytesRead = recv(hsocket, buffer, sizeof(buffer), 0);
    if (bytesRead > 0) {
        // Message reçu avec succès
        buffer[bytesRead] = '\0'; // Ajouter le terminateur de chaîne
        std::cout << "Message du serveur : " << buffer << std::endl;
    }
    else if (bytesRead == 0) {
        // La connexion a été fermée par le serveur
        std::cout << "Connexion fermée par le serveur." << std::endl;
    }
    else {
        // Gestion de l'erreur
        std::cerr << "Erreur lors de la réception du message du serveur." << std::endl;
    }

    // Initialise les chaÃ®nes globales
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_APPCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Effectue l'initialisation de l'application :
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    // Boucle de messages principale :
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Fermeture du socket du client
    closesocket(hsocket);

    // Nettoyage de Winsock
    WSACleanup();
    return (int)msg.wParam;
}



//
//  FONCTION : MyRegisterClass()
//
//  OBJECTIF : Inscrit la classe de fenÃªtre.
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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPCLIENT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_APPCLIENT);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FONCTION : InitInstance(HINSTANCE, int)
//
//   OBJECTIF : enregistre le handle d'instance et crÃ©e une fenÃªtre principale
//
//   COMMENTAIRES :
//
//        Dans cette fonction, nous enregistrons le handle de l'instance dans une variable globale, puis
//        nous crÃ©ons et affichons la fenÃªtre principale du programme.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Stocke le handle d'instance dans la variable globale

    // Créer la fenêtre Windows
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    // Créer une fenêtre SFML à partir de la poignée HWND
    sf::RenderWindow window;
    window.create(hWnd);

    // Créer un carré SFML
    sf::RectangleShape square(sf::Vector2f(100, 100));
    square.setFillColor(sf::Color::Red);
    square.setPosition(100, 100);

    // Afficher la fenêtre Windows
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Boucle principale
    MSG msg;
    while (window.isOpen())
    {
        // Gérer les messages de fenêtre Windows
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                window.close();

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Gérer les événements SFML
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(square);
        window.display();
    }

    return TRUE;
}



//
//  FONCTION : WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  OBJECTIF : Traite les messages pour la fenÃªtre principale.
//
//  WM_COMMAND  - traite le menu de l'application
//  WM_PAINT    - Dessine la fenÃªtre principale
//  WM_DESTROY  - gÃ©nÃ¨re un message d'arrÃªt et retourne
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Analyse les sélections de menu :
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Ajoutez ici le code de dessin qui utilise hdc...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_USER + 1: // Ajoutez ce cas pour traiter le message reçu du serveur
    {
        // Extrait le message reçu du serveur à partir de lParam
        const char* message = reinterpret_cast<const char*>(lParam);

        // Convertit le message en format WCHAR
        int len = MultiByteToWideChar(CP_UTF8, 0, message, -1, NULL, 0);
        wchar_t* wideMessage = new wchar_t[len];
        MultiByteToWideChar(CP_UTF8, 0, message, -1, wideMessage, len);

        // Affiche le message dans une boîte de dialogue
        MessageBox(hWnd, wideMessage, L"Message du serveur", MB_OK);

        // Nettoie la mémoire allouée pour le message converti
        delete[] wideMessage;
    }
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

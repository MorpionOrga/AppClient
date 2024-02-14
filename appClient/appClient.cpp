//Empeche d'avoir certains avertissements liÃƒÂ©s ÃƒÂ  l'utilisation de fonctions risquÃƒÂ©es
#define _CRT_SECURE_NO_WARNINGS
// Empeche erreurs SFML/Rect.inl
#define NOMINMAX
//On se lie ÃƒÂ  la bibliothÃƒÂ¨que ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

#include "SFML-2.6.1/include/SFML/Graphics.hpp"

#include "framework.h"
#include "appClient.h"
#include "clientGrid.h"
#include "valueSend.h"

Grid gameGrid;

#define MAX_LOADSTRING 100
#define _CRT_SECURE_NO_WARNINGS

// Variables globalesÂ :
HINSTANCE hInst;                                // instance actuelle
WCHAR szTitle[MAX_LOADSTRING];                  // Texte de la barre de titre
WCHAR szWindowClass[MAX_LOADSTRING];            // nom de la classe de fenÃªtre principale

// DÃ©clarations anticipÃ©es des fonctions incluses dans ce module de codeÂ :
void UpdateSFMLMessage(const std::string& message);

ATOM                MyRegisterClass(HINSTANCE hInstance);
HWND                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void handleMessage(const std::string& jsonRequest, SOCKET hsocket)
{
    rapidjson::Document document;
    document.Parse(jsonRequest.c_str());

    if (!document.HasParseError() || document.IsObject()) {
        const char* messageType = document["type"].GetString();

        if (std::strcmp(messageType, "move") == 0) {
            int x = document["x"].GetInt();
            int y = document["y"].GetInt();
            int value = document["value"].GetInt();
            gameGrid.handleMessage(y, x, value);
        }
        else if (std::strcmp(messageType, "win") == 0)
        {
            gameGrid.whoWin = document["value"].GetInt();
            std::cout << "Win" << std::endl;
        }
        else if (std::strcmp(messageType, "egalite") == 0)
        {
            gameGrid.egalite = document["value"].GetBool();
            std::cout << "Egalite" << std::endl;
        }
        else {
            std::cerr << "Unknown message type: " << messageType << std::endl;
        }
    }
    else {
        std::cerr << "Invalid JSON format. Expected an object." << std::endl;
    }
}

//Ajouter console 
void RedirectIOToConsole()
{
    // Alloue une console
    AllocConsole();

    // RÃ©cupÃ¨re la poignÃ©e de la console
    HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    int SystemOutput = _open_osfhandle((intptr_t)ConsoleHandle, _O_TEXT);

    // Redirige stdout vers la console
    FILE* COutputHandle;
    freopen_s(&COutputHandle, "CONOUT$", "w", stdout);

    // Assure que stdout n'est pas mis en mÃ©moire tampon
    setvbuf(stdout, NULL, _IONBF, 0);
}

// DÃ©claration globale de la chaÃ®ne de caractÃ¨res pour stocker le message du serveur
std::string serverMessage;
// Fonction pour mettre Ã  jour le message affichÃ© dans la fenÃªtre SFML
void UpdateSFMLMessage(const std::string& message)
{
    serverMessage = message;
    std::cout << "Message mis a jour";
    std::cout << message;
    std::cout << serverMessage;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Placez le code ici.

    RedirectIOToConsole();// Appel de la fonction de redirection de la sortie vers la console

    // Initialise les chaÃ®nes globales
    Message sendMSG;
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_APPCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Effectue l'initialisation de l'application :
    HWND hWnd = InitInstance(hInstance, nCmdShow);

    int iResult;
    // Initialisation de winsock
    WSADATA wsaData;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << ("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // CrÃƒÂ©ation de la socket
    SOCKET hsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hsocket == INVALID_SOCKET) {
        std::cout << ("socket failed\n");
        WSACleanup();
        return 1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");;


    // Connexion au serveur
    if (connect(hsocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cout << ("Connect failed\n");
        closesocket(hsocket);
        WSACleanup();
        return 1;
    }
    else {
        std::cout << "ConnectÃ© au serveur.\n";

        // Envoie d'un message pour indiquer que le client est prÃªt Ã  recevoir des messages
        const char* readyMessage = "Ready to receive messages";
        send(hsocket, readyMessage, strlen(readyMessage), 0);
    }


    if (WSAAsyncSelect(hsocket, hWnd, WM_READ, FD_READ | FD_CLOSE) == SOCKET_ERROR) {
        std::cout<<("WSAAsyncSelect failed for clientSocket\n");
        closesocket(hsocket);
        WSACleanup();
        return 1;
    }
    else {
        std::cout << "WSAAsyncSelect successful for clientSocket\n";
    }

    ////Reception du message
    //char buffer[4096];
    //int bytesRead = recv(hsocket, buffer, sizeof(buffer), 0);
    //if (bytesRead > 0) {
    //    // Message reÃ§u avec succÃ¨s
    //    buffer[bytesRead] = '\0'; // Ajouter le terminateur de chaÃ®ne
    //    std::cout << "Message du serveur : " << buffer << std::endl;

    //    // Mettre Ã  jour le message affichÃ© dans la fenÃªtre SFML avec le message reÃ§u du serveur
    //    UpdateSFMLMessage(buffer);
    //}

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_APPCLIENT));

    MSG msg;

    sf::RenderWindow window(sf::VideoMode(300, 350), "Morpion");
    //sf::RenderWindow* pWindow = &window; // Garder une rÃ©fÃ©rence Ã  la fenÃªtre SFML

    std::string pseudo;
    bool choosepseudo = true;


    while (window.isOpen())
    {
        if (choosepseudo)
        {
            sf::Event menuevent;
            while (window.pollEvent(menuevent))
            {
                if (menuevent.type == sf::Event::Closed)
                {
                    window.close();
                    closesocket(hsocket);
                    WSACleanup();
                    return 1;
                }
                else if (menuevent.type == sf::Event::TextEntered)
                {
                    if (menuevent.text.unicode < 128)
                    {
                        pseudo += (menuevent.text.unicode);
                        gameGrid.inputText.setString(pseudo);
                    }
                }
                else if (menuevent.type == sf::Event::MouseButtonPressed)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (gameGrid.playButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                    {
                        if (pseudo != "") {
                            sendMSG.player(pseudo, hsocket);
                            choosepseudo = false;
                        }
                    }
                }
            }
            window.clear();
            gameGrid.drawMenu(window);
            window.display();
        }
        else
        {
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);  
            }

            // -- Logique du jeu
            // le jeu ...
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    closesocket(hsocket);
                    WSACleanup();
                    return 1;
                }
                gameGrid.handleEvent(&sendMSG ,event, hsocket);
            }
            gameGrid.update();

            // -- Rendu

            //pWindow->clear();
            window.clear();
            gameGrid.draw(window);

            window.display();
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
//  OBJECTIF : Inscrit la classe de fenÃƒÂªtre.
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
//   OBJECTIF : enregistre le handle d'instance et crÃƒÂ©e une fenÃƒÂªtre principale
//
//   COMMENTAIRES :
//
//        Dans cette fonction, nous enregistrons le handle de l'instance dans une variable globale, puis
//        nous crÃƒÂ©ons et affichons la fenÃƒÂªtre principale du programme.
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Stocke le handle d'instance dans la variable globale

    // CrÃ©e la fenÃªtre Windows
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    //// Affiche la fenÃªtre Windows
    //ShowWindow(hWnd, nCmdShow);
    //UpdateWindow(hWnd);

    return hWnd;
}

//
//  FONCTION : WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  OBJECTIF : Traite les messages pour la fenÃƒÂªtre principale.
//
//  WM_COMMAND  - traite le menu de l'application
//  WM_PAINT    - Dessine la fenÃƒÂªtre principale
//  WM_DESTROY  - gÃƒÂ©nÃƒÂ¨re un message d'arrÃƒÂªt et retourne
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_READ:
    {
        std::cout << "read" << std::endl;
        // message des clients
        char buffer[4096];
        int bytesRead = recv(wParam, buffer, sizeof(buffer), 0);
        buffer[bytesRead] = '\0';
        handleMessage(buffer , wParam);
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



// Gestionnaire de messages pour la boÃƒÂ®te de dialogue Ãƒâ‚¬ propos de.
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


/*
- On doit faire le jeu dans le fenetre gÃ©nÃ©rÃ© par CreateWindowW dÃ©jÃ  prÃ©sent Ã  la crÃ©ation du projet ?
- Pourquoi utiliser wWinMain et pas un simple main comme dans le serveur ?
*/
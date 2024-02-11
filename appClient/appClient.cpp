//Empeche d'avoir certains avertissements liÃ©s Ã  l'utilisation de fonctions risquÃ©es
#define _CRT_SECURE_NO_WARNINGS
// Empeche erreurs SFML/Rect.inl
#define NOMINMAX
//On se lie Ã  la bibliothÃ¨que ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

#include "SFML-2.6.1/include/SFML/Graphics.hpp"

#include "framework.h"
#include "appClient.h"
#include "clientGrid.h"
#include "valueSend.h"


#define MAX_LOADSTRING 100
#define _CRT_SECURE_NO_WARNINGS

// Variables globales :
HINSTANCE hInst;                                // instance actuelle
WCHAR szTitle[MAX_LOADSTRING];                  // Texte de la barre de titre
WCHAR szWindowClass[MAX_LOADSTRING];            // nom de la classe de fenêtre principale

// Déclarations anticipées des fonctions incluses dans ce module de code :
void UpdateSFMLMessage(const std::string& message);

ATOM                MyRegisterClass(HINSTANCE hInstance);
HWND                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);



//Ajouter console 
void RedirectIOToConsole()
{
    // Alloue une console
    AllocConsole();

    // Récupère la poignée de la console
    HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    int SystemOutput = _open_osfhandle((intptr_t)ConsoleHandle, _O_TEXT);

    // Redirige stdout vers la console
    FILE* COutputHandle;
    freopen_s(&COutputHandle, "CONOUT$", "w", stdout);

    // Assure que stdout n'est pas mis en mémoire tampon
    setvbuf(stdout, NULL, _IONBF, 0);
}

// Déclaration globale de la chaîne de caractères pour stocker le message du serveur
std::string serverMessage;
// Fonction pour mettre à jour le message affiché dans la fenêtre SFML
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
   
    // Initialise les chaînes globales
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
    else {
        std::cout << "Connecté au serveur.\n";

        // Envoie d'un message pour indiquer que le client est prêt à recevoir des messages
        const char* readyMessage = "Ready to receive messages";
        send(hsocket, readyMessage, strlen(readyMessage), 0);
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

    //Reception du message
    char buffer[4096];
    int bytesRead = recv(hsocket, buffer, sizeof(buffer), 0);
    if (bytesRead > 0) {
        // Message reçu avec succès
        buffer[bytesRead] = '\0'; // Ajouter le terminateur de chaîne
        std::cout << "Message du serveur : " << buffer << std::endl;

        // Mettre à jour le message affiché dans la fenêtre SFML avec le message reçu du serveur
        UpdateSFMLMessage(buffer);
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_APPCLIENT));

    MSG msg;

    sf::RenderWindow window(sf::VideoMode(300, 350), "Morpion");
    //sf::RenderWindow* pWindow = &window; // Garder une référence à la fenêtre SFML

    Grid gameGrid;
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
                        std::cout << pseudo << std::endl;
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
                if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
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
                gameGrid.handleEvent(event, hsocket);
            }
            gameGrid.update();

            // -- Rendu

            //pWindow->clear();
            window.clear();
            gameGrid.draw(window);

            window.display();

            if (gameGrid.checkWin('X')) {
                std::cout << "X win" << std::endl;
                window.close();
            }
            else if (gameGrid.checkWin('O')) {
                std::cout << "O win" << std::endl;
                window.close();
            }
            else if (gameGrid.isFull()) {
                std::cout << "Egalite" << std::endl;
                window.close();
            }
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
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Stocke le handle d'instance dans la variable globale

    // Crée la fenêtre Windows
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    //// Affiche la fenêtre Windows
    //ShowWindow(hWnd, nCmdShow);
    //UpdateWindow(hWnd);

    return hWnd;
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
    //static sf::RenderWindow* pWindow = nullptr; // Garder une référence à la fenêtre SFML
    //static sf::RectangleShape square(sf::Vector2f(100, 100)); // Carré SFML

    switch (message)
    {
    case WM_CREATE:
    {
        //// Créer la fenêtre SFML à partir de la poignée HWND
        //pWindow = new sf::RenderWindow();
        //pWindow->create(hWnd);

        //// Paramètres du carré SFML
        //square.setFillColor(sf::Color::Red);
        //square.setPosition(100, 100);
        //break;
    }
    case WM_PAINT:
    {
        //pWindow->clear();
        //pWindow->draw(square);

        //// Affichez le message du serveur dans la fenêtre SFML
        //sf::Font font;
        //if (font.loadFromFile("font/arial.ttf")) { // Assurez-vous que le fichier arial.ttf est dans le même répertoire que votre exécutable
        //    sf::Text text;
        //    text.setFont(font);
        //    text.setString(serverMessage);
        //    text.setCharacterSize(24);
        //    text.setFillColor(sf::Color::White);
        //    text.setPosition(100, 300);
        //    pWindow->draw(text);
        //}

        //pWindow->display();
        //break;
    }
    case WM_DESTROY:
    {
        // Fermer la fenêtre SFML et libérer la mémoire
        /*if (pWindow)
        {
            pWindow->close();
            delete pWindow;
            pWindow = nullptr;
        }
        PostQuitMessage(0);
        break;*/
    }
    case WM_USER + 1: // #define WM_MAVARIABLE (WM_USER + 1)
    {
        // Recevez le message du serveur et mettez à jour la fenêtre SFML
        /*const char* message = reinterpret_cast<const char*>(lParam);
        UpdateSFMLMessage(message);
        break;*/
    }
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


/*
- On doit faire le jeu dans le fenetre généré par CreateWindowW déjà présent à la création du projet ?
- Pourquoi utiliser wWinMain et pas un simple main comme dans le serveur ?
*/
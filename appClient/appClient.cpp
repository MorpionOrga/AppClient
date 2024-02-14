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
#include "Thread.h"

Grid gameGrid;

#define MAX_LOADSTRING 100
#define _CRT_SECURE_NO_WARNINGS

// Variables globalesÂ :
CRITICAL_SECTION critical;
HINSTANCE hInst;                                // instance actuelle
WCHAR szTitle[MAX_LOADSTRING];                  // Texte de la barre de titre
WCHAR szWindowClass[MAX_LOADSTRING];            // nom de la classe de fenÃªtre principale

// DÃ©clarations anticipÃ©es des fonctions incluses dans ce module de codeÂ :
void UpdateSFMLMessage(const std::string& message);

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


class ThreadClient : public Thread
{
public:
    ThreadClient() : Thread() {  };

    virtual bool Run() override {

        while (true) {}
        return true;
    }

    virtual bool Start() override
    {
        std::cout << "Starting..." << std::endl;
        m_threadHandle = CreateThread(nullptr, 0, Static_ThreadProc, (void*)this, 0, nullptr);
        return (m_threadHandle != nullptr);
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
        wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPCLIENT));
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_APPCLIENT);
        wcex.lpszClassName = szWindowClass;
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

        return RegisterClassExW(&wcex);
    }




    void InitInstance(HINSTANCE hInstance, int nCmdShow)
    {
        hInst = hInstance; // Stocke le handle d'instance dans la variable globale

        // Creation de la fenetre Windows
        hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    }


    virtual void OnThread() override
    {
     
        hInstance = GetModuleHandle(0);

        Message sendMSG;

        LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
        LoadStringW(hInstance, IDC_APPCLIENT, szWindowClass, MAX_LOADSTRING);
        MyRegisterClass(hInstance);

        InitInstance(hInstance, 0);

        // CrÃƒÂ©ation de la socket
        SOCKET hsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (hsocket == INVALID_SOCKET) {
            std::cout << ("socket failed\n");
            WSACleanup();
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
        }
        else {
            std::cout << "Connecte au serveur.\n";

            // Envoie d'un message pour indiquer que le client est prÃªt Ã  recevoir des messages
            const char* readyMessage = "Ready to receive messages";
            send(hsocket, readyMessage, strlen(readyMessage), 0);
        }

        if (WSAAsyncSelect(hsocket, hWnd, WM_READ, FD_READ | FD_CLOSE) == SOCKET_ERROR) {
            std::cout << ("WSAAsyncSelect failed for clientSocket\n");
            closesocket(hsocket);
            WSACleanup();
        }
        else {
            std::cout << "WSAAsyncSelect successful for clientSocket\n";
        }

        HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_APPCLIENT));

        

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
                    }
                    gameGrid.handleEvent(&sendMSG, event, hsocket);
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
    }

    MSG GetMsg()
    {
        return msg;
    }

private:
    MSG msg;
    HINSTANCE hInstance;
    HWND hWnd;
};








int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    InitializeCriticalSection(&critical);

    RedirectIOToConsole();

    int iResult;
    // Initialisation de winsock
    WSADATA wsaData;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << ("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    ThreadClient* threadClient = new ThreadClient();
    threadClient->Start();

    MSG message;
    while (GetMessage(&message, nullptr, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }


    // Nettoyage de Winsock
    WSACleanup();
    return (int)(*threadClient).GetMsg().wParam;
}


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
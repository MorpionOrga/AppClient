// header.hÂ : fichier Include pour les fichiers Include systÃ¨me standard,
// ou les fichiers Include spÃ©cifiques aux projets
//

#pragma once
#include <SFML/Graphics.hpp>
//Include que nous allons utiliser
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <io.h> //ajouter console
#include <fcntl.h> // "
#include <Windows.h>


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclure les en-tÃªtes Windows rarement utilisÃ©s


// Fichiers d'en-tÃªte C RunTime
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define PORT 14843
#define WM_RESPONCE (WM_USER + 1) 
#define WM_READ (WM_USER + 2) 


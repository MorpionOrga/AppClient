#pragma once
#include <Windows.h>
#include <iostream>

class Thread
{
public: 
    Thread() : m_threadHandle(nullptr) {};

	virtual ~Thread() {};
	
    virtual bool Run() = 0;
    virtual bool Start();
    virtual void Wait();
    HANDLE m_threadHandle;

    virtual void OnThread() = 0;

    static DWORD WINAPI Static_ThreadProc(LPVOID param);

private:
    LPDWORD threadID;
};


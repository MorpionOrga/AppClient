#include "Thread.h"
#include <Windows.h>

bool Thread::Start()
{
    std::cout << "Starting..." << std::endl;
    m_threadHandle = CreateThread(nullptr, 0, Static_ThreadProc, (void*)this, 0, nullptr);
	return (m_threadHandle != nullptr);
}

void Thread::Wait()
{
    if (m_threadHandle != nullptr) {
        WaitForSingleObject(m_threadHandle, INFINITE);
        CloseHandle(m_threadHandle);
        m_threadHandle = nullptr;
    }
    std::cout << "thread close " << std::endl;
}

DWORD WINAPI Thread::Static_ThreadProc(LPVOID param)
{
    Thread* thread = static_cast<Thread*>(param);
    thread->OnThread();

    if (thread != nullptr) {
        if (!thread->Run())
            std::cerr << "error run thread" << std::endl;
    }
    return 0;
}

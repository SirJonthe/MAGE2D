#include "Thread.h"

Thread::Thread( void ) : m_thread(NULL) {}

bool Thread::Run(ThreadFunction function, void *in_data)
{
	Kill();
	m_thread = SDL_CreateThread(function, in_data);
	return IsGood();
}

int Thread::WaitUntilDone( void )
{
	int ret_val = -1;
	if (IsGood()) {
		SDL_WaitThread(m_thread, &ret_val);
		m_thread = NULL;
	}
	return ret_val;
}

void Thread::Kill( void )
{
	if (IsGood()) {
		SDL_KillThread(m_thread);
		m_thread = NULL;
	}
}

Uint32 Thread::GetCurrentID( void )
{
	return SDL_GetThreadID(NULL);
}

Uint32 Thread::GetID( void ) const
{
	return SDL_GetThreadID(m_thread);
}

bool Thread::IsGood( void ) const
{
	return m_thread != NULL;
}

Mutex::Mutex( void ) : m_mutex(NULL), m_isLocked(false), m_lockID(0)
{
	m_mutex = SDL_CreateMutex();
}

Mutex::~Mutex( void )
{
	SDL_DestroyMutex(m_mutex);
}

bool Mutex::Lock( void )
{
	bool ret_val = SDL_LockMutex(m_mutex) == 0;
	if (ret_val) {
		m_isLocked = true;
		m_lockID = Thread::GetCurrentID();
	}
	return ret_val;
}

bool Mutex::TryLock( void )
{
	if (!IsLocked()) {
		return Lock();
	}
	return false;
}

bool Mutex::Unlock( void )
{
	if (IsLocked() && GetLockerID() != Thread::GetCurrentID()) {
		SDL_SetError("Unlocking thread is not same as locking thread.");
		return false;
	}
	bool ret_val = SDL_UnlockMutex(m_mutex) == 0;
	if (ret_val) {
		m_isLocked = false;
		m_lockID = 0;
	}
	return ret_val;
}

bool Mutex::IsLocked( void ) const
{
	return m_isLocked;
}

Uint32 Mutex::GetLockerID( void ) const
{
	return m_lockID;
}

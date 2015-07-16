#ifndef THREAD_H_INCLUDED__
#define THREAD_H_INCLUDED__

#include "Platform.h"

typedef int (*ThreadFunction)(void*);

class Thread
{
private:
	SDL_Thread *m_thread;

public:
				  Thread( void );
				 ~Thread( void );

	bool          Run(ThreadFunction function, void *in_data);
	int           WaitUntilDone( void );
	void          Kill( void );

	static Uint32 GetCurrentID( void );
	Uint32        GetID( void ) const;
	bool          IsGood( void ) const;
};

class Mutex
{
private:
	SDL_mutex *m_mutex;
	bool       m_isLocked;
	Uint32     m_lockID;

public:
		   Mutex( void );
		  ~Mutex( void );

	bool   Lock( void );
	bool   Unlock( void );
	bool   TryLock( void );

	bool   IsLocked( void ) const;
	Uint32 GetLockerID( void ) const;
};

#endif // THREAD_H_INCLUDED__

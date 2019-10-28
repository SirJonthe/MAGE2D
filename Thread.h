#ifndef THREAD_H_INCLUDED__
#define THREAD_H_INCLUDED__

#include "Platform.h"

// @data ThreadFunction
// @info Wrapper for a function with int as return type and taking a void pointer as parameter.
typedef int (*ThreadFunction)(void*);

// @data Thread
// @info Thin wrapper for SDL_Thread.
class Thread
{
private:
	SDL_Thread *m_thread;

public:
				  Thread( void );
				 ~Thread( void );

	// @algo Run
	// @info Calls a function without blocking execution of the calling thread.
	// @in
	//   function -> A function pointer.
	//   in_data -> Data the function pointer may use as input parameters.
	// @out The success state of creating a new thread (TRUE of success).
	bool          Run(ThreadFunction function, void *in_data);
	
	// @algo WaitUntilDone
	// @info Rest the calling thread until the function is done executing.
	// @out Returns FALSE if no thread was allocated.
	int           WaitUntilDone( void );
	
	// @algo Kill
	// @info Ends the execution of the thread.
	void          Kill( void );

	// @algo GetCurrentID
	// @out The next unique ID.
	static Uint32 GetCurrentID( void );
	
	// @algo GetID
	// @out The thread ID.
	Uint32        GetID( void ) const;
	
	// @algo IsGood
	// @out The error state (TRUE when everything is OK).
	bool          IsGood( void ) const;
};

// @data Mutex
// @info
class Mutex
{
private:
	SDL_mutex *m_mutex;
	bool       m_isLocked;
	Uint32     m_lockID;

public:
		   Mutex( void );
		  ~Mutex( void );

	// @algo Lock
	// @info Locks a resource from use by another thread.
	// @out The success state of the lock (TRUE of success).
	bool   Lock( void );
	
	// @algo Unlock
	// @info Unlocks the resource for use by other threads.
	// @out The success state of the unlock (TRUE on success).
	bool   Unlock( void );
	
	// @algo TryLock
	// @info Attempts a lock.
	// @out The success state of the lock attempt (TRUE on success).
	bool   TryLock( void );

	// @algo IsLocked
	// @out The lock state (TRUE on lock)
	bool   IsLocked( void ) const;
	
	// @algo GetLockerID
	// @out the lock ID.
	Uint32 GetLockerID( void ) const;
};

#endif // THREAD_H_INCLUDED__

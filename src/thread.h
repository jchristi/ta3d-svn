/*  TA3D, a remake of Total Annihilation
    Copyright (C) 2006  Roland BROCHARD

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA*/



/*
**  Guide to thread classes
**
**  To make new thread classes you need to
**  make a subclass of Thread and define the
**  proc method. In this method you need to
**  dereference the params struct as whatever
**  type you are using (like net_thread_params)
**
*/

class BaseThread{

	virtual void proc(void* param) = 0;
	protected:
		int dead;

	public:
		BaseThread(){dead=1;}
		virtual ~BaseThread(){dead=1;}
		virtual int Spawn(void* param)=0;
		virtual void Join()=0;

};



#ifdef TA3D_PLATFORM_WINDOWS

#include <windows.h>
class Thread : public BaseThread{

	DWORD threadid;
	HANDLE thread;
	struct thread_params{
		void* more;
		Thread* thisthread;
	};
	struct thread_params secondary;
	
	virtual void proc(void* param);
	static DWORD WINAPI run(LPVOID param){
		((struct thread_params*)param)->thisthread->proc(((struct thread_params*)param)->more);
		return 0;
	}

	public:
		virtual int Spawn(void* param){
			dead = 0;
			secondary.thisthread = this;
			secondary.more = param;
			thread = CreateThread(NULL,0,run,&secondary,0,&threadid);
		}
		virtual void Join(){
			dead = 1;
			WaitForSingleObject(thread,2000);
		}
};

class Mutex{
	int locked;
	CRITICAL_SECTION mutex;

	public:
		Mutex() {locked=0; InitializeCriticalSection(&mutex);}
		~Mutex() {DeleteCriticalSection(&mutex);}
		void Lock() {locked=1; EnterCriticalSection(&mutex);}
		void Unlock() {locked=0; LeaveCriticalSection(&mutex);}
		int isLocked() {return locked;}
};
	
#endif /*WIN32*/




#ifdef TA3D_PLATFORM_LINUX
#include <pthread.h>

class Thread : public BaseThread{

	pthread_t thread;
	struct thread_params{
		void* more;
		Thread* thisthread;
	};
	struct thread_params secondary;

	virtual void proc(void* param)=0;
	static void* run(void* param){
		((struct thread_params*)param)->thisthread->proc(((struct thread_params*)param)->more);
		return NULL;
	}


	public:
		virtual int Spawn(void* param){
			dead = 0;
			
			secondary.thisthread = this;
			secondary.more = param;

			pthread_create(&thread,NULL,run,&secondary);
		
			return 0;
		}
		virtual void Join(){
			if(dead){
				printf("tried to join a dead thread\n");
				return;
			}
			dead = 1;
			pthread_join(thread,NULL);
		}
};

class Mutex{
	int locked;
	pthread_mutex_t mutex;

	public:
		Mutex() {locked=0; pthread_mutex_init(&mutex,NULL);}
		~Mutex() {pthread_mutex_destroy(&mutex);}
		void Lock() {locked=1; pthread_mutex_lock(&mutex);}
		void Unlock() {locked=0; pthread_mutex_unlock(&mutex);}
		int isLocked() {return locked;}
};
#endif /*LINUX*/

#include "Thread.hpp" 

    Thread::Thread(uint thread_id, PCQueue<Job>& pcq_tasks)
        : thread_id_field(thread_id), pcq_tasks(pcq_tasks){}

	Thread::~Thread() {} // Does nothing 

	/** Returns true if the thread was successfully started, false if there was an error starting the thread */
	bool Thread::start() {
		if(pthread_create(&m_thread, NULL, entry_func, (void*)(this)) != 0 )
			return false;
		return true;
	}

	/** Will not return until the internal thread has exited. */
	void Thread::join() {
		pthread_join(m_thread, NULL);
	}

	/** Returns the thread_id **/
	uint Thread::thread_id() {return thread_id_field;}

#include "Semaphore.hpp"

// Constructs a new semaphore with a counter of 0
	Semaphore::Semaphore() {
		counter = 0;
		pthread_mutex_init(&this->counter_lock, NULL);	//TO DO
		pthread_mutex_init(&this->lock, NULL);	//TO DO
		pthread_cond_init(&this->is_available, NULL);
	} 

// Constructs a new semaphore with a counter of val
	Semaphore::Semaphore(unsigned val) {
		counter = val;
		pthread_mutex_init(&this->counter_lock, NULL);	//TO DO
		pthread_mutex_init(&this->lock, NULL);	//TO DO
		pthread_cond_init(&this->is_available, NULL);
	}

	Semaphore::~Semaphore() {
		pthread_cond_destroy(&is_available);
		pthread_mutex_destroy(&counter_lock);
		pthread_mutex_destroy(&lock);
	}

// Block untill counter >0, and mark - One thread has entered the critical section.
	void Semaphore::down() {
		bool to_run = false;
		while(true) {
			pthread_mutex_lock(&counter_lock);
			if(counter > 0) {
				counter--;
				to_run = true;
			}
			pthread_mutex_unlock(&counter_lock);
			if(to_run)
				return;

			pthread_mutex_lock(&lock);
			pthread_cond_wait(&is_available, &lock);
			pthread_mutex_unlock(&lock);
		}
	} 

	// Mark: 1 Thread has left the critical section
	void Semaphore::up() {
		pthread_mutex_lock(&counter_lock);
		counter++;
		pthread_cond_broadcast(&is_available);
		pthread_mutex_unlock(&counter_lock);
	}



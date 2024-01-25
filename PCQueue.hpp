#ifndef _QUEUEL_H
#define _QUEUEL_H
#include "Headers.hpp"
#include "Semaphore.hpp"
#include "WritersReadersLock.hpp"
// Single Producer - Multiple Consumer queue
template <typename T>class PCQueue
{

public:
	PCQueue() : elements(), wr_lock() {
		wr_lock.readers_writers_init();
	}

	~PCQueue() {
		wr_lock.readers_writers_destroy();
	}

	// Blocks while queue is empty. When queue holds items, allows for a single
	// thread to enter and remove an item from the front of the queue and return it. 
	// Assumes multiple consumers.
	T pop() {
		wr_lock.readers_lock();
		T item = elements.front();
		elements.pop_front();
		wr_lock.readers_unlock();
		return item;
	}

	// Allows for producer to enter with *minimal delay* and push items to back of the queue.
	// Hint for *minimal delay* - Allow the consumers to delay the producer as little as possible.  
	// Assumes single producer 
	void push(const T& item) {
		wr_lock.writers_lock();
		elements.push_back(item);
		wr_lock.writers_unlock();
	}


private:
	// Add your class memebers here

	std::deque<T> elements;
	WritersReadersLock wr_lock;

};
// Recommendation: Use the implementation of the std::queue for this exercise
#endif
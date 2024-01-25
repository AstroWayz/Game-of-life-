#ifndef __WRITERSREADERSLOCK_H
#define __WRITERSREADERSLOCK_H
#include "Headers.hpp"

class WritersReadersLock {

    private:
        int readers_inside, writers_inside, writers_waiting;
        pthread_cond_t read_allowed;
        pthread_cond_t write_allowed;
        pthread_mutex_t global_lock;
        int counter;

    public:
        void readers_writers_init();

        void readers_writers_destroy();

        void readers_lock();

        void readers_unlock();

        void writers_lock();

        void writers_unlock();
};
#endif
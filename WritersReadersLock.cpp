#include "WritersReadersLock.hpp"
        void WritersReadersLock::readers_writers_init(){
            readers_inside = 0;
            writers_inside = 0; 
            writers_waiting = 0;
            counter = 0;

            pthread_mutex_init(&global_lock,NULL);
            pthread_cond_init(&read_allowed,NULL);
            pthread_cond_init(&write_allowed,NULL);
        }

        void WritersReadersLock::readers_writers_destroy(){
            pthread_cond_destroy(&read_allowed);
            pthread_cond_destroy(&write_allowed);
            pthread_mutex_destroy(&global_lock);
        }

        void WritersReadersLock::readers_lock(){
            pthread_mutex_lock(&global_lock);
            while(counter == 0 || writers_inside + readers_inside > 0 || writers_waiting > 0){
                pthread_cond_wait(&read_allowed,&global_lock);
            }
            readers_inside++;
            counter--;
            pthread_mutex_unlock(&global_lock);
        }

        void WritersReadersLock::readers_unlock(){
            pthread_mutex_lock(&global_lock);
            readers_inside--;
            if(readers_inside == 0){
                pthread_cond_signal(&write_allowed);
                pthread_cond_signal(&read_allowed);
            }
            pthread_mutex_unlock(&global_lock);
        }

        void WritersReadersLock::writers_lock(){
            pthread_mutex_lock(&global_lock);
            writers_waiting++;
            while(readers_inside + writers_inside > 0){
                pthread_cond_wait(&write_allowed, &global_lock);
            }
            writers_waiting--;
            writers_inside++;
            pthread_mutex_unlock(&global_lock);
        }

        void WritersReadersLock::writers_unlock(){
            pthread_mutex_lock(&global_lock);
            writers_inside--;
            counter++;
            if(writers_inside == 0){
                pthread_cond_signal(&write_allowed);
                pthread_cond_signal(&read_allowed);
            }
            pthread_mutex_unlock(&global_lock);
        }

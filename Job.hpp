#ifndef __JOB_H
#define __JOB_H


#include "Headers.hpp"

//Send To Ron
class Job{
    public:
        vector<float>* m_tile_hist;
	    int start_row;
        int finish_row;
	    int* threads_running;
	    pthread_mutex_t* threads_running_m;
		pthread_cond_t* threads_running_c;
    int_mat* current_field;
    int_mat* next_field;
    int phase;

		Job(vector<float>* m_tile_hist, int start_row, int finish_row, int* threads_running, pthread_mutex_t* threads_running_m,
            int_mat* current_field, int_mat* next_field, pthread_cond_t* threads_running_c , int phase);
		~Job();
};

#endif
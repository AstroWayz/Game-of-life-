

//Send to Ron
#include "Job.hpp"

Job::Job(vector<float>* m_tile_hist, int start_row, int finish_row, int* threads_running, pthread_mutex_t* threads_running_m,
         int_mat* current_field, int_mat* next_field, pthread_cond_t* threads_running_c , int phase)
	: m_tile_hist(m_tile_hist),
	start_row(start_row),
	finish_row(finish_row),
	threads_running(threads_running),
	threads_running_m(threads_running_m),
	threads_running_c(threads_running_c),
	current_field(current_field),
	next_field(next_field) , phase(phase){}

Job::~Job() {
	m_tile_hist = NULL;
	threads_running = NULL;
	threads_running_m = NULL;
	threads_running_c = NULL;
	current_field = NULL;
	next_field = NULL;
}
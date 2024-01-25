
#include "ThreadWorker.hpp"


    ThreadWorker::ThreadWorker(uint thread_id, PCQueue<Job>& pcq_tasks) 
        : Thread(thread_id, pcq_tasks) {}

    ThreadWorker::~ThreadWorker(){}

    void ThreadWorker::thread_workload() {
      while(true){
        Job job = pcq_tasks.pop();

        if(job.start_row == -1){
            return;
        }
        auto job_start = std::chrono::system_clock::now();
       if(job.phase == 1){
           update_next_field(job);

       }else{

           update_next_field2(job);

       }

        auto job_end = std::chrono::system_clock::now();
        //update tile hist:
        //update total unfinished jobs:
        pthread_mutex_lock(job.threads_running_m);
         (job.m_tile_hist)->push_back((float)std::chrono::duration_cast<std::chrono::microseconds>(job_end - job_start).count());
          (*(job.threads_running))--;
        pthread_cond_signal(job.threads_running_c);
        pthread_mutex_unlock(job.threads_running_m);
       }

    }

    int ThreadWorker::calcAlive ( int_mat* matrix, int row, int col,int rows, int cols) {
        int sum = 0;
        for (int i = row - 1 ; i <= row + 1; i++ ) {
            for ( int j = col - 1; j <= col + 1; j++ ){
                if( i >= 0 && i < rows && j >= 0 && j < cols ){
                    if((*matrix)[i][j]!=0){
                        sum += 1;
                    }
                }

            }
        }
        if((*matrix)[row][col]!=0)
        sum -=1;
        return sum;    
    }
int ThreadWorker::calcDominant ( int_mat* matrix, int row, int col,int rows, int cols) { // calculates the new value of the current position ;
    double sum = 0;
    double counter = 0;
    if (col > 0 &&  col < cols - 1 && row > 0 && row < rows - 1) {
        for (int i = row - 1; i <= row + 1; i++) {
            for (int j = col - 1; j <= col + 1; j++) {
                if ((*matrix)[i][j] != 0) {
                    sum += (*matrix)[i][j];
                    counter++;
                }
            }
        }

        return (int)std::round(sum/counter);

    }else {

        for (int i = row - 1; i <= row + 1; i++) {
            for (int j = col - 1; j <= col + 1; j++) {
                if (i >= 0 && i < rows && j >= 0 && j < cols) {
                    if ((*matrix)[i][j] != 0) {
                        sum += (*matrix)[i][j];
                        counter++;
                    }
                }
            }
        }

        return (int)std::round(sum/counter);
    }
}
int ThreadWorker::checkDominant( int_mat* matrix, int row, int col,int rows, int cols , Job job){
    int array[8];
    for( int m = 0 ; m < 8 ; m++){
        array[m]=0;
    }

    if (col > 0 &&  col < cols - 1 && row > 0 && row < rows - 1) {
        for (int i = row - 1 ; i <= row + 1; i++ ) {
            for ( int j = col - 1; j <= col + 1; j++ ){
                if((*matrix)[i][j]!=0){
                    array[(*matrix)[i][j]]++;
                }

            }
        }
        int dominant = 0 , curr = 0;
       for(int i = 1 ; i < 8 ; i ++){
           if(array[i]*i > curr){
               dominant = i;
               curr = array[i]*i;
           }else if (array[i]*i == curr){

               if(dominant > i){
                   dominant = i;
               }
           }
       }
        return dominant;
    }else{

        for (int i = row - 1 ; i <= row + 1; i++ ) {
            for ( int j = col - 1; j <= col + 1; j++ ){
                if( i >= 0 && i < rows && j >= 0 && j < cols ){
                    if((*matrix)[i][j]!=0){
                        array[(*matrix)[i][j]]++;
                    }
                }

            }
        }
        int dominant = 0 , curr = 0;
        for(int i = 1 ; i < 8 ; i ++){
            if(array[i]*i > curr){
                dominant = i;
                curr = array[i]*i;
            }else if (array[i]*i == curr){
                if(dominant > i){
                    dominant = i;
                }
            }
        }
        return dominant;
    }

}

void ThreadWorker::update_next_field(Job job){
        int start = job.start_row;
        int finish = job.finish_row;
        int_mat* current = job.current_field;
        int_mat* next = job.next_field;

        int rows = (*current).size();
        int cols = (*current)[0].size();

        for(int row = start; row < finish; row++){
            for(int col = 0; col < cols; col++){
                int alive_cells = 0;

                //calculating alive cells:
                if (col > 0 &&  col < cols - 1 && row > 0 && row < rows - 1) {
                    for (int i = row - 1 ; i <= row + 1; i++ ) {
                        for ( int j = col - 1; j <= col + 1; j++ ){
                            if((*current)[i][j]!=0){
                                alive_cells += 1;
                            }

                        }
                    }
                    if((*current)[row][col]!=0)
                    alive_cells -= 1;
                }
                else {
                    alive_cells += calcAlive(current, row, col, rows, cols);
                }

                //determine next generation for current cell:
                if(((*current)[row][col] != 0 && (alive_cells == 3 || alive_cells == 2 ))) {
                    (*next)[row][col] =(*current)[row][col];
                }else if((*current)[row][col] == 0 && alive_cells == 3) {
                    (*next)[row][col] = checkDominant(current,row,col,rows,cols,job);
                }
                else
                    (*next)[row][col] = 0;
            }
        }
    }
void  ThreadWorker::update_next_field2(Job job) {

    int start = job.start_row;
    int finish = job.finish_row;
    int_mat *current = job.current_field;
    int_mat *next = job.next_field;
    int rows = (*current).size();
    int cols = (*current)[0].size();

    for (int row = start; row < finish; row++) {
        for (int col = 0; col < cols; col++) {

            if ((*current)[row][col] != 0) {
                (*next)[row][col] = calcDominant(current, row, col, rows, cols);
            }else{
                (*next)[row][col] = 0;
            }


        }
    }
}
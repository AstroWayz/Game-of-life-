
#ifndef __THREADWORKER_H
#define __THREADWORKER_H


#include "Thread.hpp"

class ThreadWorker : public Thread {
public:
    ThreadWorker(uint thread_id, PCQueue<Job>& pcq_tasks);
    ~ThreadWorker();
    void thread_workload();

private:
    int calcAlive ( int_mat* matrix, int row, int col,int rows, int cols);
    int calcDominant ( int_mat* matrix, int row, int col,int rows, int cols);
    void update_next_field(Job job);
    int checkDominant( int_mat* matrix, int row, int col,int rows, int cols , Job job);
    void update_next_field2(Job job);

};
#endif
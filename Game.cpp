#include "Game.hpp"
using namespace utils;
/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
static int_mat* create_field_from_file(string);
static void printStandart(const int_mat& field, uint field_width, uint field_height);

static const char *colors[7] = {BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN};

Game::Game(game_params gp) 
	:	file_name(gp.filename),
		m_gen_num(gp.n_gen),
		m_thread_num(gp.n_thread),
		m_tile_hist(),
		m_gen_hist(),
		m_threadpool(),
		interactive_on(gp.interactive_on),
		print_on(gp.print_on),
		pcq_tasks(),
		threads_running(0),
		current_field(NULL),
		next_field(NULL)		
{
	pthread_mutex_init(&threads_running_m, NULL);
	pthread_cond_init(&threads_running_c, NULL);
}//Constructor

Game::~Game() {
	pthread_mutex_destroy(&threads_running_m);
	pthread_cond_destroy(&threads_running_c);
}//Destructor

const vector<float> Game::gen_hist() const {
	return this->m_gen_hist;
}// Returns the generation timing histogram  

const vector<float> Game::tile_hist() const {
	return this->m_tile_hist;
}// Returns the tile timing histogram

uint Game::thread_num() const {
	return this->m_thread_num;
}//Returns the effective number of running threads = min(thread_num, field_height)

void Game::run() {

	_init_game(); // Starts the threads and all other variables you need
	print_board("Initial Board");
	for (uint i = 0; i < m_gen_num; ++i) {
		auto gen_start = std::chrono::system_clock::now();
       
		_step(i); // Iterates a single generation

		auto gen_end = std::chrono::system_clock::now();
		m_gen_hist.push_back((float)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
		print_board(NULL);
	} // generation loop
	print_board("Final Board");
	_destroy_game();
}

void Game::_init_game() {
	// Create game fields
	current_field = create_field_from_file(file_name); 
	next_field = new int_mat(*current_field);
	
	//deside on the effective number of threads:
	m_thread_num = m_thread_num > current_field->size() ? current_field->size() : m_thread_num;  
	//restarting size of histogram forthreads times:
	m_tile_hist = vector<float>(m_thread_num, 0);
	// Create and Start all the threads:
	for( uint i = 0 ; i < m_thread_num; i++) {
		Thread* created_thread = new ThreadWorker(i, pcq_tasks);
		m_threadpool.push_back(created_thread); 
		while( !(created_thread->start()) );
	}
	// Testing of your implementation will presume all threads are started here
}

void Game::_step(uint curr_gen) {
	//indication for unfinished tasks (=total running working threads) initialized in this generation.
	//TODO ; in each step we have 2 phases ;;;;;;;
	pthread_mutex_lock(&threads_running_m);
	threads_running = m_thread_num;
	pthread_mutex_unlock(&threads_running_m);

	//pushing N jobs to phase 1:
	int number_rows = current_field -> size();
	int offset = (number_rows) / m_thread_num;
	int start = 0;
	int finish = start + offset;
	
	for(uint i = 0; i< m_thread_num ; i++) {
		pcq_tasks.push(Job(&m_tile_hist, start, finish, &threads_running, &threads_running_m, current_field, next_field, &threads_running_c ,1));
		start = finish;
		finish = (i == m_thread_num - 2) ? number_rows : (start + offset);
	}
	// finish pushing ;)
	
	// Wait for the workers to finish calculating 
	pthread_mutex_lock(&threads_running_m);
	while(threads_running != 0) { 
		pthread_cond_wait(&threads_running_c, &threads_running_m);
	}

	pthread_mutex_unlock(&threads_running_m);

	// Swap pointers between current and next field
    int_mat* temp = current_field;
	current_field = next_field;
	next_field = temp;


    pthread_mutex_lock(&threads_running_m);
    threads_running = m_thread_num;
    pthread_mutex_unlock(&threads_running_m);



    //pushing N jobs to phase 2:
    pthread_mutex_lock(&threads_running_m);
    threads_running = m_thread_num;
    pthread_mutex_unlock(&threads_running_m);
     number_rows = current_field -> size();
     offset = (number_rows) / m_thread_num;
     start = 0;
     finish = start + offset;

    for(uint i = 0; i< m_thread_num ; i++) {
        pcq_tasks.push(Job(&m_tile_hist, start, finish, &threads_running, &threads_running_m, current_field, next_field, &threads_running_c ,2));
        start = finish;
        finish = (i == m_thread_num - 2) ? number_rows : (start + offset);
    }

    // finish pushing ;)

    // Wait for the workers to finish calculating
    pthread_mutex_lock(&threads_running_m);
    while(threads_running != 0) {
        pthread_cond_wait(&threads_running_c, &threads_running_m);
    }

    pthread_mutex_unlock(&threads_running_m);

    // Swap pointers between current and next field
    int_mat* temp2 = current_field;
    current_field = next_field;
    next_field = temp2;

}

void Game::_destroy_game(){
	for(uint i = 0 ; i < m_thread_num; i++) {
		pcq_tasks.push(Job(NULL, -1, -1, NULL, NULL, NULL, NULL,NULL,1));
	}
	for(uint i = 0 ; i < m_thread_num; i++) {
		m_threadpool[i]->join();
		delete m_threadpool[i];
	}
	delete current_field;
	delete next_field;
}

/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
void Game::print_board(const char* header) {

	if(print_on){ 

		// Clear the screen, to create a running animation 
		if(interactive_on)
			system("clear");

		// Print small header if needed
		if (header != NULL)
			cout << "<------------" << header << "------------>" << endl;
		
		printStandart(*current_field, (*current_field)[0].size(), current_field->size());
		// Display for GEN_SLEEP_USEC micro-seconds on screen 
		if(interactive_on)
			usleep(GEN_SLEEP_USEC);
	}

}

// TODO 
static int_mat* create_field_from_file(string file_name) {
	vector<string> rows = read_lines(file_name);
	int num_rows = rows.size();
	int num_cols = split(rows[0], ' ').size();
    int_mat* finished_matrix = new int_mat(num_rows, vector<uint>(num_cols, false));
	for(int i = 0; i < num_rows; i++) {
		vector<string> words_in_row = split(rows[i], ' ');
		for(int j = 0; j < num_cols; j++) {
		    if((words_in_row[j] == "1")){
                (*finished_matrix)[i][j] = 1;

		    }
            if((words_in_row[j] == "2")){
                (*finished_matrix)[i][j] = 2;

            }
            if((words_in_row[j] == "3")){
                (*finished_matrix)[i][j] = 3;

            }
            if((words_in_row[j] == "4")){
                (*finished_matrix)[i][j] = 4;

            }
            if((words_in_row[j] == "5")){
                (*finished_matrix)[i][j] = 5;

            }
            if((words_in_row[j] == "6")){
                (*finished_matrix)[i][j] = 6;

            }
            if((words_in_row[j] == "7")){
                (*finished_matrix)[i][j] = 7;

            }
            if((words_in_row[j] == "0")){
                (*finished_matrix)[i][j] = 0;
            }

		}
	}
	return finished_matrix;
}




static void printStandart(const int_mat& field, uint field_width, uint field_height){
	cout << u8"╔" << string(u8"═") * field_width << u8"╗" << endl;
		for (uint i = 0; i < field_height; ++i) {
			cout << u8"║";
			for (uint j = 0; j < field_width; ++j) {
                if (field[i][j] > 0)
                    cout << colors[field[i][j] % 7] << u8"█" << RESET;
                else
                    cout << u8"░";;
			}
			cout << u8"║" << endl;
		}
		cout << u8"╚" << string(u8"═") * field_width << u8"╝" << endl;
}


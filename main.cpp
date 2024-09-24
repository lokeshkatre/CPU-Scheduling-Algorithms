#include<iostream>
#include "Process.h"
#include "Scheduling_algorithm.h"
using namespace std;


int main(){
    //Sets number of processes
	int num_processes = 20;
    Algorithm algo;

	srand((unsigned)time(0));
	int random_burst;
	int importance;

	//vector will serve as the ready queue
	vector <process> processes;

	//80% of processes have a semi random arrival time determined by exponential distribution
	//20% of processes have 0 arrival time
	int num_rand = num_processes * .8;
	int num_zero = num_processes - num_rand;

	//Creates processes with zero arrival time
	for (int i = 0; i < num_zero; i++)
	{
		random_burst = (rand()%3500)+500;
		importance = (rand()%5);
		process temp(random_burst, 0, i+1, importance);
		processes.push_back(temp);
	}

	//Creates processes with non zero arrival time
	for(int i = 0; i < num_rand; i++)
	{
		random_burst = (rand()%3500)+500;
		importance = (rand()%5);
		double lambda = 0.001;
		double r = ((double) rand()/(RAND_MAX));
		double x = -(log(r)/lambda);
		if ( x > 8000 ) { i--; continue; }
		process temp(random_burst, (int)x, (num_zero + i+1), importance);
		processes.push_back(temp);
      	}

	//Sorts processes by arrival time
	sort(processes.begin(), processes.end(), sorter3);

	//Assigns pids 1 - n based on arrival time. Makes the output easier to follow
	for (unsigned int i = 0; i < processes.size(); i++)
	{
		processes[i].set_pid(i+1);
	}

	
	//Sets variables for statistical information
	max_wait = 0.0;
        min_wait = num_processes * 5000.0;
        max_initial = 0.0;
	min_initial = num_processes * 5000.0;

	cin.get();

	cout << "\033[4mFIRST COME FIRST SERVE :\033[0m" << endl << endl;

	cin.get();
	//FCFS
     	algo.first_come_first_serve(processes, num_processes);
	cout << endl;
	
	//Statistics
	statistics(num_processes);
	cin.get();

	cout << "\033[4mSHORTEST JOB FIRST:\033[0m" << endl << endl;
	cin.get();

	//Non-Preemptive SJF
	algo.shortest_job_first(processes, num_processes);
	cout << endl;

	statistics(num_processes);

	cin.get();

	cout << "\033[4mSHORTEST JOB REMAINING OR PREEMPTIVE SHORTEST JOB FIRST:\033[0m" << endl << endl;
	cin.get();
	//Shortest_job_remaining or preemptive sjf
	algo.shortest_job_remaining(processes, num_processes);
	cout << endl;

	statistics(num_processes);
	cin.get();

	cout << "\033[4mROUND ROBIN:\033[0m" << endl << endl;
	cin.get();

	//Round Robin
	algo.round_robin(processes, num_processes);
	cout << endl;

	statistics(num_processes);
	cin.get();

	cout << "\033[4mPRIORITY:\033[0m" << endl << endl;
	cin.get();
	
	//Priority
	algo.priority(processes, num_processes);
	cout << endl;

	statistics(num_processes);
	cin.get();
	
	return 0;
}

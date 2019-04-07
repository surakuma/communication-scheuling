#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <queue>
#include <utility>
#include <cassert>

#include <limits>
#include <iomanip>
#include <fstream>
#include <sstream>

#include <numeric>
#include <random>

#define MAXNUMBEROFTASKS 10000


#ifndef CAPACITY
#define CAPACITY 0
#endif

#ifndef PRINTCOMPLETESCHEDULE
#define PRINTCOMPLETESCHEDULE 0
#endif
using namespace std;

class Task
{
    public:
        unsigned int task_id;

        double input_comm_time, comp_time, output_comm_time;
        long unsigned int input_volume, output_volume;

        double start_comm_time, end_comm_time;
        double start_comp_time, end_comp_time;

        long unsigned int internal_memory_requirement;
        long unsigned int memory_requirement;

        long unsigned int available_memory_after_comm_scheduling = CAPACITY;
        long unsigned int available_memory_after_comp_scheduling = CAPACITY;
        std::string task_name;
        //private:
        long unsigned int getMemoryRequirement() { return input_volume + internal_memory_requirement; }

        unsigned int sequence_id;
};

void obtainSequence(Task* tasks, int ntasks, unsigned int capacity)
{
    std::vector<int> initial_sequence(ntasks);
    std::iota(initial_sequence.begin(), initial_sequence.end(), 0);
    std::vector<std::vector<int>>bins;
    std::vector<int> bin_size;

    for(auto i=ntasks; i>0; i--)
    {
        unsigned int id = rand()%i;
        unsigned int task_id = initial_sequence[id];
        assert(task_id == tasks[task_id].task_id);
        initial_sequence.erase(initial_sequence.begin() +id);
        bool found =false;
        for(auto j=0; j<bin_size.size(); j++)
        {
            if(bin_size[j] <= tasks[task_id].memory_requirement);
            {
                bin_size[j]-=tasks[task_id].memory_requirement;
                bins[j].push_back(task_id);
                found = true;
            }
        }

        if(!found)
        {
            std::vector<int> new_bin;
            new_bin.push_back(task_id);
            bin_size.push_back(capacity - tasks[task_id].memory_requirement);
            bins.push_back(new_bin);
        }
    }
    assert(initial_sequence.empty());
    unsigned int seq =0;
    for(auto i=0; i<bins.size(); i++)
        for(auto j=0; j<bins[i].size(); j++)
            tasks[bins[i][j]].sequence_id = seq++;
    assert(seq == ntasks);

}


void compute_schedule_for_specified_order(Task tasks[], int ntasks, long unsigned int capacity)
{
    long unsigned int available_memory_for_present_task = capacity;
    double early_available_comm_time = 0;
    double early_available_comp_time = 0;
    std::queue<Task> computation_queue;

    long unsigned int i=0;
    //k denotes the index of next task in computation channel 
    long unsigned int k=0;
    //While all tasks complete on computation channel
    while(k<ntasks)
    {
        if(!computation_queue.empty())
        {
            if(computation_queue.front().end_comp_time <= early_available_comm_time)
            {
                available_memory_for_present_task += computation_queue.front().memory_requirement;
                computation_queue.pop();
            }
        }
        if(k<i && early_available_comp_time <= early_available_comm_time)
        {
            tasks[k].start_comp_time = max(early_available_comp_time, tasks[k].end_comm_time);
            early_available_comp_time = tasks[k].start_comp_time + tasks[k].comp_time;
            tasks[k].end_comp_time = early_available_comp_time;
            computation_queue.push(tasks[k]);
            tasks[k].available_memory_after_comp_scheduling = available_memory_for_present_task;
            k++;

        }
        else if(tasks[i].memory_requirement <= available_memory_for_present_task )
        {
            tasks[i].start_comm_time = early_available_comm_time;
            early_available_comm_time += tasks[i].input_comm_time;
            tasks[i].end_comm_time = early_available_comm_time;
            available_memory_for_present_task -= tasks[i].memory_requirement;
            tasks[i].available_memory_after_comm_scheduling = available_memory_for_present_task;
            i++;
        }
        else
        {
            early_available_comm_time = early_available_comp_time;
        }

    }

}



//Print scheduling trace
//void print_schedule(string alg_name, Task tasks[], int ntasks, ofstream outputfile)
void print_schedule(string alg_name, Task tasks[], int ntasks, long unsigned int capacity)
{
    double makespan = 0;
    if(PRINTCOMPLETESCHEDULE)
        cout<<"#TaskId start_comm end_comm avail_mem_after_comm_sched start_comp end_comp avail_mem_after_comp_sched\n";
    for(auto i=0; i<ntasks; i++)
    {
        if(tasks[i].end_comp_time > makespan)
            makespan = tasks[i].end_comp_time;

        if(PRINTCOMPLETESCHEDULE)
            cout<<tasks[i].task_id<<" "<<tasks[i].start_comm_time<<" "<<tasks[i].end_comm_time<<" "<<tasks[i].available_memory_after_comm_scheduling<< " "<<tasks[i].start_comp_time<<" "<<tasks[i].end_comp_time<<" "<<tasks[i].available_memory_after_comp_scheduling<<endl;
    }
    cout<<std::fixed << std::setprecision(9);
    cout<< capacity << " " << makespan << endl;
//    outputfile<<alg_name <<" "<< makespan <<"\n";
}
int main(int argc, char* argv[])
{
    int ntasks = 0;
    //cin>>ntasks;

    Task* tasks = new Task[MAXNUMBEROFTASKS];
    ifstream file("input.txt", ios::in);

    if(file.good())
    {
        string str;
        int i = 0;
        unsigned int max_capacity_requirement=0;
        while(getline(file, str))
        {
            if(str[0] == '#') continue;

            istringstream iss(str);
            tasks[i].task_id = i;
            iss>> tasks[i].input_volume >>tasks[i].input_comm_time >> tasks[i].comp_time ;

            tasks[i].internal_memory_requirement  = 0;
            tasks[i].memory_requirement = tasks[i].getMemoryRequirement();
            if(tasks[i].memory_requirement > max_capacity_requirement)
                max_capacity_requirement = tasks[i].memory_requirement;
            i++;
        }
        file.close();
        ntasks = i;
        if(ntasks < 1)
        {
            std::cout<<"Number of tasks is 0 .. Exiting ...\n";
            exit(0);
        }
        if(max_capacity_requirement > CAPACITY)
        {
            std::cout<<"Some tasks need more than available memory capacity ... Exiting...\n";
            exit(0);
        }
    }
    else
    {
        std::cout <<"Input file is bad. Exiting ..\n";
        exit(0);
    }


    string alg_name="order_of_Gilmore_Gomory";

    long unsigned int capacity = CAPACITY;
    long unsigned int inc = CAPACITY/8;
    for(auto i=0; i<9; i++)
    {

        obtainSequence(tasks, ntasks, capacity);
        sort(tasks, tasks + ntasks, [](Task x, Task y) { return x.sequence_id < y.sequence_id;});
        compute_schedule_for_specified_order(tasks, ntasks, capacity);
        print_schedule(alg_name, tasks, ntasks, capacity);
        sort(tasks, tasks + ntasks, [](Task x, Task y) { return x.task_id < y.task_id;});
        capacity += inc;

    }

    delete [] tasks;
}

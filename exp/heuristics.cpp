#include <iostream>
#include <string>
#include <algorithm>
#include <queue>
#include <limits>
#include <iomanip>
#include <fstream>
#include <sstream>
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
        unsigned int input_volume, output_volume;

        double start_comm_time, end_comm_time;
        double start_comp_time, end_comp_time;

        unsigned int internal_memory_requirement;
        unsigned int memory_requirement;

        unsigned int available_memory_after_comm_scheduling = CAPACITY;
        unsigned int available_memory_after_comp_scheduling = CAPACITY;
        std::string task_name;
        //private:
        unsigned int getMemoryRequirement() { return input_volume + internal_memory_requirement; }
};

//TODO: don't think how we can use it as the first step of quick sort
bool computeIntensiveFunction(Task x, Task y) { return x.comp_time >= y.comp_time;}

bool commIncrease(Task x, Task y) { return x.input_comm_time < y.input_comm_time;}
bool compDecrease(Task x, Task y) { return x.comp_time > y.comp_time;}
bool commPlusCompIncrease(Task x, Task y) { return x.input_comm_time + x.comp_time < y.input_comm_time + y.comp_time;}
bool commPlusCompDecrease(Task x, Task y) { return x.input_comm_time + x.comp_time > y.input_comm_time + y.comp_time;}


void optimal_order_infinite_memory(Task tasks[], int ntasks)
{
    Task tmp = tasks[ntasks-1];
    unsigned int i = ntasks-1;

    for(int j=ntasks-2; j>=0; j--)
    {
        if(tasks[j].comp_time < tasks[j].input_comm_time)
        {
            tasks[i] = tasks[j];
            tasks[j] = tasks[i-1];
            i--;
        }
    }
    tasks[i] = tmp;

    if(tasks[i].comp_time >= tasks[i].input_comm_time)
        i++;

    if(i>0)
        sort(tasks, tasks+i, commIncrease);
    if(i<ntasks)
        sort(tasks+i, tasks+ntasks, compDecrease);

}

void compute_schedule_for_specified_order(Task tasks[], int ntasks)
{
    unsigned int available_memory_for_present_task = CAPACITY;
    double early_available_comm_time = 0;
    double early_available_comp_time = 0;
    std::queue<Task> computation_queue;

    unsigned int i=0;
    //k denotes the index of next task in computation channel 
    unsigned int k=0;
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
void print_schedule(string alg_name, Task tasks[], int ntasks)
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
    //cout<<"Makespan= "<<makespan<<endl;
    cout<<std::fixed << std::setprecision(9);
    cout<< alg_name << " " << makespan << endl;
//    outputfile<<alg_name <<" "<< makespan <<"\n";
}
int find_next_maximum_accelerated_task_with_memory_restriction(Task tasks[], unsigned int i, unsigned int ntasks, double early_available_comm_time,
        double early_available_comp_time, unsigned int available_memory_for_present_task)
{
    int index = -1;
    int gap = std::numeric_limits<int>::max();
    double accel_ratio = 0;

    for(int j=i; j<ntasks; j++)
    {
        if(tasks[j].memory_requirement<=available_memory_for_present_task)
        {
            int current_gap = early_available_comm_time + tasks[j].input_comm_time - early_available_comp_time;
            current_gap = max(0, current_gap);
            if(current_gap <= gap)
            {
                double current_accel_ratio = std::numeric_limits<double>::max();
                if(tasks[j].input_comm_time)
                    current_accel_ratio = tasks[j].comp_time/((double)tasks[j].input_comm_time);

                if((current_gap < gap) || ((gap == 0) && (current_accel_ratio > accel_ratio)))
                {
                    index = j;
                    gap = current_gap;
                    accel_ratio = current_accel_ratio;
                }
            }
        }
    }
    return index;
}



int find_next_smallest_communication_task_with_memory_restriction(Task tasks[], unsigned int i, unsigned int ntasks, double early_available_comm_time,
        double early_available_comp_time, unsigned int available_memory_for_present_task)
{

    int index = -1;
    int gap = std::numeric_limits<int>::max();
    unsigned int length = 0;

    for(int j=i; j<ntasks; j++)
    {
        if(tasks[j].memory_requirement<=available_memory_for_present_task)
        {
            int current_gap = early_available_comm_time + tasks[j].input_comm_time - early_available_comp_time;
            current_gap = max(0, current_gap);
            if(current_gap <= gap)
            {
                if((current_gap < gap) || ((gap == 0) && (tasks[j].input_comm_time < length)))
                {
                    index = j;
                    gap = current_gap;
                    length = tasks[j].input_comm_time;

                }
            }
        }
    }
    return index;
}


int find_next_largest_communication_task_with_memory_restriction(Task tasks[], unsigned int i, unsigned int ntasks, double early_available_comm_time,
        double early_available_comp_time, unsigned int available_memory_for_present_task)
{
    int index = -1;
    int gap = std::numeric_limits<int>::max();
    unsigned int length = 0;

    for(int j=i; j<ntasks; j++)
    {
        if(tasks[j].memory_requirement<=available_memory_for_present_task)
        {
            int current_gap = early_available_comm_time + tasks[j].input_comm_time - early_available_comp_time;
            current_gap = max(0, current_gap);
            if(current_gap <= gap)
            {
                if((current_gap < gap) || ((gap == 0) && (tasks[j].input_comm_time > length)))
                {
                    index = j;
                    gap = current_gap;
                    length = tasks[j].input_comm_time;

                }
            }
        }
    }
    return index;
}
template <class criteria>
void compute_schedule_next_task_heuristic(Task tasks[], unsigned int ntasks, criteria criteria_lambda, bool initial_order=false)
{
        unsigned int available_memory_for_present_task = CAPACITY;
        double early_available_comm_time = 0;
        double early_available_comp_time = 0;
        std::queue<Task> computation_queue;

        int index;

        unsigned int i=0;
        //k denotes the index of next task in computation channel 
        unsigned int k=0;
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
            else if((initial_order == true )&&(tasks[i].memory_requirement <= available_memory_for_present_task ))
            {
                tasks[i].start_comm_time = early_available_comm_time;
                early_available_comm_time += tasks[i].input_comm_time;
                tasks[i].end_comm_time = early_available_comm_time;
                available_memory_for_present_task -= tasks[i].memory_requirement;
                tasks[i].available_memory_after_comm_scheduling = available_memory_for_present_task;
                i++;
            }
            else if((index=criteria_lambda(tasks, i, ntasks, early_available_comm_time, early_available_comp_time, available_memory_for_present_task)) != -1)
            {
                Task temp = tasks[index];
                if(initial_order)
                {
                    for(int j=index; j>i; j--)
                        tasks[j] = tasks[j-1];
                }
                else
                tasks[index] = tasks[i];

                tasks[i] = temp;

//                cout<<temp.input_comm_time<<" "<<temp.comp_time<<"\n";
                
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
            //iss>>tasks[i].input_comm_time >> tasks[i].input_volume >> tasks[i].internal_memory_requirement >> tasks[i].comp_time ;
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
            cout<<"Number of tasks is 0 .. Exiting ...\n";
            exit(0);
        }
        if(max_capacity_requirement > CAPACITY)
        {
            cout<<"Some tasks need more than available memory capacity ... Exiting...\n";
            exit(0);
        }
    }
    else
    {
        cout <<"Input file is bad. Exiting ..\n";
        exit(0);
    }

    int alg_choice = -1;
    if(argc < 2)
    {
        cout<<"Need more command line parameter.. Exiting ..."<<endl;
    }
    else
        alg_choice = atoi(argv[1]);
    //cout<<"Enter Algorithm Choice(0-20)\n";
    //cin>>alg_choice;


//    ofstream outputfile("output.txt", ios::app);


    if(alg_choice == 0)
    {
        string alg_name = "optimal_time_infinite_case";

        //an optimal heuristic for infinite memory
        //sort tasks based on the ratio of communication time and computation time
        /*
           {
           Task tmp = tasks[0];
           unsigned int i = 0;

           for(auto j=1; j<ntasks; j++)
           {
           if(tasks[j].comp_time >= tasks[j].input_comm_time)
           {
           tasks[i] = tasks[j];
           tasks[j] = tasks[i+1];
           i++;
           }
           }
           tasks[i] = tmp;

           if(tasks[i].comp_time >= tasks[i].input_comm_time)
           i++;

           sort(tasks, tasks+i, commIncrease);
           sort(tasks+i, tasks+ntasks, compDecrease);
           }
         */

        optimal_order_infinite_memory(tasks, ntasks);

        double early_available_comm_time = 0;
        double early_available_comp_time = 0;

        for(auto i=0; i<ntasks; i++)
        {

            tasks[i].start_comm_time = early_available_comm_time;
            early_available_comm_time += tasks[i].input_comm_time;
            tasks[i].end_comm_time = early_available_comm_time;


            tasks[i].start_comp_time = max(early_available_comp_time, tasks[i].end_comm_time);
            early_available_comp_time = tasks[i].start_comp_time + tasks[i].comp_time;
            tasks[i].end_comp_time = early_available_comp_time;

        }
        print_schedule(alg_name, tasks, ntasks);

    }
    else if(alg_choice == 1)
    {   
        string alg_name="order_of_submission";

        compute_schedule_for_specified_order(tasks, ntasks);
        print_schedule(alg_name, tasks, ntasks);


    }
    else if(alg_choice == 2)
    {
        //Heuristic 1: order of heuristics is same as the order give by optimal strategy in infinite memory scenario
        //TODO: memory requirement of each task is not more than C

        string alg_name="order_of_optimal_strategy_infinite_memory";

        optimal_order_infinite_memory(tasks, ntasks);
        compute_schedule_for_specified_order(tasks, ntasks);
        print_schedule(alg_name, tasks, ntasks);

        /*
           while((i<ntasks) || (!computation_queue.empty()))
           {
           if(!computation_queue.empty())
           {
           if(computation_queue.front().end_comp_time <= early_available_comm_time)
           {
           available_memory_for_present_task += computation_queue.front().memory_requirement;
           computation_queue.pop();
           }
           }
           if((i<ntasks) && (tasks[i].memory_requirement<=available_memory_for_present_task))
           {
           tasks[i].start_comm_time = early_available_comm_time;
           early_available_comm_time += tasks[i].input_comm_time;
           tasks[i].end_comm_time = early_available_comm_time;

           tasks[i].start_comp_time = max(early_available_comp_time, tasks[i].end_comm_time);
           early_available_comp_time = tasks[i].start_comp_time + tasks[i].comp_time;
           tasks[i].end_comp_time = early_available_comp_time;

           computation_queue.push(tasks[i]);
           available_memory_for_present_task -= tasks[i].memory_requirement;
           i++;
           }
           else 
           {
           if(!computation_queue.empty())
           early_available_comm_time = max(early_available_comm_time, computation_queue.front().end_comp_time);
           }
           }
         */

    }
    else if(alg_choice==3)
    {
        //Heuristic Increasing orde of communication
        string alg_name="increasing_order_of_communication_strategy";
        sort(tasks, tasks+ntasks, commIncrease);
        //sort(tasks+i, tasks+ntasks, compDecrease);

        compute_schedule_for_specified_order(tasks, ntasks);
        print_schedule(alg_name, tasks, ntasks);
    }
    else if(alg_choice == 4)
    {
        //Heuristic: decreasing order of computation time
        string alg_name="decreasing_order_of_computation_strategy";
        //sort(tasks, tasks+ntasks, commIncrease);
        sort(tasks, tasks+ntasks, compDecrease);


        compute_schedule_for_specified_order(tasks, ntasks);
        print_schedule(alg_name, tasks, ntasks);
    }
    else if(alg_choice == 5)
    {
        //Heuristic: increasing order of communication + computation time
        string alg_name="increasing_order_of_communication_plus_computation_strategy";
        sort(tasks, tasks+ntasks, commPlusCompIncrease);
        //sort(tasks, tasks+ntasks, commPlusCompDecrease);


        compute_schedule_for_specified_order(tasks, ntasks);
        print_schedule(alg_name, tasks, ntasks);
    }
    else if(alg_choice == 6)
    {
        //Heuristic: decreasing order of communication + computation time
        string alg_name="decreasing_order_of_communication_plus_computation_strategy";
        //sort(tasks, tasks+ntasks, commPlusCompIncrease);
        sort(tasks, tasks+ntasks, commPlusCompDecrease);


        compute_schedule_for_specified_order(tasks, ntasks);
        print_schedule(alg_name, tasks, ntasks);
    }
    //TODO: heuristics based on input volume and intermediate meory requirements
    else if(alg_choice == 7)
    {
        //Heuristic: When communication channel is idle, the largest task is selected which respects memory requirement
        // and induces less idle time on computation channel

        string alg_name="largest_communication_task_respects_memory_restriction";
        compute_schedule_next_task_heuristic(tasks, ntasks,
                find_next_largest_communication_task_with_memory_restriction);

        print_schedule(alg_name, tasks, ntasks);
        /*
           unsigned int available_memory_for_present_task = CAPACITY;
           double early_available_comm_time = 0;
           double early_available_comp_time = 0;
           std::queue<Task> computation_queue;

           unsigned int i=0;

           while((i<ntasks) || (!computation_queue.empty()))
           {
           while((!computation_queue.empty()) && (computation_queue.front().end_comp_time <=
           early_available_comm_time))
           {
           available_memory_for_present_task += computation_queue.front().memory_requirement;
           computation_queue.pop();
           }

           int index = find_next_largest_task_with_memory_restriction(tasks, i, ntasks, early_available_comm_time,
           early_available_comp_time, available_memory_for_present_task);
           if(index != -1)
           {
           Task temp = tasks[index];
           tasks[index] = tasks[i];
           tasks[i] = temp;

           tasks[i].start_comm_time = early_available_comm_time;
           early_available_comm_time += tasks[i].input_comm_time;
           tasks[i].end_comm_time = early_available_comm_time;

           tasks[i].start_comp_time = max(early_available_comp_time, tasks[i].end_comm_time);
           early_available_comp_time = tasks[i].start_comp_time + tasks[i].comp_time;
           tasks[i].end_comp_time = early_available_comp_time;

           computation_queue.push(tasks[i]);
           available_memory_for_present_task -= tasks[i].memory_requirement;
           i++;
           }
           else 
           {
           if(!computation_queue.empty())
           early_available_comm_time = max(early_available_comm_time, computation_queue.front().end_comp_time);
           }
           }
         */
    }
    else if(alg_choice == 8)
    {
        //Heuristic: When communication channel is idle, the smallest task is selected which respects memory requirement
        // and induces less idle time on computation channel
        string alg_name="smallest_communication_task_respects_memory_restriction";
        compute_schedule_next_task_heuristic(tasks, ntasks,
                find_next_smallest_communication_task_with_memory_restriction);

        print_schedule(alg_name, tasks, ntasks);
    }
    else if(alg_choice == 9)
    {
        //Heuristic: When communication channel is idle, the maximum accelerated task is selected which respects memory requirement
        // and induces less idle time on computation channel
        string alg_name="maximum_accelerated_task_respects_memory_restriction";
        compute_schedule_next_task_heuristic(tasks, ntasks,
                find_next_smallest_communication_task_with_memory_restriction);

        print_schedule(alg_name, tasks, ntasks);
    }
    else if(alg_choice == 10)
    {
        //Heuristic: intial order of processing is same as the order give by optimal strategy in infinite memory
        // scenario, when a task is with restricted by memory, then another largest task who respects memory limit and
        // induces less idle time on computation channel is selected  

        string alg_name="optimal_order_infinite_memory_largest_communication_task_respects_memory_restriction";
        bool initial_order = true;
        optimal_order_infinite_memory(tasks, ntasks);
        compute_schedule_next_task_heuristic(tasks, ntasks,
                find_next_largest_communication_task_with_memory_restriction, initial_order);

        print_schedule(alg_name, tasks, ntasks);
    }
    else if(alg_choice == 11)
    {
        //Heuristic: intial order of processing is same as the order give by optimal strategy in infinite memory
        // scenario, when a task is with restricted by memory, then another smallest task who respects memory limit and
        // induces less idle time on computation channel is selected  
        string alg_name="optimal_order_infinite_memory_smallest_communication_task_respects_memory_restriction";
        bool initial_order = true;
        optimal_order_infinite_memory(tasks, ntasks);
        compute_schedule_next_task_heuristic(tasks, ntasks,
                find_next_largest_communication_task_with_memory_restriction, initial_order);

        print_schedule(alg_name, tasks, ntasks);
    }
    else if(alg_choice == 12)
    {
        //Heuristic: intial order of processing is same as the order give by optimal strategy in infinite memory
        // scenario, when a task is with restricted by memory, then maximum accelerated task who respects memory limit and
        // induces less idle time on computation channel is selected  

        string alg_name="optimal_order_infinite_memory_maximum_accelerated_task_respects_memory_restriction";
        bool initial_order = true;
        optimal_order_infinite_memory(tasks, ntasks);
        compute_schedule_next_task_heuristic(tasks, ntasks,
                find_next_maximum_accelerated_task_with_memory_restriction, initial_order);

        print_schedule(alg_name, tasks, ntasks);
    }
    else
    {
        cout<<"Unvalid scheduler choice \n";
    }

//    outputfile.close();


    delete[] tasks;

    return 0;
}

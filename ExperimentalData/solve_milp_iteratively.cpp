#include <iostream>
#include <string>
#include <algorithm>
#include <queue>
#include <limits>
#include <iomanip>
#include <fstream>
#include <sstream>

#include <communication_lp.h>

#define MAXNUMBEROFTASKS 10000
#define SUBSETMAXNUMBEROFTASKS 15

#ifndef CAPACITY
#define CAPACITY 0
#endif

#ifndef SCALEFACTOR
#define SCALEFACTOR 1
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
    cout<< alg_name << " " << makespan/SCALEFACTOR << endl;
//    outputfile<<alg_name <<" "<< makespan <<"\n";
}

void print_lp_schedule(int n, double start_point, int* task_id,double* start_comm_time, double* CM, double* start_comp_time,
        double* CP)
{

    cout <<"********** schedule (start point = " << start_point << ") ********\n";
    for(auto i=1; i<=n; i++)
    {
        cout << task_id[i] << " ";
        cout << start_point + start_comm_time[i] << " ";
        cout << start_point + start_comm_time[i] + CM[i] << " ";
        cout << start_point + start_comp_time[i] << " ";
        cout << start_point + start_comp_time[i] + CP[i] << "\n";
    }
}

double compute_makespan(int n, double start_lp_point, double* start_comp_time, double* CP)
{
    double makespan = 0;
    for(auto i=1; i<=n; i++)
    {
        if(start_comp_time[i] + CP[i] > makespan)
            makespan = start_comp_time[i] + CP[i];
    }
    makespan += start_lp_point;
    return makespan;
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
            tasks[i].input_comm_time *= SCALEFACTOR;
            tasks[i].comp_time *= SCALEFACTOR;
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

    int subset_size = 1;
    if(argc < 2)
    {
        cout<<"Need more command line parameter.. Exiting ..."<<endl;
    }
    else
        subset_size = atoi(argv[1]);

    int end_iter = 0;
    int nprevious_tasks = 0;

    double CM[SUBSETMAXNUMBEROFTASKS];
    double CP[SUBSETMAXNUMBEROFTASKS];
    unsigned int cap[SUBSETMAXNUMBEROFTASKS];
    unsigned int tm[SUBSETMAXNUMBEROFTASKS];
    int fixed_start_comm_time[SUBSETMAXNUMBEROFTASKS];
    int fixed_start_comp_time[SUBSETMAXNUMBEROFTASKS];
    double start_comm_time[SUBSETMAXNUMBEROFTASKS];
    double start_comp_time[SUBSETMAXNUMBEROFTASKS];
    int task_mapping[SUBSETMAXNUMBEROFTASKS];

    double start_lp_point = 0;

    while(end_iter < ntasks)
    {
        int start_iter = end_iter;
        end_iter = min(start_iter + subset_size - nprevious_tasks, ntasks);
        
        if(start_iter >= end_iter && start_iter < ntasks)
            end_iter = start_iter + 1;

        cout << "Start_iter = " << start_iter << "End_iter = " << end_iter <<std::endl;
        int a;

        int i;
        int j = 0;

        for(i=start_iter; i<end_iter; i++)
        {
            CM[nprevious_tasks+j+1] = tasks[i].input_comm_time;
            CP[nprevious_tasks+j+1] = tasks[i].comp_time;
            cap[nprevious_tasks+j+1] = tasks[i].memory_requirement;
            tm[nprevious_tasks+j+1] = 0;

            task_mapping[nprevious_tasks+j+1] = tasks[i].task_id;
            fixed_start_comm_time[nprevious_tasks+j+1] = 0;
            fixed_start_comp_time[nprevious_tasks+j+1] = 0;

            cout << "Task Id=" << task_mapping[nprevious_tasks+j+1] << " Comm =" << CM[nprevious_tasks+j+1] << " Comp= " <<
                CP[nprevious_tasks+j+1] << " CAP= " <<cap[nprevious_tasks+j+1] <<"\n";
            j++;
        }
        int n = nprevious_tasks + j ;
        unsigned int capacity = CAPACITY;
        formulate_lp(n, capacity, CM, CP, cap, tm, fixed_start_comm_time, start_comm_time, fixed_start_comp_time, start_comp_time);
        print_lp_schedule(n, start_lp_point, task_mapping, start_comm_time, CM, start_comp_time, CP);
        double makespan = compute_makespan(n, start_lp_point, start_comp_time, CP);
        //reset parameter



        for(i=1; i<=n; i++)
        {
            int task_id = task_mapping[i];
            if(!fixed_start_comm_time[i])
            {
            tasks[task_id].start_comm_time = start_lp_point + start_comm_time[i];
            tasks[task_id].end_comm_time = tasks[task_id].start_comm_time + CM[i];
            }
            
            if(!fixed_start_comp_time[i])
            {
            tasks[task_id].start_comp_time = start_lp_point + start_comp_time[i];
            tasks[task_id].end_comp_time = tasks[task_id].start_comp_time + CP[i];
            }
        }
        if(end_iter < ntasks)
        {
            nprevious_tasks = 0;

            double max_comm_time = 0;
            for(i=1; i<=n; i++)
            {
                if(start_comm_time[i] + CM[i] > max_comm_time)
                    max_comm_time = start_comm_time[i] + CM[i];
            }
            start_lp_point += max_comm_time;

            for(i=1; i<=n; i++)
            {
                if(start_comp_time[i] + CP[i] > max_comm_time)
                {
                    if(start_comp_time[i] <max_comm_time)
                    {
                        fixed_start_comp_time[nprevious_tasks+1] = 1;
                        CP[nprevious_tasks+1] = start_comp_time[i] + CP[i] - max_comm_time;
                        start_comp_time[nprevious_tasks+1] = 0;
                    }
                    else
                    {
                        fixed_start_comp_time[nprevious_tasks+1] = 0;
                        CP[nprevious_tasks+1] =  CP[i];
                    }

                    CM[nprevious_tasks+1] = 0;
                    fixed_start_comm_time[nprevious_tasks+1] = 1;
                    start_comm_time[nprevious_tasks+1] = 0;
                    task_mapping[nprevious_tasks+1] = task_mapping[i];
                    nprevious_tasks++;

                }
            }

            //assert(nprevious_tasks != n);
        }
    }

    print_schedule("lp_heuristic", tasks, ntasks);

}


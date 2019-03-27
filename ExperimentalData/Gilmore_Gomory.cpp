#include <iostream>
#include <string>
#include <algorithm>
#include <list>
#include <queue>
#include <vector>
#include <utility>
#include <cassert>

#include <limits>
#include <iomanip>
#include <fstream>
#include <sstream>

#define MAXNUMBEROFTASKS 10000
#define MAXNUMBEROFJOBS MAXNUMBEROFTASKS

#define LARGENUMBER 1.0e+18

#ifndef CAPACITY
#define CAPACITY 0
#endif

#ifndef PRINTCOMPLETESCHEDULE
#define PRINTCOMPLETESCHEDULE 0
#endif
using namespace std;

class Job
{
    public:
        int id;
        int new_id;
        double A;
        double B;
};
class ChangeCost
{
    public:
        double cost;
        int id;
};


class Task
{
    public:
        int rank;
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
};

bool sortByRank(Task x, Task y) {return x.rank <= y.rank;}
bool sortByValue(ChangeCost& x, ChangeCost& y) {return x.cost  < y.cost;}
bool sortByB(Job x, Job y) {return x.B <= y.B;}
bool sortByA(Job x, Job y) {return x.A <= y.A;}
bool sortByNewId(Job x, Job y) { return x.new_id <= y.new_id;}


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
bool find_in_tree(std::vector<std::pair<int,int> >& edges, int ntasks, int src, int dst)
{
    bool new_edge = true;

    bool *visited = new bool[ntasks+1];

    for(int i=1; i<=ntasks; i++)
        visited[i] = false;
    visited[src] = true;

    std::list<int> queue;
    queue.push_back(src);
    while(!queue.empty())
    {
        int v = queue.front();
        queue.pop_front();
        for(int i=0; i<edges.size(); i++)
        {
            if((edges[i].first == v) && (visited[edges[i].second] == false))
            {
                visited[edges[i].second] = true;
                queue.push_back(edges[i].second);
            }
            else if((edges[i].second == v) && (visited[edges[i].first] == false))
            {
                visited[edges[i].first] = true;
                queue.push_back(edges[i].first);
            }

        }
    }
    if(visited[dst]) new_edge = false;

    delete [] visited;
    if(new_edge)
    {
        //std::cout << "New Arc = (" << src << ", " << dst << ")" <<std::endl; 
        edges.push_back(std::make_pair(src,dst));
    }
    return new_edge;
}
int change_function(int x, int y)
{
    if(x == y)
        return x+1;
    else if ( (x+1) == y)
        return x;
    else 
        return y;
}

int main(int argc, char* argv[])
{
    int ntasks = 0;
    //cin>>ntasks;

    Task* tasks = new Task[MAXNUMBEROFTASKS];
    Job* jobs = new Job[MAXNUMBEROFJOBS];
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
            jobs[i+1].id = i+1;
            jobs[i+1].A = tasks[i].input_comm_time;
            jobs[i+1].B = tasks[i].comp_time;

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

    //adding dummy job

    jobs[ntasks+1].id = ntasks+1;
    jobs[ntasks+1].A = -LARGENUMBER;
    jobs[ntasks+1].B = LARGENUMBER;

    ntasks = ntasks + 1;

    int i;
    sort(jobs+1, jobs+ntasks+1, sortByB);
    for(i=1; i<=ntasks; i++)
        jobs[i].new_id = i;
    int *mapping_for_A = new int[MAXNUMBEROFJOBS];
    sort(jobs+1, jobs+ntasks+1, sortByA);
    for(i=1; i<=ntasks; i++)
        mapping_for_A[i] = jobs[i].new_id;
    sort(jobs+1, jobs+ntasks+1, sortByNewId);

    ChangeCost *change_cost = new ChangeCost[MAXNUMBEROFJOBS];

    for(i=1; i<=ntasks; i++)
    {
        change_cost[i].cost = 0;
        change_cost[i].id = i;
    }
    for(i=1; i<ntasks; i++)
    {
        double var1 = min(jobs[i+1].B, jobs[mapping_for_A[i+1]].A);
        double var2 = max(jobs[i].B, jobs[mapping_for_A[i]].A);
        if(var1 > var2)
            change_cost[i].cost = var1 - var2;
    }

    /*
       std::cout << " No. " << " B " << " A " << " No." <<std::endl;  

       for(i=ntasks; i>0; i--)
       std::cout << jobs[i].new_id << " " << jobs[i].B << " " << jobs[mapping_for_A[i]].A << " " << mapping_for_A[i] << std::endl; 

       std::cout << " *********** COST *********** " << std::endl;
       for(i=1; i<ntasks; i++)
       std::cout << "cost(" << i << "," << i+1 << ") = " << change_cost[i].cost << std::endl;
     */

    std::vector<std::pair<int,int> > edges;

    for(i=1; i<=ntasks; i++)
    {
        //removing self edges
        if(i != mapping_for_A[i])
            edges.push_back(std::make_pair(i, mapping_for_A[i]));
    }
    sort(change_cost +1, change_cost+ntasks, sortByValue);
/*
    sort(change_cost +1, change_cost+ntasks, [](ChangeCost& x, ChangeCost& y){return x.cost < y.cost;});
    for(i=1; i<ntasks; i++)
    {
        for(int j=i+1; j<ntasks; j++)
            if(change_cost[i].cost > change_cost[j].cost)
            {
                ChangeCost temp = change_cost[i];
                change_cost[i] = change_cost[j];
                change_cost[j] = temp;
            }
    }
    */
    std::vector<int> group1;
    std::vector<int> group2;
    for(i=1; i<ntasks; i++)
    {
        bool new_reachable = find_in_tree(edges, ntasks, change_cost[i].id, change_cost[i].id+1);
        if(new_reachable)
        {
            int j = change_cost[i].id;
            if(jobs[mapping_for_A[j]].A >= jobs[j].B)
                group1.push_back(j);
            else
                group2.push_back(j);
        }
    }
    //sort grp1 in increasing order
    sort(group1.begin(), group1.end(),[](int a, int b){ return a<b;} );

    //sort grp2 in decreasing order
    sort(group2.begin(), group2.end(),[](int a, int b){ return a>b;} );

    /*
    std::cout << "**** group1 **** " << std::endl;
    for(i=0; i<group1.size(); i++)
        std::cout << group1[i] << " ";
    std::cout <<std::endl;

    std::cout << "**** group2 **** " << std::endl;
    for(i=0; i<group2.size(); i++)
        std::cout << group2[i] << " ";
    std::cout <<std::endl;
    */

    int *successors = new int[MAXNUMBEROFJOBS];

    for(i=1; i<=ntasks; i++)
    {
        int successor = i;
        for(int j=0; j<group2.size(); j++)
            successor = change_function(group2[j], successor);
        for(int j=0; j<group1.size(); j++)
            successor = change_function(group1[j], successor);

        successors[i] = mapping_for_A[successor];

    }

    /*
    std::cout << "TaskId successor" << std::endl;

    for(i=1; i<=ntasks; i++)
        std::cout << i << " " << successors[i] << std::endl;

     */

    //std::cout << "NewId OriginalId Rank" << std::endl;
    int next_task = successors[ntasks];
    for(i=1; i<ntasks; i++)
    {
        assert(next_task == jobs[next_task].new_id);
        int original_id = jobs[next_task].id -1;
        tasks[original_id].rank = i;
        //std::cout << next_task << " " << original_id << " " << i << std::endl;
        next_task = successors[next_task];
    }
    assert(next_task == ntasks);
    delete [] successors;
    delete [] mapping_for_A;
    delete [] change_cost;

    delete [] jobs;


    ntasks--;
    sort(tasks, tasks + ntasks, sortByRank);

    string alg_name="order_of_Gilmore_Gomory";

    long unsigned int capacity = CAPACITY;
    long unsigned int inc = CAPACITY/10;
    for(i=0; i<10; i++)
    {

        compute_schedule_for_specified_order(tasks, ntasks, capacity);
        print_schedule(alg_name, tasks, ntasks, capacity);
        capacity += inc;

    }

    delete [] tasks;
}

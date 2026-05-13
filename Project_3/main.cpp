#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>

using namespace std;

const int INF = 1e9;

struct Task 
{
    int id; //identyfikator
    int pj; //czas wykonania
    int wj; //waga / współczynnik kary
    int dj; //żądany termin zakończenia
};

int calculateTWT(const vector<Task>& schedule)
{
    int currentTime = 0;
    int totalPenalty = 0;
    for(const auto& task : schedule)
    {
        currentTime+= task.pj;
        int tardiness = max(0, currentTime - task.dj);
        totalPenalty += tardiness * task.wj;
    }
    return totalPenalty;
}

vector<Task> generateInstance(int n, int seed, bool xIsSum)
{
    mt19937 gen(seed);
    uniform_int_distribution<> dist_p(1, 29);
    uniform_int_distribution<> dist_w(1, 9);

    vector<Task> tasks(n);
    int sum_p = 0;

    for(int i = 0; i < n; ++i)
    {
        tasks[i].id = i + 1;
        tasks[i].pj = dist_p(gen);
        sum_p += tasks[i].pj;
    }

    for (int i = 0; i < n; ++i) {
        tasks[i].wj = dist_w(gen);
    }

    int X = xIsSum ? sum_p : 29;
    uniform_int_distribution<> dist_d(1, X);
    
    for (int i = 0; i < n; ++i) {
        tasks[i].dj = dist_d(gen);
    }
    
    return tasks;
}

//Metoda zachłanna

pair<int, vector<Task>> greedyAlgorithm(vector<Task> tasks) {
    sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        return a.dj < b.dj; 
    });
    return {calculateTWT(tasks), tasks};
}

//Przegląd zupełny
pair<int, vector<Task>> bruteForce(vector<Task> tasks)
{
    sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b){
        return a.id < b.id;
    });

    int bestCost = INF;
    vector<Task> bestSchedule;

    do {
        int currentCost = calculateTWT(tasks);
        if (currentCost < bestCost) {
            bestCost = currentCost;
            bestSchedule = tasks;
        }
    } while (next_permutation(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        return a.id < b.id;
    }));
    
    return {bestCost, bestSchedule};
}

//Programowanie dynamiczne
pair<int, vector<Task>> dynamicProgramming(const vector<Task>& tasks)
{
    int n = tasks.size();
    int numStates = 1 << n;

    vector<int> memory(numStates, INF);
    vector<int> parent(numStates, -1);

    memory[0] = 0;

    for(int mask = 1; mask < numStates; ++mask)
    {
        int sum_p = 0;
        for (int i = 0; i < n; ++i) 
        {
            if (mask & (1 << i)) 
            { 
                sum_p += tasks[i].pj;
            }
        }

    

        for (int i = 0; i < n; ++i) 
        {
                if (mask & (1 << i)) 
                {
                    int prev_mask = mask ^ (1 << i);
                    int tardiness = max(0, sum_p - tasks[i].dj);
                    int cost = memory[prev_mask] + tardiness * tasks[i].wj;
                
              
                  if (cost < memory[mask]) 
                    {
                        memory[mask] = cost;
                        parent[mask] = i; 
                    }
                }
        }
    }

    int current_mask = numStates - 1; 
    vector<Task> optimalSchedule;

    while (current_mask > 0) {
        int last_task_idx = parent[current_mask];
        optimalSchedule.push_back(tasks[last_task_idx]);
        current_mask ^= (1 << last_task_idx);
    }
    
    reverse(optimalSchedule.begin(), optimalSchedule.end());
    
    return {memory[numStates - 1], optimalSchedule};


}

void printSchedule(const std::vector<Task>& schedule)
{
    for (const auto& t : schedule) 
    {
        cout << t.id << " ";
    }
    cout << endl;
}




int main()
{
    int n = 10; 
    int seed = 42;

    cout << "ROZMIAR PROBLEMU n = " << n << endl;
    cout << "===============================\n";
    
    cout << "--- TEST DLA X = A (Suma P_i) ---\n";
    auto tasks_A = generateInstance(n, seed, true);
    
    auto greedy_A = greedyAlgorithm(tasks_A);
    cout << "Zachlanny:        " << greedy_A.first << " \t| Kolejnosc: ";
    printSchedule(greedy_A.second);
    
    auto bf_A = bruteForce(tasks_A);
    cout << "Brute Force:      " << bf_A.first << " \t| Kolejnosc: ";
    printSchedule(bf_A.second);
    
    auto dp_A = dynamicProgramming(tasks_A);
    cout << "Prog. Dynamiczne: " << dp_A.first << " \t| Kolejnosc: ";
    printSchedule(dp_A.second);
    
    cout << "\n";
    
    cout << "--- TEST DLA X = 29 ---\n";
    auto tasks_29 = generateInstance(n, seed, false);
    
    auto greedy_29 = greedyAlgorithm(tasks_29);
    cout << "Zachlanny:        " << greedy_29.first << " \t| Kolejnosc: ";
    printSchedule(greedy_29.second);
    
    auto bf_29 = bruteForce(tasks_29);
    cout << "Brute Force:      " << bf_29.first << " \t| Kolejnosc: ";
    printSchedule(bf_29.second);
    
    auto dp_29 = dynamicProgramming(tasks_29);
    cout << "Prog. Dynamiczne: " << dp_29.first << " \t| Kolejnosc: ";
    printSchedule(dp_29.second);


    return 0; 
}
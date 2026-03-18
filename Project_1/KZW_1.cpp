#include <iostream>
#include <vector>
#include <random>
#include <queue>
#include <unordered_map>


using namespace std;

struct zadanie
{
    int id; // numer zadania
    int rj; // czas przygotowania/termin dostępności
    int pj; // czas wykonania
    int qj; // czas dostarczenia/stygnięcia
};

struct planowane_wyniki
{
    unordered_map<int, int> S; // Momenty rozpoczęcia, indeksowane po ID zadania
    unordered_map<int, int> C; // Momenty zakończenia, indeksowane po ID zadania
    int Cmax; // Czas zakończenia ostatniego zadania

};

vector<zadanie> generatorInst(int n, int seed, int X)
{
    mt19937 gen(seed); //generowanie ziarna
    uniform_int_distribution<int> pDist(1, 29); 
    vector<int> p(n);
    int A = 0;

    for (int j = 0; j < n; j++)
    {
        p[j] = pDist(gen);
        A += p[j];
    }

    uniform_int_distribution<int> rDist(1, A);
    uniform_int_distribution<int> qDist(1, X);

    vector<zadanie> zadania(n);

    for (int j = 0; j < n; j++)
    {
        zadania[j].pj = p[j];
        zadania[j].id = j + 1;
        zadania[j].rj = rDist(gen);
        zadania[j].qj = qDist(gen);
    }

    return zadania;
}

planowane_wyniki Calculate(vector<zadanie> zadania)
{
    planowane_wyniki wyniki;
    wyniki.Cmax = 0;
    
    // Obliczamy czasy dla zadań w danej kolejności
    int current_time = 0;
    for (int j = 0; j < zadania.size(); j++)
    {
        int start = max(zadania[j].rj, current_time);
        int end = start + zadania[j].pj;
        
        wyniki.S[zadania[j].id] = start;
        wyniki.C[zadania[j].id] = end;
        wyniki.Cmax = max(wyniki.Cmax, end + zadania[j].qj);
        
        current_time = end;
    }

    return wyniki;
}

vector<zadanie> schrage(vector<zadanie> zadania)
{
    int k = 1;      
    return zadania;
}

// do kolejki N -> najmniejsze r na górze
struct CompareR
{
    bool operator()(const zadanie& a, const zadanie& b) const
    {
        return a.rj > b.rj;
    }
};

// do kolejki G -> największe q na górze
struct CompareQ
{
    bool operator()(const zadanie& a, const zadanie& b) const
    {
        return a.qj < b.qj;
    }
};


vector<zadanie> schrage_heap(vector<zadanie> zadania)
{
    priority_queue<zadanie, vector<zadanie>, CompareR> N;
    priority_queue<zadanie, vector<zadanie>, CompareQ> G;
    vector<zadanie> wynik;
    // wrzucenie wszystkich zadań do N
    for (const zadanie& task : zadania)
    {
        N.push(task);
    }
    int t = 0;
    
    while (!G.empty() || !N.empty())
    {
        // przenosimy wszystkie dostępne zadania z N do G
        while (!N.empty() && N.top().rj <= t)
        {
            G.push(N.top());
            N.pop();
        }
        if (G.empty())
        {
            t = N.top().rj; // jeśli G jest puste, przeskocz do momentu dostępności następnego zadania
        }
        else
        {
            zadanie current = G.top();
            G.pop();
            t += current.pj; // wykonaj zadanie
            wynik.push_back(current);
        }
    }
    return wynik;
}

int main()
{
    vector<zadanie> test1 = generatorInst(6, 42, 29);
    int A = 0;
    for (auto& j : test1)
    {
        A += j.pj;
    }

    for(int i = 0; i < test1.size(); i++)
    {
        cout << "Zadanie " << test1[i].id << ": r = " << test1[i].rj << ", p = " << test1[i].pj << ", q = " << test1[i].qj << endl;
    }

    vector<zadanie> wynik1 = schrage_heap(test1);

    for(int i = 0; i < wynik1.size(); i++)
    {
        cout << "Zadanie " << wynik1[i].id << ": r = " << wynik1[i].rj << ", p = " << wynik1[i].pj << ", q = " << wynik1[i].qj << endl;
    }

    planowane_wyniki wyniki1 = Calculate(test1);
    for (int i = 1; i <= test1.size(); i++)
    {
        cout << "Zadanie " << i << ": S = " << wyniki1.S[i] << ", C = " << wyniki1.C[i] << endl;
    }
    cout << "Cmax: " << wyniki1.Cmax << endl;
    
    
    planowane_wyniki wyniki1_schrage = Calculate(wynik1);
    for (int i = 0; i < wynik1.size(); i++)
    {
        cout << "Zadanie " << wynik1[i].id << ": S = " << wyniki1_schrage.S[wynik1[i].id] << ", C = " << wyniki1_schrage.C[wynik1[i].id] << endl;
    }
    cout << "Cmax: " << wyniki1_schrage.Cmax << endl;



    vector<zadanie> test2 = generatorInst(6, 42, A);
    planowane_wyniki wyniki2 = Calculate(test2);
    for (int i = 1; i <= test2.size(); i++)
    {
        cout << "Zadanie " << i << ": S = " << wyniki2.S[i] << ", C = " << wyniki2.C[i] << endl;
    }
    cout << "Cmax: " << wyniki2.Cmax << endl;
    return 0;
}

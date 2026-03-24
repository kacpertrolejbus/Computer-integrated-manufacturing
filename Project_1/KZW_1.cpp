using namespace std;

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <queue>

struct zadanie
{
    int id; // numer zadania
    int rj; // czas przygotowania/termin dostępności
    int pj; // czas wykonania
    int qj; // czas dostarczenia/stygnięcia
    int pozostale_p; //czas, który został do wykonania
};

struct planowane_wyniki
{
    vector<int> S; // Momenty rozpoczęcia wykonywania zadań
    vector<int> C; // Momenty zakończenia wykonywania zadań
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
        zadania[j].pozostale_p = p[j];
        zadania[j].id = j + 1;
        zadania[j].rj = rDist(gen);
        zadania[j].qj = qDist(gen);
    }

    return zadania;
}

planowane_wyniki Calculate(vector<zadanie> zadania)
{
    planowane_wyniki wyniki;
    wyniki.S.resize(zadania.size());
    wyniki.C.resize(zadania.size());
    wyniki.S[0] = zadania[0].rj;
    wyniki.C[0] = wyniki.S[0] + zadania[0].pj;
    wyniki.Cmax = wyniki.C[0] + zadania[0].qj;
    for (int j = 1; j < zadania.size(); j++)
    {
        wyniki.S[j] = max(zadania[j].rj, wyniki.C[j - 1]);
        wyniki.C[j] = wyniki.S[j] + zadania[j].pj;
        wyniki.Cmax = max(wyniki.Cmax, wyniki.C[j] + zadania[j].qj);
    }


    return wyniki;
}

struct PQj{
    bool operator()(const zadanie& a, const zadanie& b){
        return a.qj < b.qj;
    }
};

int schragePMTN(vector<zadanie> N)
{
    int t = 0;
    int Cmax = 0;
    priority_queue<zadanie, vector<zadanie>, PQj> G;

    zadanie l;
    l.id = 0;
    l.qj = 0;
    l.pozostale_p = 0;

    sort(N.begin(), N.end(), [](const zadanie& a, const zadanie& b){
        return a.rj > b.rj;
    });

    while(!N.empty() || !G.empty() || l.pozostale_p > 0)
    {
        while (!N.empty() && N.back().rj <= t)
        {
            zadanie j = N.back();
            N.pop_back();
            G.push(j);

            if(j.qj > l.qj)
            {
                cout << "Przerwanie zadania " << j.id << "!\n Zdejmuje z maszyny zadanie " << l.id << endl;
                l.pozostale_p = l.pozostale_p - (t - j.rj);
                t = j.rj;
                if(l.pozostale_p > 0)
                {
                    G.push(l);
                }
            }
        }

        if(G.empty() && !N.empty() && l.pozostale_p == 0)
        {
            t = N.back().rj;
            continue;
        }

        if(l.pozostale_p == 0 && !G.empty())
        {
            l = G.top();
            G.pop();
            cout << "[Czas " << t << "] -> Zadanie " << l.id << " wchodzi na maszyne (zostalo mu " << l.pozostale_p << " czasu)" << endl;
        }

        if(l.pozostale_p > 0)
        {
            int next_t;
            if(!N.empty())
            {
                next_t = min(t + l.pozostale_p, N.back().rj);
            }
            else{
                next_t = t + l.pozostale_p;
            }

            l.pozostale_p -= (next_t - t);
            t = next_t;

            if(l.pozostale_p == 0)
            {
                Cmax = max(Cmax, t + l.qj);
                cout << "[Czas " << t << "] <- ZAKONCZENIE zadania " << l.id << " (aktualny rekord Cmax: " << Cmax << ")" << endl;
            }
        }
    }

    return Cmax;
}



int main()
{
    vector<zadanie> test1 = generatorInst(6, 42, 29);
    int A = 0;
    for (auto& j : test1)
    {
        A += j.pj;
    }
    planowane_wyniki wyniki1 = Calculate(test1);
    for (int i = 0; i < test1.size(); i++)
    {
        cout << "Zadanie " << test1[i].id << ": S = " << wyniki1.S[i] << ", C = " << wyniki1.C[i] << endl;
    }
    cout << "Cmax: " << wyniki1.Cmax << endl;

    int ostateczny_cmax_1 = schragePMTN(test1);
    cout << "WYNIK SCHRAGE PMTN 1: " << ostateczny_cmax_1 << endl;

    vector<zadanie> test2 = generatorInst(6, 42, A);
    planowane_wyniki wyniki2 = Calculate(test2);
    for (int i = 0; i < test2.size(); i++)
    {
        cout << "Zadanie " << test2[i].id << ": S = " << wyniki2.S[i] << ", C = " << wyniki2.C[i] << endl;
    }
    cout << "Cmax: " << wyniki2.Cmax << endl;

    int ostateczny_cmax_2 = schragePMTN(test2);
    cout << "WYNIK SCHRAGE PMTN 2: " << ostateczny_cmax_2 << endl;
    return 0;
}
 
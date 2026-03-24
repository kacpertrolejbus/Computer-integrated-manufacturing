using namespace std;
using std::begin, std::end;

#include <iostream>
#include <vector>
#include <random>

struct zadanie
{
    int id; // numer zadania
    int rj; // czas przygotowania/termin dostępności
    int pj; // czas wykonania
    int qj; // czas dostarczenia/stygnięcia
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


zadanie minimum(const vector<zadanie>& zadania)
{
    return *min_element(zadania.begin(), zadania.end(),
        [](const zadanie& a, const zadanie& b)
        {
            return a.rj < b.rj;
        });
}

zadanie maximum(const vector<zadanie>& zadania)
{
    return *max_element(zadania.begin(), zadania.end(),
        [](const zadanie& a, const zadanie& b)
        {
            return a.qj < b.qj;
        });
}

vector<zadanie> schrage(vector<zadanie> zadania)
{
    vector<zadanie> pusty;
    vector<zadanie> pelny = zadania;
    vector<zadanie> permutacja;

    int t = minimum(pelny).rj;
    int Cmax = 0;

    while (!pusty.empty() || !pelny.empty())
    {
        while (!pelny.empty() && minimum(pelny).rj <= t)
        {
            auto it = min_element(pelny.begin(), pelny.end(),
                [](const zadanie& a, const zadanie& b)
                {
                    return a.rj < b.rj;
                });

            pusty.push_back(*it);
            pelny.erase(it);
        }

        if (!pusty.empty())
        {
            auto it = max_element(pusty.begin(), pusty.end(),
                [](const zadanie& a, const zadanie& b)
                {
                    return a.qj < b.qj;
                });

            zadanie j = *it;
            pusty.erase(it);

            permutacja.push_back(j);
            t += j.pj;
            Cmax = max(Cmax, t + j.qj);
        }
        else
        {
            t = minimum(pelny).rj;
        }
    }

    return permutacja;
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

    vector<zadanie> test2 = generatorInst(6, 42, A);
    planowane_wyniki wyniki2 = Calculate(test2);
    for (int i = 0; i < test2.size(); i++)
    {
        cout << "Zadanie " << test2[i].id << ": S = " << wyniki2.S[i] << ", C = " << wyniki2.C[i] << endl;
    }
    cout << "Cmax: " << wyniki2.Cmax << endl;
    return 0;
}

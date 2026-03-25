#include <iostream>
#include <vector>
#include <random>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <climits>


using namespace std;

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

struct PQj{
    bool operator()(const zadanie& a, const zadanie& b){
        return a.qj < b.qj;
    }
};

int schragePMTN(vector<zadanie> N, bool czyprint = true)
{
    int t = 0;
    int Cmax = 0;
    priority_queue<zadanie, vector<zadanie>, PQj> G; //Zbiór gotowych zadań

    // Inicjalizuj pozostale_p na pj dla każdego zadania
    for(auto& z : N) {
        z.pozostale_p = z.pj;
    }

    zadanie l; //obecne zadanie
    l.id = 0;
    l.qj = 0;
    l.pozostale_p = 0; //ustawione na 0 ponieważ łatwe do przebicia 

    sort(N.begin(), N.end(), [](const zadanie& a, const zadanie& b){
        return a.rj > b.rj;
    }); //posortowane malejąco

    while(!N.empty() || !G.empty() || l.pozostale_p > 0) //główna petla 
    {
        while (!N.empty() && N.back().rj <= t) //odbiór dostaw z N do G
        {
            zadanie j = N.back();
            N.pop_back();
            G.push(j);

            if(j.qj > l.qj) //jeśli nowe zadanie ma większe qj niż to na maszynie to przerywamy 
            {
                if(czyprint)
                cout << "Przerwanie zadania " << j.id << "!\n Zdejmuje z maszyny zadanie " << l.id << endl;
                l.pozostale_p = l.pozostale_p - (t - j.rj);
                t = j.rj;
                if(l.pozostale_p > 0)
                {
                    G.push(l);
                }
            }
        }

        if(G.empty() && !N.empty() && l.pozostale_p == 0) //skok w czasie  do najbliższego rj w N jeśli nie ma gotowych zadań
        {
            t = N.back().rj;
            continue; //powrót na początek pętli
        }

        if(l.pozostale_p == 0 && !G.empty()) //jeśli maszyna jest wolna lub zadanie się skończyło to bierzemy nowe G
        {
            l = G.top();
            G.pop();
            if(czyprint)
            cout << "[Czas " << t << "] -> Zadanie " << l.id << " wchodzi na maszyne (zostalo mu " << l.pozostale_p << " czasu)" << endl;
        }

        if(l.pozostale_p > 0) //wykonanie zadania na maszynie
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
                if(czyprint)
                cout << "[Czas " << t << "] <- ZAKONCZENIE zadania " << l.id << " (aktualny rekord Cmax: " << Cmax << ")" << endl;
            }
        }
    }

    return Cmax;
}

int hBoundFromTasks(const vector<zadanie>& tasks)
{
    if(tasks.empty())
    {
        return 0;
    }

    int minR = INT_MAX;
    int minQ = INT_MAX;
    int sumP = 0;

    for(const auto& z : tasks)
    {
        minR = min(minR, z.rj);
        minQ = min(minQ, z.qj);
        sumP += z.pj;
    }

    return minR + sumP + minQ;
}

void carlierRekurencja(vector<zadanie>& zadania, int& UB, vector<zadanie>& najlepszaPermutacja)
{
    vector<zadanie> pi = schrage(zadania);
    planowane_wyniki wyniki = Calculate(pi);
    int U = wyniki.Cmax;

    if(U < UB)
    {
        UB = U;
        najlepszaPermutacja = pi;
    }

    int n = static_cast<int>(pi.size());
    if(n == 0)
    {
        return;
    }

    vector<int> C(n, 0);
    int t = 0;
    for(int i = 0; i < n; i++)
    {
        t = max(t, pi[i].rj) + pi[i].pj;
        C[i] = t;
    }

    int b = -1;
    for(int i = n - 1; i >= 0; i--)
    {
        if(C[i] + pi[i].qj == U)
        {
            b = i;
            break;
        }
    }

    if(b == -1)
    {
        return;
    }

    int a = -1;
    int sumP = 0;
    for(int i = b; i >= 0; i--)
    {
        sumP += pi[i].pj;
        if(pi[i].rj + sumP + pi[b].qj == U)
        {
            a = i;
        }
    }

    if(a == -1)
    {
        return;
    }

    int c = -1;
    for(int i = b - 1; i >= a; i--)
    {
        if(pi[i].qj < pi[b].qj)
        {
            c = i;
            break;
        }
    }

    if(c == -1)
    {
        return;
    }

    int rK = INT_MAX;
    int qK = INT_MAX;
    int pK = 0;

    for(int i = c + 1; i <= b; i++)
    {
        rK = min(rK, pi[i].rj);
        qK = min(qK, pi[i].qj);
        pK += pi[i].pj;
    }

    int idC = pi[c].id;
    int idxC = -1;
    for(int i = 0; i < static_cast<int>(zadania.size()); i++)
    {
        if(zadania[i].id == idC)
        {
            idxC = i;
            break;
        }
    }

    if(idxC == -1)
    {
        return;
    }

    int oldR = zadania[idxC].rj;
    int oldQ = zadania[idxC].qj;

    zadania[idxC].rj = max(zadania[idxC].rj, rK + pK);
    {
        int LBpmtn = schragePMTN(zadania, false);
        vector<zadanie> zbiorK = vector<zadanie>(pi.begin() + c + 1, pi.begin() + b + 1);
        vector<zadanie> zbiorKc = zbiorK;
        zbiorKc.push_back(zadania[idxC]);

        int LB = max({LBpmtn, hBoundFromTasks(zbiorK), hBoundFromTasks(zbiorKc)});
        if(LB < UB)
        {
            carlierRekurencja(zadania, UB, najlepszaPermutacja);
        }
    }
    zadania[idxC].rj = oldR;

    zadania[idxC].qj = max(zadania[idxC].qj, qK + pK);
    {
        int LBpmtn = schragePMTN(zadania, false);
        vector<zadanie> zbiorK = vector<zadanie>(pi.begin() + c + 1, pi.begin() + b + 1);
        vector<zadanie> zbiorKc = zbiorK;
        zbiorKc.push_back(zadania[idxC]);

        int LB = max({LBpmtn, hBoundFromTasks(zbiorK), hBoundFromTasks(zbiorKc)});
        if(LB < UB)
        {
            carlierRekurencja(zadania, UB, najlepszaPermutacja);
        }
    }
    zadania[idxC].qj = oldQ;
}

vector<zadanie> carlier(vector<zadanie> zadania)
{
    vector<zadanie> oryginalneZadania = zadania;
    int UB = INT_MAX;
    vector<zadanie> najlepszaPermutacja;
    carlierRekurencja(zadania, UB, najlepszaPermutacja);

    unordered_map<int, zadanie> poId;
    for(const auto& z : oryginalneZadania)
    {
        poId[z.id] = z;
    }

    vector<zadanie> wynikOryginalny;
    for(const auto& z : najlepszaPermutacja)
    {
        wynikOryginalny.push_back(poId[z.id]);
    }

    return wynikOryginalny;
}
int main()
{   cout << "generowanie instancji" << endl;
    vector<zadanie> test1 = generatorInst(6, 42, 29);
    planowane_wyniki wyniki1 = Calculate(test1);
    int A = 0;
    for (auto& j : test1)
    {
        A += j.pj;
    }
    cout << "instancja 1" << endl;
    for(int i = 0; i < test1.size(); i++)
    {
        cout << "Zadanie " << test1[i].id << ": r = " << test1[i].rj << ", p = " << test1[i].pj << ", q = " << test1[i].qj << endl;
    }
    cout << "Cmax: " << wyniki1.Cmax << endl;
    
   
    cout << "generowanie instancji 2 i wyniki" << endl;
    vector<zadanie> test2 = generatorInst(6, 42, A);
    planowane_wyniki wyniki2 = Calculate(test2);
    for (int i = 1; i <= test2.size(); i++)
    {
        cout << "Zadanie " << i << ": S = " << wyniki2.S[i] << ", C = " << wyniki2.C[i] << endl;
    }
    cout << "Cmax: " << wyniki2.Cmax << endl;
    
    

    // schrage dla test1
    cout << "Schrage standardowy" << endl;
    vector<zadanie> perm1 = schrage(test1);
    planowane_wyniki wyn_sch1 = Calculate(perm1);
    cout << "Schrage - permutacja (test1) id: ";
    for (auto& z : perm1) cout << z.id << " ";
    cout << endl;
    cout << "Cmax (po permutacji Schrage): " << wyn_sch1.Cmax << endl;

    // schrage dla test2
    vector<zadanie> perm2 = schrage(test2);
    planowane_wyniki wyn_sch2 = Calculate(perm2);
    cout << "Schrage - permutacja (test2) id: ";
    for (auto& z : perm2) cout << z.id << " ";
    cout << endl;
    cout << "Cmax (po permutacji Schrage): " << wyn_sch2.Cmax << endl;
  


    // schrage heap dla test1
    cout << "Schrage heap" << endl;
    vector<zadanie> perm1h = schrage_heap(test1);
    planowane_wyniki wyn_schh1 = Calculate(perm1h);
    cout << "Schrage - permutacja (test1) id: ";
    for (auto& z : perm1h) cout << z.id << " ";
    cout << endl;
    cout << "Cmax (po permutacji Schrage heap): " << wyn_schh1.Cmax << endl;

    // schrage heap dla test2
    vector<zadanie> perm2h = schrage_heap(test2);
    planowane_wyniki wyn_schh2 = Calculate(perm2h);
    cout << "Schrage - permutacja (test2) id: ";
    for (auto& z : perm2h) cout << z.id << " ";
    cout << endl;
    cout << "Cmax (po permutacji Schrage heap): " << wyn_schh2.Cmax << endl;
    

    // schrage PMTN dla test1
    cout << "Schrage PMTN" << endl;
    int CmaxPMTN1 = schragePMTN(test1);
    cout << "Cmax (po permutacji Schrage PMTN): " << CmaxPMTN1 << endl;

    // schrage PMTN dla test2
    int CmaxPMTN2 = schragePMTN(test2);
    cout << "Cmax (po permutacji Schrage PMTN): " << CmaxPMTN2 << endl;

    //carlier dla test1
    cout << "Carlier" << endl;
    vector<zadanie> carlierPerm1 = carlier(test1);
    planowane_wyniki wyn_carlier1 = Calculate(carlierPerm1);
    cout << "Carlier - permutacja (test1) id: ";
    for (auto& z : carlierPerm1) cout << z.id << " ";
    cout << endl;
    cout << "Cmax (po permutacji Carliera): " << wyn_carlier1.Cmax << endl;

    //carlier dla test2
    vector<zadanie> carlierPerm2 = carlier(test2);
    planowane_wyniki wyn_carlier2 = Calculate(carlierPerm2);
    cout << "Carlier - permutacja (test2) id: ";
    for (auto& z : carlierPerm2) cout << z.id << " ";   
    cout << endl;
    cout << "Cmax (po permutacji Carliera): " << wyn_carlier2.Cmax << endl;

     return 0;

}

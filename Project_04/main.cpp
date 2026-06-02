#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <climits>

using namespace std;

struct Operacja
{
    int id_zadania;
    int id_maszyny;
    int czas_trwania;
    int indeks_w_zadaniu;
};

struct Zadanie
{
    int id_zadania;
    vector<Operacja> operacje;
    int suma_czasow = 0;
};

typedef vector<vector<Operacja>> Harmonogram;

vector<Zadanie> generujInstancje(int n, int m, int ziarno)
{
    mt19937 gen(ziarno);

    int max_operacji = static_cast<int>(m * 1.2);
    if (max_operacji < 1) max_operacji = 1;

    vector<Zadanie> zadania(n);
    uniform_int_distribution<> dyst_o(1, max_operacji);
    uniform_int_distribution<> dyst_p(1, 29);
    uniform_int_distribution<> dyst_m(0, m-1);

    for(int j = 0; j < n; ++j)
    {
        zadania[j].id_zadania = j;
        int o_j  = dyst_o(gen);

        for(int k = 0; k < o_j; ++k)
        {
            Operacja op;
            op.id_zadania = j;
            op.indeks_w_zadaniu = k;
            op.czas_trwania = dyst_p(gen);
            op.id_maszyny = dyst_m(gen);

            zadania[j].operacje.push_back(op);
            zadania[j].suma_czasow += op.czas_trwania;
        }
    }
    return zadania;
}

int obliczCmax(const Harmonogram& plan, int n, int m)
{
    vector<int> czas_maszyny(m, 0);
    vector<int> czas_zadania(n, 0);
    vector<int> indeks_na_maszynie(m, 0);
    vector<int> indeks_w_zadaniu(n, 0);

    int c_max = 0;
    int przetworzone_operacje = 0;
    int calkowita_liczba_operacji = 0;

    for(int i = 0; i < m; ++i)
    {
        calkowita_liczba_operacji += plan[i].size();
    }

    while(przetworzone_operacje < calkowita_liczba_operacji)
    {
        bool czy_cos_wykonano = false;

        for(int i = 0; i < m; ++i)
        {
            if(indeks_na_maszynie[i] < plan[i].size())
            {
                const Operacja& op = plan[i][indeks_na_maszynie[i]];

                if(indeks_w_zadaniu[op.id_zadania] == op.indeks_w_zadaniu)
                {
                    int start = max(czas_maszyny[i], czas_zadania[op.id_zadania]);
                    int koniec = start + op.czas_trwania;

                    czas_maszyny[i] = koniec;
                    czas_zadania[op.id_zadania] = koniec;

                    if(koniec > c_max)
                    {
                        c_max = koniec;
                    }

                    indeks_na_maszynie[i]++;
                    indeks_w_zadaniu[op.id_zadania]++;
                    przetworzone_operacje++;
                    czy_cos_wykonano = true;
                }
            }
        }

        if(!czy_cos_wykonano)
        {
            return INT_MAX;
        }
    }
    return c_max;
}

Harmonogram algorytmINSA(vector<Zadanie> zadania, int m)
{
    sort(zadania.begin(), zadania.end(), [](const Zadanie& a, const Zadanie& b)
{
    return a.suma_czasow > b.suma_czasow;
});

Harmonogram ostateczy_plan(m);

for(const auto& zadanie : zadania)
{
    for(const auto& operacja : zadanie.operacje)
    {
        int maszyna = operacja.id_maszyny;
        int najlepsza_pozycja = -1;
        int najlepszy_cmax = INT_MAX;

        int liczba_mozliwych_miejsc = ostateczy_plan[maszyna].size() + 1;

        for(int pozycja = 0; pozycja < liczba_mozliwych_miejsc; ++pozycja)
        {
            Harmonogram testowy_plan = ostateczy_plan;

            testowy_plan[maszyna].insert(testowy_plan[maszyna].begin() + pozycja, operacja);

            int aktualny_cmax = obliczCmax(testowy_plan, zadania.size(), m);

            if(aktualny_cmax < najlepszy_cmax)
            {
                najlepszy_cmax = aktualny_cmax;
                najlepsza_pozycja = pozycja;
            }
        }
        ostateczy_plan[maszyna].insert(ostateczy_plan[maszyna].begin() + najlepsza_pozycja, operacja);

    }
}
return ostateczy_plan;
}

void wypiszInstancje(const vector<Zadanie>& zadania) {
    cout << "--- WYGENEROWANE ZADANIA ---" << endl;
    for (const auto& zadanie : zadania) {
        cout << "Zadanie " << zadanie.id_zadania << " (suma czasow: " << zadanie.suma_czasow << "):" << endl;
        for (const auto& op : zadanie.operacje) {
            cout << "  Operacja " << op.indeks_w_zadaniu 
                 << " -> Maszyna: " << op.id_maszyny 
                 << ", Czas: " << op.czas_trwania << endl;
        }
    }
    cout << "----------------------------" << endl;
}

void wypiszHarmonogram(const Harmonogram& plan) {
    cout << "--- OSTATECZNY HARMONOGRAM NA MASZYNACH ---" << endl;
    for (int i = 0; i < plan.size(); ++i) {
        cout << "Maszyna " << i << ": ";
        if (plan[i].empty()) {
            cout << "Brak przypisanych operacji";
        } else {
            for (const auto& op : plan[i]) {
                cout << "[Z" << op.id_zadania << "_O" << op.indeks_w_zadaniu << " (czas: " << op.czas_trwania << ")] ";
            }
        }
        cout << endl;
    }
    cout << "-------------------------------------------" << endl;
}

int main()
{
    int n = 10;       // liczba zadań
    int m = 5;        // liczba maszyn
    int ziarno = 123; 

    cout << "Generowanie instancji dla n=" << n << ", m=" << m << " (ziarno: " << ziarno << ")" << endl;
    vector<Zadanie> zadania = generujInstancje(n, m, ziarno);

    wypiszInstancje(zadania);

    cout << "\nUruchamianie algorytmu INSA..." << endl;
    Harmonogram wynik = algorytmINSA(zadania, m);

    cout << endl;
    wypiszHarmonogram(wynik);

    int ostateczny_cmax = obliczCmax(wynik, n, m);
    cout << "--- ZAKONCZONO ---" << endl;
    cout << "Zoptymalizowany czas C_max wynosi: " << ostateczny_cmax << endl;

    return 0;
}
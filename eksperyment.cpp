#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <random>
#include <cmath>

using namespace std;
using namespace std::chrono;

struct Rzecz {
    int waga;
    int wartosc;
    int id;
    double oplacalnosc;
};

struct WynikAlgorytmu {
    int uzytecznosc;
    double czas_ns;
};

bool porownajOplacalnosc(const Rzecz& a, const Rzecz& b) {
    if (a.oplacalnosc == b.oplacalnosc) {
        return a.waga < b.waga;
    }
    return a.oplacalnosc > b.oplacalnosc;
}

// Funkcja pomocnicza do generowania instancji
vector<Rzecz> wygenerujPrzedmioty(int n, int b, mt19937& gen) {
    uniform_int_distribution<int> waga_dist(1, max(1, b / 2));
    uniform_int_distribution<int> wartosc_dist(1, 20);
    vector<Rzecz> rzeczy(n);
    for (int i = 0; i < n; ++i) {
        rzeczy[i].waga = waga_dist(gen);
        rzeczy[i].wartosc = wartosc_dist(gen);
        rzeczy[i].id = i + 1;
        rzeczy[i].oplacalnosc = (double)rzeczy[i].wartosc / rzeczy[i].waga;
    }
    return rzeczy;
}

// Funkcje statystyczne
double obliczSrednia(const vector<double>& czasy) {
    double suma = 0;
    for (double czas : czasy) suma += czas;
    return suma / czasy.size();
}

double obliczOdchylenie(const vector<double>& czasy, double srednia) {
    double suma_kwadratow = 0;
    for (double czas : czasy) {
        suma_kwadratow += (czas - srednia) * (czas - srednia);
    }
    return sqrt(suma_kwadratow / czasy.size());
}

// ================= ALGORYTMY (Wersje ciche) =================

WynikAlgorytmu algorytmDynamiczny(const vector<Rzecz>& rzeczy, int n, int b) {
    vector<vector<int>> plecak(n + 1, vector<int>(b + 1, 0));
    auto start = high_resolution_clock::now();

    for (int i = 1; i <= n; ++i) {
        for (int j = 0; j <= b; ++j) {
            if (rzeczy[i - 1].waga <= j) {
                int uzytecznosc_bez = plecak[i - 1][j];
                int uzytecznosc_z = plecak[i - 1][j - rzeczy[i - 1].waga] + rzeczy[i - 1].wartosc;
                plecak[i][j] = max(uzytecznosc_bez, uzytecznosc_z);
            } else {
                plecak[i][j] = plecak[i - 1][j];
            }
        }
    }
    auto stop = high_resolution_clock::now();
    return { plecak[n][b], (double)duration_cast<nanoseconds>(stop - start).count() };
}

WynikAlgorytmu algorytmZachlanny(vector<Rzecz> rzeczy, int n, int b) {
    auto start = high_resolution_clock::now();
    sort(rzeczy.begin(), rzeczy.end(), porownajOplacalnosc);

    int zajete_miejsce = 0;
    int laczna_uzytecznosc = 0;

    for (int i = 0; i < n; ++i) {
        if (zajete_miejsce + rzeczy[i].waga <= b) {
            zajete_miejsce += rzeczy[i].waga;
            laczna_uzytecznosc += rzeczy[i].wartosc;
        }
    }
    auto stop = high_resolution_clock::now();
    return { laczna_uzytecznosc, (double)duration_cast<nanoseconds>(stop - start).count() };
}

WynikAlgorytmu algorytmSilowy(const vector<Rzecz>& rzeczy, int n, int b) {
    auto start = high_resolution_clock::now();
    long long wszystkie_kombinacje = 1LL << n;
    int max_uzytecznosc = 0;

    for (long long i = 0; i < wszystkie_kombinacje; i++) {
        int aktualna_waga = 0;
        int aktualna_uzytecznosc = 0;

        for (int j = 0; j < n; j++) {
            if (i & (1LL << j)) {
                aktualna_waga += rzeczy[j].waga;
                aktualna_uzytecznosc += rzeczy[j].wartosc;
            }
        }
        if (aktualna_waga <= b && aktualna_uzytecznosc > max_uzytecznosc) {
            max_uzytecznosc = aktualna_uzytecznosc;
        }
    }
    auto stop = high_resolution_clock::now();
    return { max_uzytecznosc, (double)duration_cast<nanoseconds>(stop - start).count() };
}

// ================= EKSPERYMENTY =================

void Eksperyment1() {
    cout << "=== EKSPERYMENT 1: Wplyw liczby przedmiotow (n) na czas obliczen ===\n";
    cout << "Stala pojemnosc b = 100. Liczba powtorzen dla kazdego n = 10.\n\n";
    cout << setw(5) << "n" 
         << setw(15) << "AD Sredni[ns]" << setw(15) << "AD Odch.[ns]"
         << setw(15) << "AZ Sredni[ns]" << setw(15) << "AZ Odch.[ns]"
         << setw(15) << "AB Sredni[ns]" << setw(15) << "AB Odch.[ns]" << "\n";
    cout << string(95, '-') << "\n";

    mt19937 gen(12345); // Stałe ziarno dla powtarzalności wyników
    int b = 100;

    // 15 wartości rosnących liniowo (od 10 do 24, alg. siłowy jest bezpieczny do n=24)
    for (int n = 10; n <= 24; ++n) {
        vector<double> czasy_AD, czasy_AZ, czasy_AB;

        for (int rep = 0; rep < 10; ++rep) {
            vector<Rzecz> rzeczy = wygenerujPrzedmioty(n, b, gen);
            czasy_AD.push_back(algorytmDynamiczny(rzeczy, n, b).czas_ns);
            czasy_AZ.push_back(algorytmZachlanny(rzeczy, n, b).czas_ns);
            czasy_AB.push_back(algorytmSilowy(rzeczy, n, b).czas_ns);
        }

        double sr_AD = obliczSrednia(czasy_AD), odch_AD = obliczOdchylenie(czasy_AD, sr_AD);
        double sr_AZ = obliczSrednia(czasy_AZ), odch_AZ = obliczOdchylenie(czasy_AZ, sr_AZ);
        double sr_AB = obliczSrednia(czasy_AB), odch_AB = obliczOdchylenie(czasy_AB, sr_AB);

        cout << setw(5) << n 
             << setw(15) << fixed << setprecision(0) << sr_AD << setw(15) << odch_AD
             << setw(15) << sr_AZ << setw(15) << odch_AZ
             << setw(15) << sr_AB << setw(15) << odch_AB << "\n";
    }
    cout << "\n";
}

void Eksperyment2() {
    cout << "=== EKSPERYMENT 2: Wplyw pojemnosci bagaznika (b) na czas obliczen ===\n";
    cout << "Stala liczba przedmiotow n = 20. Liczba powtorzen dla kazdego b = 10.\n\n";
    cout << setw(5) << "b" 
         << setw(15) << "AD Sredni[ns]" << setw(15) << "AD Odch.[ns]"
         << setw(15) << "AZ Sredni[ns]" << setw(15) << "AZ Odch.[ns]"
         << setw(15) << "AB Sredni[ns]" << setw(15) << "AB Odch.[ns]" << "\n";
    cout << string(95, '-') << "\n";

    mt19937 gen(54321);
    int n = 20;

    // 15 wartości b rosnących liniowo
    for (int b = 20; b <= 300; b += 20) {
        vector<double> czasy_AD, czasy_AZ, czasy_AB;

        for (int rep = 0; rep < 10; ++rep) {
            vector<Rzecz> rzeczy = wygenerujPrzedmioty(n, b, gen);
            czasy_AD.push_back(algorytmDynamiczny(rzeczy, n, b).czas_ns);
            czasy_AZ.push_back(algorytmZachlanny(rzeczy, n, b).czas_ns);
            czasy_AB.push_back(algorytmSilowy(rzeczy, n, b).czas_ns);
        }

        double sr_AD = obliczSrednia(czasy_AD), odch_AD = obliczOdchylenie(czasy_AD, sr_AD);
        double sr_AZ = obliczSrednia(czasy_AZ), odch_AZ = obliczOdchylenie(czasy_AZ, sr_AZ);
        double sr_AB = obliczSrednia(czasy_AB), odch_AB = obliczOdchylenie(czasy_AB, sr_AB);

        cout << setw(5) << b 
             << setw(15) << fixed << setprecision(0) << sr_AD << setw(15) << odch_AD
             << setw(15) << sr_AZ << setw(15) << odch_AZ
             << setw(15) << sr_AB << setw(15) << odch_AB << "\n";
    }
    cout << "\n";
}

void Eksperyment3() {
    cout << "=== EKSPERYMENT 3: Jakosc algorytmu zachlannego (AZ) wzgledem optimum (AD) ===\n";
    int liczba_instancji = 1000;
    cout << "Liczba wygenerowanych losowych instancji: " << liczba_instancji << " (losowe n z [20, 100], b z [50, 200]).\n";

    mt19937 gen(99999);
    uniform_int_distribution<int> n_dist(20, 100);
    uniform_int_distribution<int> b_dist(50, 200);

    int liczba_optymalnych = 0;
    double suma_jakosci = 0.0;
    int max_odchylenie = 0;

    for (int i = 0; i < liczba_instancji; ++i) {
        int n = n_dist(gen);
        int b = b_dist(gen);
        vector<Rzecz> rzeczy = wygenerujPrzedmioty(n, b, gen);

        int optymalny = algorytmDynamiczny(rzeczy, n, b).uzytecznosc;
        int zachlanny = algorytmZachlanny(rzeczy, n, b).uzytecznosc;

        if (zachlanny == optymalny) {
            liczba_optymalnych++;
        }
        
        // Zabezpieczenie przed dzieleniem przez zero
        if (optymalny > 0) {
            suma_jakosci += (double)zachlanny / optymalny;
        } else {
            suma_jakosci += 1.0; 
        }

        int odchylenie = optymalny - zachlanny;
        if (odchylenie > max_odchylenie) {
            max_odchylenie = odchylenie;
        }
    }

    double procent_optymalnych = ((double)liczba_optymalnych / liczba_instancji) * 100.0;
    double srednia_jakosc = (suma_jakosci / liczba_instancji) * 100.0;

    cout << "1. Procent przypadkow ze znalezionym optimum: " << fixed << setprecision(2) << procent_optymalnych << "%\n";
    cout << "2. Srednia jakosc rozwiazania AZ (wzgl. optimum): " << srednia_jakosc << "%\n";
    cout << "3. Najwieksze zaobserwowane odchylenie bezwzgledne: " << max_odchylenie << " jednostek uzytecznosci.\n";
    cout << string(95, '=') << "\n";
}

int main() {
    // Odpalenie eksperymentów po kolei
    Eksperyment1();
    Eksperyment2();
    Eksperyment3();

    return 0;
}
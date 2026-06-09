#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <utility>
#include <chrono>
#include <iomanip>
#include <random>
#include <string>

using namespace std;
using namespace std::chrono;

struct Rzecz {
    int waga;
    int wartosc;
    int id;
    double oplacalnosc;
};

bool porownajOplacalnosc(const Rzecz& a, const Rzecz& b) {
    if (a.oplacalnosc == b.oplacalnosc) {
        return a.waga < b.waga; // Jeśli opłacalność równa, wybieramy lżejszy
    }
    return a.oplacalnosc > b.oplacalnosc;
}

int algorytmDynamiczny(const vector<Rzecz>& rzeczy, int n, int b, bool trybDemo) {
    vector<vector<int>> plecak(n + 1, vector<int>(b + 1, 0));
    vector<pair<int, int>> sygnatury;

    auto start = high_resolution_clock::now();

    for (int i = 1; i <= n; ++i) { //sprawdzenie uztecznosci z i bez przedmiotu
        for (int j = 0; j <= b; ++j) {
            if (rzeczy[i - 1].waga <= j) {
                int uzytecznosc_bez = plecak[i - 1][j];
                int uzytecznosc_z = plecak[i - 1][j - rzeczy[i - 1].waga] + rzeczy[i - 1].wartosc;

                if (uzytecznosc_z > uzytecznosc_bez) {
                    plecak[i][j] = uzytecznosc_z;
                } 
                else if (uzytecznosc_bez > uzytecznosc_z) {
                    plecak[i][j] = uzytecznosc_bez;
                } 
                else {
                    plecak[i][j] = uzytecznosc_z; 
                    sygnatury.push_back({i, j});
                }
            } else {
                plecak[i][j] = plecak[i - 1][j];
            }
        }
    }

    auto stop = high_resolution_clock::now();
    auto czasTrwania = duration_cast<nanoseconds>(stop - start);

    int uzytecznosc = plecak[n][b];
    int pozostale_miejsce = b;
    vector<int> wybraneID;
    int zajete_miejsce = 0;

    for (int i = n; i > 0 && uzytecznosc > 0; --i) {
        if (uzytecznosc != plecak[i - 1][pozostale_miejsce]) {
            wybraneID.push_back(rzeczy[i - 1].id);
            uzytecznosc -= rzeczy[i - 1].wartosc;
            pozostale_miejsce -= rzeczy[i - 1].waga;
            zajete_miejsce += rzeczy[i - 1].waga;
        }
    }
    sort(wybraneID.begin(), wybraneID.end());

    cout << endl;
    cout << "algorytm programowania dynamicznego";
    cout << endl;
    cout << "Czas obliczen: " << czasTrwania.count() << " nanosekund\n";
    cout << "laczna uzytecznosc/wartosc: " << plecak[n][b] << "\n";
    cout << "Sumaryczny rozmiar zestawu: " << zajete_miejsce << " / " << b << "\n";
    cout << "Identyfikatory elementow: ";
    if (wybraneID.empty()) cout << "Brak (nic sie nie zmiescilo)";
    else {
        for (int id : wybraneID) cout << id << " ";
    }
    cout << "\n";

    if (trybDemo) {
        cout << "tabela (i - przedmiot, w - waga)" << endl;
        cout << setw(5) << "i\\w |"; 
        for (int j = 0; j <= b; ++j) {
            cout << setw(4) << j << " ";
        }
        cout << "\n";
        cout << string(6 + (b + 1) * 5, '-') << "\n";
        for (int i = 0; i <= n; ++i) {
            cout << setw(3) << i << " |"; 
            
            for (int j = 0; j <= b; ++j) {
                cout << setw(4) << plecak[i][j] << " ";
            }
            cout << "\n";
        }
        cout << "\nsygnatury:\n";
        if (sygnatury.empty()) cout << "Brak sygnatur\n";
        else {
            for (const auto& sig : sygnatury) {
                cout << sig.first << "-" << sig.second << " " << endl;
            }
            cout << "\n";
        }
    }

    return plecak[n][b];
}

int algorytmZachlanny(vector<Rzecz> rzeczy, int n, int b, bool trybDemo) {
    auto start = high_resolution_clock::now();

    // Sortowanie według współczynnika opłacalności
    sort(rzeczy.begin(), rzeczy.end(), porownajOplacalnosc);

    int zajete_miejsce = 0;
    int laczna_uzytecznosc = 0;
    vector<int> wybraneID;
    vector<string> logiDecyzji;

    for (int i = 0; i < n; ++i) {
        string log = "Przedmiot ID: " + to_string(rzeczy[i].id) + 
                     " (Rozmiar: " + to_string(rzeczy[i].waga) + 
                     ", Uzytecznosc: " + to_string(rzeczy[i].wartosc) + 
                     ", Oplacalnosc: " + string(1, ' ') + to_string(rzeczy[i].oplacalnosc).substr(0,5) + ") -> ";
        
        if (zajete_miejsce + rzeczy[i].waga <= b) { //jesli doloze ten przedmiot to sie zmmiesci? jesli nie to do nastepnego, moze bedzie lzejszy
            zajete_miejsce += rzeczy[i].waga;
            laczna_uzytecznosc += rzeczy[i].wartosc;
            wybraneID.push_back(rzeczy[i].id);
            log += "ZABRANY";
        } else {
            log += "ODRZUCONY (brak miejsca)";
        }
        logiDecyzji.push_back(log);
    }

    auto stop = high_resolution_clock::now();
    auto czasTrwania = duration_cast<nanoseconds>(stop - start);

    sort(wybraneID.begin(), wybraneID.end());

    cout << endl;
    cout << "algorytm zachlanny" << endl;
    cout << endl;
    cout << "Czas obliczen: " << czasTrwania.count() << " nanosekund\n";
    cout << "laczna uzytecznosc/wartosc: " << laczna_uzytecznosc << "\n";
    cout << "Sumaryczny rozmiar zestawu: " << zajete_miejsce << " / " << b << "\n";
    cout << "Identyfikatory elementow: ";
    if (wybraneID.empty()) cout << "Brak";
    else {
        for (int id : wybraneID) cout << id << " ";
    }
    cout << "\n";

    if (trybDemo) {
        cout << "kolejne decyzje algorytmu: " << endl;
        for (const string& log : logiDecyzji) {
            cout << log << "\n";
        }
    }

    return laczna_uzytecznosc;
}

int algorytmSilowy(const vector<Rzecz>& rzeczy, int n, int b, bool trybDemo) {
    auto start = high_resolution_clock::now();

     // 1 << n to szybki sposób zapisania 2 do potęgi n (przesunięcie bitowe)
    long long wszystkie_kombinacje = 1LL << n; //1LL to 1 ale long long aby uniknac przepelnienia stosu kiedy przesuwa sie 1 w lewo zeby uzyskac potege
    int max_uzytecznosc = 0;
    int suma_rozmiaru = 0;
    long long liczba_dopuszczalnych = 0;
    long long liczba_optymalnych = 0;
    vector<int> najlepsze_przedmioty;

    // kazda mozliwa kombinacja od 0 do (2^n - 1)
    for (long long i = 0; i < wszystkie_kombinacje; i++) {
        int aktualna_waga = 0;
        int aktualna_uzytecznosc = 0;

        // sprawdzamy ktore bity są zapalone (ktore przedmioty biore w tej kombinacji)
        for (int j = 0; j < n; j++) {
            if (i & (1LL << j)) { // jesli jty bit liczby i to 1
                aktualna_waga += rzeczy[j].waga;
                aktualna_uzytecznosc += rzeczy[j].wartosc;
            }
        }
        // czy w ogole sie zmiescilismy
        if (aktualna_waga <= b) {
            liczba_dopuszczalnych++;

            if (aktualna_uzytecznosc > max_uzytecznosc) {
                max_uzytecznosc = aktualna_uzytecznosc;
                suma_rozmiaru = aktualna_waga;
                liczba_optymalnych = 1; //nowy rekord wiec reset
                
                najlepsze_przedmioty.clear(); 
                for (int j = 0; j < n; j++) {
                    if (i & (1LL << j)) {
                        najlepsze_przedmioty.push_back(rzeczy[j].id); //zapisuje nr przedmiotow
                    }
                }
            } 
            // jesli remis bo zestaw o takim samym wyniku
            else if (aktualna_uzytecznosc == max_uzytecznosc && max_uzytecznosc > 0) {
                liczba_optymalnych++;
            }
        }
    }

    auto stop = high_resolution_clock::now();
    auto czas_obliczen = duration_cast<nanoseconds>(stop - start).count();

    cout << endl;
    cout << "algorytm silowy" << endl;
    cout << endl;
    cout << "Czas obliczen: " << czas_obliczen << " nanosekund\n";
    cout << "laczna uzytycznosc/wartosc: " << max_uzytecznosc << "\n";
    cout << "Sumaryczny rozmiar zestawu: " << suma_rozmiaru << " / " << b << "\n";
    cout << "Identyfikatory elementow: ";
    if (najlepsze_przedmioty.empty()) cout << "Brak";
    else {
        sort(najlepsze_przedmioty.begin(), najlepsze_przedmioty.end());
        for (int id : najlepsze_przedmioty) cout << id << " ";
    }
    cout << "\n";

    if (trybDemo) {
        cout << "statystyki";
        cout << "- Liczba wszystkich sprawdzonych konfiguracji: " << wszystkie_kombinacje << "\n";
        cout << "- Liczba rozwiazan dopuszczalnych (mieszczacych sie w bagazniku): " << liczba_dopuszczalnych << "\n";
        cout << "- Liczba rozwiazan optymalnych (osiagajacych maksimum): " << liczba_optymalnych << "\n";
    }

    return max_uzytecznosc;
}

int main() {
    int n = 0, b = 0;
    vector<Rzecz> rzeczy;

    cout << "Wybierz tryb wprowadzania danych:\n";
    cout << "1. Wczytaj dane z pliku tekstowego\n";
    cout << "2. Wygeneruj dane losowo\n";
    cout << "Wybor: ";
    int zrodlo;
    if (!(cin >> zrodlo) || (zrodlo != 1 && zrodlo != 2)) {
        cout << "Blad: Niepoprawny wybor. Konczenie programu.\n";
        return 1;
    }

    if (zrodlo == 1) {
        string nazwa_pliku;
        cout << "Podaj nazwe pliku (np. plik.txt): ";
        cin >> nazwa_pliku;
        
        ifstream plik(nazwa_pliku);
        if (!plik.is_open()) {
            cout << "Blad: Nie mozna otworzyc pliku: " << nazwa_pliku << "\n";
            return 1;
        }

        if (!(plik >> n >> b)) {
            cout << "Blad: Plik jest pusty lub ma zly format naglowka!\n";
            return 1;
        }

        if (n <= 0 || b <= 0) {
            cout << "Blad: Liczba elementow (n) oraz pojemnosc (b) musza byc > 0!\n";
            return 1;
        }

        rzeczy.resize(n);
        for (int i = 0; i < n; ++i) {
            if (!(plik >> rzeczy[i].waga >> rzeczy[i].wartosc)) {
                cout << "Blad: Uszkodzone dane przy przedmiocie nr " << i + 1 << "!\n";
                return 1;
            }
            if (rzeczy[i].waga <= 0 || rzeczy[i].wartosc <= 0) {
                cout << "Blad: Parametry przedmiotu nr " << i + 1 << " musza byc dodatnie!\n";
                return 1;
            }
            rzeczy[i].id = i + 1;
            rzeczy[i].oplacalnosc = (double)rzeczy[i].wartosc / rzeczy[i].waga;
        }
        plik.close();
        cout << "Poprawnie wczytano " << n << " elementow z pliku.\n";
    } 
    else {
        cout << "Podaj liczbe elementow sprzetu (n): ";
        if (!(cin >> n) || n <= 0) {
            cout << "Blad: Liczba elementow musi byc liczba dodatnia!\n";
            return 1;
        }
        cout << "Podaj pojemnosc bagaznika (b): ";
        if (!(cin >> b) || b <= 0) {
            cout << "Blad: Pojemnosc musi byc liczba dodatnia!\n";
            return 1;
        }

        mt19937 gen(random_device{}());
        uniform_int_distribution<int> waga_dist(1, max(1, b / 2)); 
        uniform_int_distribution<int> wartosc_dist(1, 20);

        rzeczy.resize(n);
        cout << "\nWygenerowano nastepujace elementy (ID: Rozmiar | Uzytecznosc):\n";
        for (int i = 0; i < n; ++i) {
            rzeczy[i].waga = waga_dist(gen);
            rzeczy[i].wartosc = wartosc_dist(gen);
            rzeczy[i].id = i + 1;
            rzeczy[i].oplacalnosc = (double)rzeczy[i].wartosc / rzeczy[i].waga;
            cout << "Przedmiot " << rzeczy[i].id << ": Rozmiar = " << rzeczy[i].waga 
                 << ", Uzytecznosc = " << rzeczy[i].wartosc << "\n";
        }
    }

    bool trybDemo = (n <= 15);
    if (trybDemo) {
        cout << "tryb demo" << endl;
    } else {
        cout << "tryb standardowy" << endl;
    }

    int optymalnyWynik = algorytmDynamiczny(rzeczy, n, b, trybDemo);
    int zachlannyWynik = algorytmZachlanny(rzeczy, n, b, trybDemo);
    
    if (n <= 22) {
        algorytmSilowy(rzeczy, n, b, trybDemo);
    } else {
        cout << "\n[INFO] Pominieto Algorytm Silowy (AB) - rozmiar instancji (n > 22) wydluzylby krytycznie czas pracy.\n";
    }

    cout << endl;
    cout << "podsumowanie analizy optymalnosci";
    cout << endl;
    if (zachlannyWynik == optymalnyWynik) {
        cout << "Algorytm zachlanny (AZ) znalazl rozwiazanie OPTYMALNE.\n";
    } else {
        cout << "Algorytm zachlanny (AZ) znalazl rozwiazanie SUBOPTYMALNE.\n";
        cout << "Blad bezwzgledny (Roznica do optimum): " << (optymalnyWynik - zachlannyWynik) << " pkt uzytecznosci.\n";
    }
    cout << endl;

    return 0;
}
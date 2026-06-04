#include<iostream>
#include<vector>
#include<fstream>
#include<cstdlib>
#include<ctime>
#include<chrono>
using namespace std;
void algorytmSilowy(int n, int b, const vector<int>& w, const vector<int>& p) {

    auto start = chrono::high_resolution_clock::now();

    // 1 << n to szybki sposób zapisania 2 do potęgi n (przesunięcie bitowe)
    long long wszystkie_kombinacje = 1LL << n;  //1LL to 1 ale long long aby uniknac przepelnienia stosu kiedy przesuwa sie 1 w lewo zeby
                                                //uzyskac potege
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
                aktualna_waga += w[j];
                aktualna_uzytecznosc += p[j];
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
                        najlepsze_przedmioty.push_back(j + 1); //zapisuje nr przedmiotow
                    }
                }
            } 
            // jesli remis bo zestaw o takim samym wyniku
            else if (aktualna_uzytecznosc == max_uzytecznosc) {
                liczba_optymalnych++;
            }
        }
    }

    auto stop = chrono::high_resolution_clock::now();
    auto czas_obliczen = chrono::duration_cast<chrono::microseconds>(stop - start).count();

    cout << "Czas obliczen: " << czas_obliczen << " mikrosekund" << endl;
    cout << "Wartosc uzyskanego rozwiazania (laczna uzytecznosc): " << max_uzytecznosc << endl;
    cout << "Sumaryczny rozmiar wybranego zestawu: " << suma_rozmiaru << endl;
    
    cout << "Identyfikatory elementow: ";
    if (najlepsze_przedmioty.empty()) {
        cout << "Brak (nic sie nie zmiescilo)";
    } else {
        for (int id : najlepsze_przedmioty) {
            cout << id << " ";
        }
    }
    cout << endl;

    if (n <= 15) {
        cout << "- Liczba wszystkich sprawdzonych konfiguracji: " << wszystkie_kombinacje << endl;
        cout << "- Liczba rozwiazan dopuszczalnych (mieszcza sie w pojemnosci): " << liczba_dopuszczalnych << endl;
        cout << "- Liczba rozwiazan optymalnych (z maksymalna uzytecznoscia): " << liczba_optymalnych << endl;
    } else {
        cout << "\n(Tryb demonstracyjny wylaczony, poniewaz n > 15)" << endl;
    }
}
int main() {
    int n, b;
    vector<int> w, p;

    int zrodlo;
    cout << "=== WYBOR ZRODLA DANYCH ===" << endl;
    cout << "1. Wczytaj dane z pliku (plik.txt)" << endl;
    cout << "2. Wygeneruj dane losowo" << endl;
    cout << "Wybor: ";
    if (!(cin >> zrodlo)) {
        cout << "Nalezy wpisac liczbe!" << endl;
        return 1;
    }

    if (zrodlo == 1) {
        fstream plik;
        plik.open("plik.txt", ios::in);
        if(!plik.good()) {
            cout << "Blad: Nie udalo sie otworzyc pliku!" << endl;
            return 1; 
        }
        
        if (!(plik >> n >> b)) {
            cout << "Blad: Plik jest pusty lub zly format!" << endl;
            return 1;
        }

        if (n <= 0 || b <= 0) {
            cout << "Blad: Ilosc elementow (n) i pojemnosc (b) musza byc wieksze od 0!" << endl;
            return 1;
        }
        
        w.resize(n);
        p.resize(n);
        for (int i = 0; i < n; i++) {
            if (!(plik >> w[i] >> p[i])) {
                cout << "Blad: Brakujace lub uszkodzone dane przy przedmiocie nr " << i + 1 << "!" << endl;
                return 1;
            }
            if (w[i] <= 0 || p[i] <= 0) {
                cout << "Blad: Waga i wartosc przedmiotu nr " << i + 1 << " musza byc dodatnie!" << endl;
                return 1;
            }
        }
        plik.close();
        cout << "Poprawnie wczytano dane z pliku." << endl;
        
    } else if (zrodlo == 2) {
        cout << "\nPodaj liczbe elementow sprzetu (n): ";
        if (!(cin >> n) || n <= 0) {
            cout << "Blad: Liczba elementow musi byc liczba dodatnia!" << endl;
            return 1;
        }
        cout << "Podaj pojemnosc bagaznika (b): ";
        if (!(cin >> b) || b <= 0) {
            cout << "Blad: Pojemnosc musi byc liczba dodatnia!" << endl;
            return 1;
        }
        
        w.resize(n);
        p.resize(n);
        
        srand(time(0)); 
        
        cout << "Wygenerowano nastepujace przedmioty:" << endl;
        for (int i = 0; i < n; i++) {
            //waga
            w[i] = (rand() % b) + 1; 
            //wartosc
            p[i] = (rand() % 20) + 1; 
            
            cout << "Przedmiot " << i + 1 << " - waga: " << w[i] << ", uzytecznosc: " << p[i] << endl;
        }
    } else {
        cout << "Niepoprawny wybor!" << endl;
        return 1;
    }

    int wybor;
    cout << "Wybierz algorytm:\n1. Dynamiczny (AD)\n2. Silowy (AB)\n3. Zachlanny (AZ)\nWybor: ";
    cout<<endl;
    if (!(cin >> wybor)) {
        cout << "Nalezy podac liczbe!" << endl;
        return 1;
    }

    algorytmSilowy(n, b, w, p);
    return 0;
}


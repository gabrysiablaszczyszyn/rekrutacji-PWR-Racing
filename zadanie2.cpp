#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <utility>

using namespace std;

// Struktura przechowująca pojedynczy pomiar z czujnika
struct Punkt {
    float czas;
    float x;
    float y;
};

// wczytuje dane z plików CSV
// Otwiera plik, pomija pierwszy wiersz (nagłówek)
vector<Punkt> wczytajCSV(const string& nazwa_pliku) {
    vector<Punkt> dane;
    ifstream plik(nazwa_pliku);
    
    if (!plik.is_open()) {
        cout << "Nie udalo sie otworzyc pliku!" << "\n";
        return dane;
    }

    string ignorowana_linia;
    // Pominięcie nagłówka (timestamp,x,y)
    getline(plik, ignorowana_linia);

    Punkt p;
    char przecinek;

    // Odczyt danych z pominięciem znaków przecinka
    while (plik >> p.czas >> przecinek >> p.x >> przecinek >> p.y) {
        dane.push_back(p);
    }

    plik.close();
    return dane;
}
// przeprowadza fuzję danych z obu czujników
// Wykorzystuje koncepcję filtru komplementarnego. 
// Gęste próbki z Sensora 1 zapewniają ciągłość ruchu
// a rzadsze próbki z Sensora 2 korygują powstawanie dryfu
vector<Punkt> poprawna_trasa(const vector<Punkt>& sensor1, const vector<Punkt>& sensor2){
    vector<Punkt> zoptymalizowana_trasa;

    // Parametry filtru zostały dostosowane do niwelowania dużego dryfu Sensora 1
    float alpha = 0.15;
    float prog_bledu = 6;
    
    Punkt nowy_punkt;
    float przewidywany_x = sensor1[0].x;
    float przewidywany_y = sensor1[0].y;
    zoptymalizowana_trasa.push_back(sensor1[0]); 
    size_t index_s2 = 0;
    float d;

    // Iteracja po wszystkich gęstych odczytach pierwszego sensora
    for (size_t i = 1; i < sensor1.size(); ++i) {
        przewidywany_x = przewidywany_x + sensor1[i].x - sensor1[i - 1].x;
        przewidywany_y = przewidywany_y + sensor1[i].y - sensor1[i - 1].y;

        // Jeśli dostępny jest odczyt z drugiego sensora w danym czasie - aplikujemy korektę
        if (index_s2 < sensor2.size() && sensor1[i].czas >= sensor2[index_s2].czas) {
            d = sqrt((przewidywany_x - sensor2[index_s2].x) * (przewidywany_x - sensor2[index_s2].x) + 
                     (przewidywany_y - sensor2[index_s2].y) * (przewidywany_y - sensor2[index_s2].y));

            // Jeśli różnica mieści się w akceptowalnym limicie dryfu, uśredniamy wyniki wagowo
            if (d <= prog_bledu) {
                przewidywany_x = alpha * przewidywany_x + (1 - alpha) * sensor2[index_s2].x;
                przewidywany_y = alpha * przewidywany_y + (1 - alpha) * sensor2[index_s2].y;
            }
            
            index_s2 = index_s2 + 1;
        }
        // Zapis zoptymalizowanego punktu do ostatecznej trasy
        nowy_punkt.czas = sensor1[i].czas;
        nowy_punkt.x = przewidywany_x;
        nowy_punkt.y = przewidywany_y;
        zoptymalizowana_trasa.push_back(nowy_punkt);
    }
    return zoptymalizowana_trasa;
}

//zwracająca pozycję dla zadanego czasu T
//Sprawdza krawędzie (edge cases), a następnie używa wyszukiwania
// binarnego do szybkiego znalezienia przedziału czasowego.
pair <float, float> oblicz_pozycje(float T, const vector<Punkt>& trasa){
    //wartość spoza wymaganego zakresu (rekurencyjne ponowienie)
    if (T < 0 || T > 33){
        cout << " << Wartosc spoza zakresu!" << "\n";
        cout << " << Podaj T " << "\n";
        cout << " >> ";
        cin >> T;
        return oblicz_pozycje(T, trasa);
    }
    
    int lewy = 0;
    int prawy = trasa.size() - 1;

    // Algorytm wyszukiwania binarnego
    while (lewy <= prawy) {
        int srodek = lewy + (prawy - lewy) / 2;

        if (trasa[srodek].czas == T) {
            return {trasa[srodek].x, trasa[srodek].y};
        }
            
        if (trasa[srodek].czas < T) {
            lewy = srodek + 1; // Szukamy w prawej połowie
        } else {
            prawy = srodek - 1; // Szukamy w lewej połowie
        }
    }
    // Zabezpieczenie przed wyjściem poza tablicę na krawędziach
    if (prawy < 0) return {trasa.front().x, trasa.front().y};
    if (lewy >= trasa.size()) return {trasa.back().x, trasa.back().y};

    float procent = (T - trasa[prawy].czas ) / (trasa[lewy].czas - trasa[prawy].czas);
    return {trasa[prawy].x + procent * (trasa[lewy].x - trasa[prawy].x), trasa[prawy].y + procent * (trasa[lewy].y - trasa[prawy].y)};
}

int main() {
    vector<Punkt> sensor1 = wczytajCSV("sensor1.csv");
    vector<Punkt> sensor2 = wczytajCSV("sensor2.csv");

    vector<Punkt> zoptymalizowana_trasa = poprawna_trasa(sensor1, sensor2);

    float T;
    cout << " << Podaj T " << "\n";
    cout << " >> ";
    cin >> T;
    auto polozenie = oblicz_pozycje(T, zoptymalizowana_trasa);
    cout << "<< " << polozenie.first << " ";
    cout << polozenie.second;

    return 0;
}
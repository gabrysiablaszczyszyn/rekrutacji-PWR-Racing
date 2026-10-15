#include <iostream>
#include <fstream>
#include <vector> 
#include <cmath>
#include <utility>

using namespace std;

// Struktura reprezentująca pojedynczy pachołek.
struct Punkt {
    int id;
    float x;
    float y;
    bool usuniety = false; // jeśli true, punkt to duplikat i go pomijamy
};


// Oblicza i zwraca usredniona pozycje pachołka na podstawie sumy wspolrzednych.
//Modyfikuje przekazane przez referencje nastepne_id.
Punkt usredniony_punkt(float suma_x, float suma_y, int licznik_liczb, int& nastepne_id){
    Punkt nowy_punkt;
    nowy_punkt.id = nastepne_id;
    nastepne_id = nastepne_id + 1;
    nowy_punkt.x = suma_x / licznik_liczb;
    nowy_punkt.y = suma_y / licznik_liczb;
    return nowy_punkt;
}

// Wypisuje ID pachołków z wektora w formacie [1,2,3].
// Zabezpieczona przed edge case'em (pusty wektor).
void wypisz_tabele(const vector<Punkt>& punkty){
    if(!punkty.empty()){
        cout << "[";
        for (int i = 0; i < punkty.size() - 1; i++){
            cout << punkty[i].id << ",";
        }
        cout << punkty[punkty.size() - 1].id << "]" << "\n";
    }
}


//Przeprowadza deduplikacje, analizuje zbiór i grupuje punkty znajdujące się 
//względem siebie w promieniu mniejszym niż zdefiniowana min_odleglosc.
vector<Punkt> poprawione_punkty(int liczba_punktow, float min_odleglosc, vector<Punkt>& punkty) {
    vector<Punkt> usuniete_punkty;
    vector<Punkt> nowe_punkty;

    int licznik_liczb;
    float suma_x, suma_y;
    int nastepne_id = punkty.back().id + 1;
    Punkt nowy_punkt;

    // Główna pętla algorytmu. Przechodzi przez każdy punkt bazowy na mapie, 
    // ignorując te, które zostały już oznaczone jako usunięte w poprzednich krokach
    for (int i = 0; i < liczba_punktow - 1; ++i){
        usuniete_punkty.clear();
        if (!punkty[i].usuniety){
            suma_x = punkty[i].x;
            suma_y = punkty[i].y;
            licznik_liczb = 1;
            
            // Pętla wewnętrzna przeszukuje wyłącznie resztę trasy (od i+1 do końca).
            // Szuka sąsiadów dla aktualnego punktu bazowego
            for (int j = i + 1; j < liczba_punktow; ++j){
                if (!punkty[j].usuniety){
                    if (pow(punkty[i].x  - punkty[j].x,2) + pow(punkty[i].y - punkty[j].y,2) < pow(min_odleglosc,2)){
                        if (licznik_liczb == 1){
                            usuniete_punkty.push_back(punkty[i]);
                        }
                        usuniete_punkty.push_back(punkty[j]);
                        // Dodaje współrzędne znalezionego duplikatu do sumy uśredniającej 
                        // i oznacza go jako usunięty, by algorytm pominął go w przyszłości
                        suma_x = suma_x + punkty[j].x;
                        suma_y = suma_y + punkty[j].y;
                        licznik_liczb = licznik_liczb + 1;
                        punkty[j].usuniety = true;

                    }
                }
            }   
            //jesli aktualny punkt bazowy ma duplikaty 
            // uśredniamy ich pozycję w jeden nowy, centralny punkt
            if (licznik_liczb > 1){
                nowy_punkt = usredniony_punkt(suma_x, suma_y, licznik_liczb, nastepne_id);
                nowe_punkty.push_back(nowy_punkt);
                // Oznaczamy oryginalny punkt bazowy jako usunięty
                punkty[i].usuniety = true;
                cout << "CONE" << " "  << nowy_punkt.id << " ";
                cout << nowy_punkt.x << " " << nowy_punkt.y << " ";
                wypisz_tabele(usuniete_punkty);
            }
            // Jeśli punkt bazowy nie miał żadnych sąsiadów w okolicy
            // po prostu przepisujemy go do nowej, poprawionej mapy
            else {
                nowe_punkty.push_back(punkty[i]);
            }
        }
    }
// Sprawdzenie ostatniego pachołka w zbiorze danych
// Ponieważ główna pętla algorytmu zatrzymuje się na przedostatnim elemencie (i < liczba_punktow - 1), 
// musimy ręcznie zweryfikować stan ostatniego punktu i dodać go, jeśli nie był duplikatem
    if (!punkty[liczba_punktow - 1].usuniety){
        nowe_punkty.push_back(punkty[liczba_punktow - 1]);
    }

    return nowe_punkty;
}

// Wzgledem aktualnego rozwazanego odcinka [skrajny_lewy, skrajny_prawy]
// rozdziela pozostale punkty na te ktore znajduja sie po lewej i po prawej stronie.
// Wykorzystuje wzor na wyznacznik wektorow.
pair <vector<Punkt>, vector<Punkt>> podzielPunkty(Punkt skrajny_lewy, Punkt skrajny_prawy, const vector<Punkt>& tabela) {
    vector<Punkt> lewa_strona;
    vector<Punkt> prawa_strona;
    float D;
    for (int i = 0; i < tabela.size(); i++){
        // Obliczenie wyznacznika (D) dla kazdego punktu
        D = (skrajny_prawy.x - skrajny_lewy.x) * (tabela[i].y - skrajny_lewy.y) - (skrajny_prawy.y - skrajny_lewy.y) * (tabela[i].x - skrajny_lewy.x);
        if (D > 0) {
            lewa_strona.push_back(tabela[i]);
        } 
        else if (D < 0) {
            prawa_strona.push_back(tabela[i]);
        }
}
    return {lewa_strona, prawa_strona};
}

void QuickHull(Punkt skrajny_lewy, Punkt skrajny_prawy, const vector<Punkt>& tabela, vector<Punkt>& otoczka){
    if (!tabela.empty()) {
        float d, max_odleglosc = -1;
        Punkt najdalej_oddalony;

        for (int i = 0; i < tabela.size(); i++ ){
            d = abs((skrajny_prawy.x - skrajny_lewy.x) * (tabela[i].y - skrajny_lewy.y) - 
              (skrajny_prawy.y - skrajny_lewy.y) * (tabela[i].x - skrajny_lewy.x));
            if(d > max_odleglosc){
                max_odleglosc = d;
                najdalej_oddalony = tabela[i];
            }
        }
        auto wynik_podzialu_1 = podzielPunkty(skrajny_lewy, najdalej_oddalony, tabela);
        auto wynik_podzialu_2 = podzielPunkty(najdalej_oddalony, skrajny_prawy, tabela);
        QuickHull(skrajny_lewy, najdalej_oddalony, wynik_podzialu_1.first, otoczka);
        otoczka.push_back(najdalej_oddalony);
        QuickHull(najdalej_oddalony, skrajny_prawy, wynik_podzialu_2.first, otoczka);
    }


}
//wylicza i zwraca najktotsza otoczke wykorzystujac algoryt QuickHull
vector<Punkt> otoczka(const vector<Punkt>& punkty) {
    Punkt skrajny_lewy = punkty[0], skrajny_prawy = punkty[0];
    vector<Punkt> otoczka;

    //Zabezpieczenie przed pustym zbiorem danych
    if (punkty.empty()) {
        return otoczka;
    }
    //znajdujemy skrajne pukty na lewo i prawo
    for (int i = 1; i < punkty.size(); i++){
        if (punkty[i].x < skrajny_lewy.x){
            skrajny_lewy = punkty[i];
        }
        if (punkty[i].x > skrajny_prawy.x){
            skrajny_prawy = punkty[i];
        }
    }
    //sortujemy punkty wzgledem ocinka [skrajny_lewy, skrajny_prawy] na punkty 
    //znajdujace sie po jego lewej i prawej
    auto wynik_podzialu = podzielPunkty(skrajny_lewy, skrajny_prawy, punkty);
    otoczka.push_back(skrajny_lewy);
    // Rekurencyjnie tworzymy gorna czesc otoczk
    QuickHull(skrajny_lewy, skrajny_prawy, wynik_podzialu.first, otoczka);
    otoczka.push_back(skrajny_prawy);
    // Rekurencyjnie tworzymy gorna czesc otoczk
    QuickHull(skrajny_prawy,skrajny_lewy, wynik_podzialu.second, otoczka);

    return otoczka;

}

//oblicza i zwraca obwod wielokata
float obwod(const vector<Punkt>& punkty){
    //Zabezpieczenie przed pustym zbiorem danych
    if (punkty.empty()) {
        return 0;
    }
    float wynik = 0;
    for (int i = 0; i < punkty.size(); i++){
        // Uzywamy modulo (%), aby ostatni punkt polaczyl sie z powrotem z pierwszym, domykajac obwów
        wynik = wynik + sqrt(pow(punkty[i].x - punkty[(i + 1) % punkty.size()].x ,2) + pow(punkty[i].y - punkty[(i + 1) % punkty.size()].y, 2));
    }
    return wynik;
}


int main() {
    ifstream plik("example_input.txt");

    //Zabezpieczenie przed brakiem pliku w folderze lub błędem otwarcia.
    if(!plik.is_open()){
        cout << "Nie udalo sie otworzyc pliku!" << "\n";
        return 1;
    }

    int liczba_punktow;
    float min_odleglosc;
    
    plik >> liczba_punktow;
    plik >> min_odleglosc;
    vector<Punkt> punkty(liczba_punktow);  
    
    for (int i = 0; i < liczba_punktow; ++i){
        plik >> punkty[i].id >> punkty[i].x >> punkty[i].y;
    }
    plik.close();

    // 1. Deduplikacja pachołków
    vector<Punkt> nowe_punkty = poprawione_punkty(liczba_punktow, min_odleglosc, punkty);

    // 2. Wyznaczenie otoczki wypukłej i jej obwodu
    vector<Punkt> tabela = otoczka(nowe_punkty);
    float dlugosc = obwod(tabela);
    cout << "ENCLOSURE " << dlugosc << " ";
    wypisz_tabele(tabela);

    return 0;
}
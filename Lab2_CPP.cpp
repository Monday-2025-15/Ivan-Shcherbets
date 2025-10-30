#include <iostream>    // rozwiazanie poprawne
using namespace std;
class Samochod
{
private:
    string marka;
    string model;
    int rokProdukcji;
public:
    Samochod()
    {
        marka = "Toyota";
        model = "Camry";
        rokProdukcji = 2004;
    }
    Samochod(string marka, string model, int rokProdukcji)
    {
        this->marka=marka;
        this->model=model;
        this->rokProdukcji=rokProdukcji;
    }
    ~Samochod()
    {
        cout << "Destruktor wywolany dla: " << marka << " " << model << endl;
    }
};
class Tablica
{
private:
    int* tablica;   // warto, by i rozmiar tablicy byl atrybutem
public:
    Tablica()
    {
        tablica = new int[10];
    }
    Tablica(int n)
    {
        tablica = new int[n];
    }
    ~Tablica()
    {
        delete[] tablica;
    }
};
int main(int argc, char* argv[])  // "oszczedny" ten main...
{
    Samochod car;
    Tablica tab(5);
    return 0;
};

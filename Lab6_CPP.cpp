#include <iostream>   
#include <vector>
#include <memory>

using namespace std;

class Pojazd{    // szkoda, ze zabraklo destruktorow w klasach pochodnych; rozwiazanie poprawne
	public:
		virtual void przyspiesz()=0;
		virtual void zatrzymaj()=0;
		virtual ~Pojazd()=default;
};
class Samochod : public Pojazd{
	public:
		void przyspiesz() override {cout << "Samochod przyspiesza\n";}
		void zatrzymaj() override {cout << "Samochod zatrzyma\n";}
};
class Rower : public Pojazd{
    public:
		void przyspiesz() override {cout << "Rower przyspiesza\n";}
		void zatrzymaj() override {cout << "Rower zatrzyma\n";}
};
class Motocykl : public Pojazd{
    public:
        void przyspiesz() override {cout << "Motocykl przyspiesza\n";}
        void zatrzymaj() override {cout << "Motocykl zatrzyma\n";}
};
class Skuter : public Pojazd{
    public:
        void przyspiesz() override {cout << "Skuter przyspiesza\n";}
        void zatrzymaj() override {cout << "Skuter zatrzyma\n";}
};

int main()
{
	vector<unique_ptr<Pojazd>> pojazdy;
	pojazdy.push_back(make_unique<Samochod>());
	pojazdy.push_back(make_unique<Rower>());
	pojazdy.push_back(make_unique<Motocykl>());
	pojazdy.push_back(make_unique<Skuter>());
	for(auto& pojazd : pojazdy)
	{
		pojazd->przyspiesz();
		pojazd->zatrzymaj();
	}
	return 0;
}

#include <iostream>  // rozwiazanie w ogolnosci poprawne; prosze uzupelnic o komentarz do zadania nr 6
using namespace std;
//1
class Pojazd1{
	public:
		virtual void przyspiesz(){cout << "Pojazd przyspiesza\n";}
		void zatrzymaj(){cout << "Pojazd zatrzymany\n";}
};
class Samochod1 : public Pojazd1{
	public:
		void przyspiesz() override {cout << "Samochod przyspiesza\n";}
};
class Rower1 : public Pojazd1{
    public:
		void przyspiesz() override {cout << "Rower przyspiesza\n";}
};
//2,3
class Pojazd2{
    public:
	virtual void przyspiesz()=0;
	virtual ~Pojazd2() {}
};
class Samochod2 : public Pojazd2{
public:
	void przyspiesz() override {cout << "Samochod przyspiesza\n";}
};
class Rower2 : public Pojazd2{
public:
	void przyspiesz() override {cout << "Rower przyspiesza\n";}
};
//5,6
class Pojazd3{
	protected:
		string marka;
		string model;
	Pojazd3(string marka, string model, int id) : marka(marka), model(model), id(id) {}
	private:
		int id;
};
class SamochodProtected : protected Pojazd3 {
	public:
		SamochodProtected() : Pojazd3("Ford","Focus",123) {}
		string Info()
		{
			return "Samochod: " + marka + " " + model + "\n";  // tutaj tez id niedostepne
		}
};
class SamochodPrivate : private Pojazd3 {
    public:
		SamochodPrivate() : Pojazd3("BMW","M3",999) {}
        string Info()
        {
            return "Samochod: " + marka + " " + model + "\n"; //id nedostepne
        }
};   // jaka jest zatem roznica miedzy dziedziczeniem protected a private?
//protected zachowuje dostep dla dalszych klas pochodnych a private jego calkowice zamyka
//7
class Silnikowy1 {
protected:
	void przyspiesz(){cout << "Przyspiesz";}
};
class Elektryczny1 {
protected:
	void ladowania(){cout << "ladujemo\n";}
};
class Hybryda1 : public Silnikowy1, public Elektryczny1{
public:
	void print() {przyspiesz(); ladowania();}
};
//9
class Pojazd4 {
public:
	virtual void przyspiesz()=0;
	virtual void zatrzymaj() {cout << "zatrzymano\n";}
};
class Samochod4 : public Pojazd4 {};
class ElektrycznySamochod : public Samochod4 {
private:
	int speed = 0;

public:
	void przyspiesz() override {
		speed++;
		cout << "Elektryczny samochod przyspiesza, predkosc = " << speed << endl;
	}
};   // brak klasy SamochodElektryczny
//10
class Pojazd5{
    public:
       virtual void przyspiesz(){cout << "Pojaz przyspiesza\n";} // te metode nalezaloby zdefiniowac jako czysto wirtualna
};
class Silnikowy2 : public Pojazd5{
public: virtual void przyspiesz(){cout << "Silnikowy przyspiesza\n";}
};
class Elektryczny2 : public Pojazd5{
public: virtual void przyspiesz(){cout << "Elektryczny przyspiesza\n";}
};
class Hybryda2 : public Silnikowy2, public Elektryczny2{
public: virtual void przyspiesz(){cout << "Hybryda przyspiesza\n";}
};
int main()
{
	Samochod1 s1;
	Rower1 r1;
	s1.przyspiesz();
	r1.przyspiesz();
	s1.zatrzymaj();

	Pojazd2* p2 = new Samochod2();
	p2->przyspiesz();
	delete p2;

	p2 = new Rower2();
	p2->przyspiesz();
	delete p2;

	SamochodProtected sp;
	cout << sp.Info();
	SamochodPrivate spv;
	cout << spv.Info();

	Hybryda1 h1;
	h1.print();

	ElektrycznySamochod e4;
	e4.przyspiesz();
	e4.przyspiesz();
	e4.zatrzymaj();

	Hybryda2 h2;
	h2.przyspiesz();

	return 0;
}

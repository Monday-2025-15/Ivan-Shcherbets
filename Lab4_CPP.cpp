#include <iostream>
#include <algorithm>  // w calym programie nie ma ani jednego konstruktora, to nie jest zwyczajowa praktyka
// bardzo prosze uzupelnic rozwiazanie
using namespace std;

class Osoba{
	private:
		string imie;
		string nazwisko;
		int wiek;
		string email;
		string telefon;
	public:
		Osoba(string imie, string nazwisko, int wiek): imie(imie), nazwisko(nazwisko), wiek(wiek){}
		Osoba(string imie, string nazwisko, int wiek, string email, string telefon) : imie(imie), nazwisko(nazwisko), wiek(wiek)
		{
			SetEmail(email);
			SetTelefon(telefon);
		}
		string GetImie() {return imie;}
		string GetNazwisko() {return nazwisko;}
		int GetWiek() {return wiek;}
		string GetEmail() {return email;}
		string GetTelefon() {return telefon;}
		void SetImie(string imie) {this->imie=imie;}
		void SetNazwisko(string nazwisko) {this->nazwisko=nazwisko;}
		void SetWiek(int wiek) {this->wiek = wiek;}
		void SetEmail(string email)
		{
			if(count(email.begin(), email.end(), '@')>0) this->email=email;
			else cout << "Podano niepoprawny email\n";
		}
		void SetTelefon(string telefon)
		{
			if(telefon.size()==9) this->telefon=telefon;
			else cout << "Podano niepoprawny telefon\n";
		}
};
class Pracownik{
	protected:
	string stanowisko;
	float wynagrodzenie;
	public:
	Pracownik(string stanowisko, float wynagrodzenie) :stanowisko(stanowisko), wynagrodzenie(wynagrodzenie){}
	void SetStanowisko(string stanowisko) {this->stanowisko=stanowisko;}
	void SetWynagrodzenie(float wynagrodzenie) {this->wynagrodzenie=wynagrodzenie;}
	virtual void pokazDane()
	{
		cout << "stanowisko = " << stanowisko
			<< "\nwynagrodzenie = " << wynagrodzenie
	}
};
class Nauczyciel : public Pracownik{
	private:
		string przedmiot;
	public:
	Nauczyciel(string stanowisko, float wynagrodzenie, string przedmiot) : Pracownik(stanowisko, wynagrodzenie), przedmiot(przedmiot){}
	void SetPrzedmiot(string przedmiot) {this->przedmiot = przedmiot;}
	void pokazDane()  // frament tej funkcji moglby byc zdefiniowany w nadklasie - nie trzeba by go powielac w podklasach
	{
		Pracownik::pokazDane();
		cout  << "\nprzedmiot = " << przedmiot << endl;
	}
};
class Administracja : public Pracownik{
	private: 
		string adress;
	public:
		Nauczyciel(string stanowisko, float wynagrodzenie, string adress) : Pracownik(stanowisko, wynagrodzenie), adress(adress){}
        void pokazDane()
        {
             Pracownik::pokazDane();
			cout << "\nadress = "<< adress <<  endl;
        }
};
int main()
{
	Osoba him("Alex", "Lebron", 23);
	cout << him.GetImie() << endl;
	cout << him.GetNazwisko() << endl;
	cout << him.GetWiek() << endl;
	him.SetTelefon("y4tutreu8g8fdahdu");
	him.SetEmail("a@g.com");
	cout << him.GetEmail() << endl;
	Nauczyciel jeden("One", 1, "algebra");
	jeden.SetStanowisko("Two");
	jeden.pokazDane();
	Administracja admin("?", 500,"49000");
	admin.SetWynagrodzenie(999);
	admin.pokazDane();
	return 0;
}

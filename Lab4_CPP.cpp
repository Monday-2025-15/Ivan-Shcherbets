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
	void SetStanowisko(string stanowisko) {this->stanowisko=stanowisko;}
	void SetWynagrodzenie(float wynagrodzenie) {this->wynagrodzenie=wynagrodzenie;}
};
class Nauczyciel : public Pracownik{
	private:
		string przedmiot;
	public:
	void SetPrzedmiot(string przedmiot) {this->przedmiot = przedmiot;}
	void pokazDane()  // frament tej funkcji moglby byc zdefiniowany w nadklasie - nie trzeba by go powielac w podklasach
	{
		cout << "stanowisko = " << stanowisko
			<< "\nwynagrodzenie = " << wynagrodzenie
		        << "\nprzedmiot = " << przedmiot << endl;
	}
};
class Administracja : public Pracownik{
	private: 
		string adress;
	public:
		Administracja(string adress) {this->adress = adress;}
        void pokazDane()
        {
                cout << "stanowisko = " << stanowisko
                        << "\nwynagrodzenie =  " << wynagrodzenie 
			<< "\nadress = "<< adress <<  endl;
        }
};
int main()
{
	Osoba him;
	him.SetImie("Alex");
	him.SetNazwisko("Lebron");
	him.SetWiek(23);
	cout << him.GetImie() << endl;
	cout << him.GetNazwisko() << endl;
	cout << him.GetWiek() << endl;
	him.SetTelefon("y4tutreu8g8fdahdu");
	him.SetEmail("a@g.com");
	cout << him.GetEmail() << endl;
	Nauczyciel jeden;
	jeden.SetStanowisko("One");
	jeden.SetWynagrodzenie(1);
	jeden.SetPrzedmiot("algebra");
	jeden.pokazDane();
	Administracja admin("49000");
	admin.SetStanowisko("?");
	admin.SetWynagrodzenie(999);
	admin.pokazDane();
	return 0;
}

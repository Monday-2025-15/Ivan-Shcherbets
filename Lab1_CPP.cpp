#include <iostream>       // rozwiazanie poprawne
#include <vector>
#include <random>
using namespace std;
class Car
{
public:
    string make;  // w wiekszosci przypadkow atrybuty powinny byc prywatne
    string model;
    int year;
    int currentYear;
    Car(string make, string model, int year, int currentYear)
    {
        this->make=make;
        this->model=model;
        this->year=year;
        this->currentYear=currentYear;
    }
    void displayInfo()
    {
        cout << "make: " << make << endl
             << "model: " << model << endl
             << "year: " << year << endl;
    }
    int getCarAge()
    {
        return currentYear-year;
    }
    bool isSameCar(Car otherCar)
    {
        return otherCar.year==year && otherCar.make==make && otherCar.model==model;
    }
};

class Parrot
{
private:
    vector<string> phrases;
public:
    Parrot(vector<string> phrases  = {"Hi", "Hello", "phrase", "car"})
    {
        this->phrases=phrases;
    }
    void say(int n=1)
    {
        for(int i=0; i < n; i++)
        {
            int randNum = rand() % phrases.size();
            cout << phrases[randNum] << endl;
        }
    }
    void addPhrase(string phrase)
    {
        phrases.push_back(phrase);
    }
};

int main()
{
    srand(time(0));
    Car car1("abc", "nissan", 1990, 2025);
    car1.displayInfo();
    cout << "age: " << car1.getCarAge() << endl;
    Car car2("abc", "toyota", 1995, 2026);
    cout << boolalpha;
    cout << "same? " << car1.isSameCar(car2) << endl;
    cout << "Commands:\n";
    cout << "  add [word]  -> add new phrase (no spaces)\n";
    cout << "  say [n]     -> say n times\n";
    cout << "  exit       -> quit program\n";
    Parrot parrot({"Hi", "Hello"});
    while (true) {
        string input;
        getline(cin, input);
        if (input=="exit") break;
        if (input.rfind("add",0)==0)
        {
            string phrase = input.substr(3);
            phrase.erase(0, phrase.find_first_not_of(" "));
            if (!phrase.empty()) parrot.addPhrase(phrase);
        }
        else if (input.rfind("say",0)==0)
        {
            string numStr = input.substr(3);
            numStr.erase(0, numStr.find_first_not_of(" "));
            if (!numStr.empty()) parrot.say(stoi(numStr));
            else parrot.say();
        }
    }
    return 0;
}

#include <iostream>
#include <string>
using namespace std;
//Singleton
class DatabaseConnection {
private:
    static DatabaseConnection* instance;
    DatabaseConnection() {}
public:
    DatabaseConnection(const DatabaseConnection&) = delete;
    void operator=(const DatabaseConnection&) = delete;
    static DatabaseConnection* getInstance() {
        if (instance == nullptr) {
            instance = new DatabaseConnection();
        }
        return instance;
    }

    void executeQuery(string query) {
        cout << "Wykonywanie zapytania: " << query << endl;
    }
};
DatabaseConnection* DatabaseConnection::instance = nullptr;
//Factory Method
class Vehicle {
public:
    virtual void move() = 0;
    virtual ~Vehicle() {}
};
class Car : public Vehicle {
public:
    void move() override {
        cout << "Jadę samochodem po drodze." << endl;
    }
};
class Bike : public Vehicle {
public:
    void move() override {
        cout << "Jadę rowerem po ścieżce." << endl;
    }
};
class VehicleFactory {
public:
    Vehicle* getVehicle(string type) {
        if (type == "CAR") {
            return new Car();
        }
        else if (type == "BIKE") {
            return new Bike();
        }
        return nullptr;
    }
};
//Builder;
class Pizza {
private:
    string dough;
    string sauce;
    string topping;

public:
    Pizza(string d, string s, string t)
        : dough(d), sauce(s), topping(t) {}

    class Builder {
    private:
        string dough;
        string sauce;
        string topping;

    public:
        Builder& setDough(string d) {
            this->dough = d;
            return *this;
        }

        Builder& setSauce(string s) {
            this->sauce = s;
            return *this;
        }

        Builder& setTopping(string t) {
            this->topping = t;
            return *this;
        }

        Pizza build() {
            return Pizza(dough, sauce, topping);
        }
    };

    void display() const {
        cout << "Pizza: " << dough
             << ", " << sauce
             << ", " << topping << endl;
    }
};

int main() {
    DatabaseConnection* connection1 = DatabaseConnection::getInstance();
    DatabaseConnection* connection2 = DatabaseConnection::getInstance();
    cout << "Czy obie instancje są takie same? " << (connection1 == connection2) << endl;

    VehicleFactory factory;
    Vehicle* v1 = factory.getVehicle("CAR");
    if(v1) v1->move();
    Vehicle* v2 = factory.getVehicle("BIKE");
    if(v2) v2->move();
    delete v1;
    delete v2;
    Pizza::Builder builder;
    Pizza myPizza = builder.setDough("Grube")
                           .setSauce("BBQ")
                           .setTopping("Kurczak")
                           .build();
    myPizza.display();
    return 0;
}
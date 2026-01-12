//Singleton
class DatabaseConnection {
    private static DatabaseConnection instance;

    private DatabaseConnection() {}

    public static DatabaseConnection getInstance() {
        if (instance == null) {
            instance = new DatabaseConnection();
        }
        return instance;
    }

    public void executeQuery(String query) {
        System.out.println("Wykonywanie zapytania: " + query);
    }
}
//Factory
interface Vehicle {
    void move();
}

class Car implements Vehicle {
    @Override
    public void move() {
        System.out.println("Jadę samochodem po drodze.");
    }
}

class Bike implements Vehicle {
    @Override
    public void move() {
        System.out.println("Jadę rowerem po ścieżce.");
    }
}

class VehicleFactory {
    public Vehicle getVehicle(String type) {
        if ("CAR".equals(type)) {
            return new Car();
        } else if ("BIKE".equals(type)) {
            return new Bike();
        }
        return null;
    }
}
//Builder
class Pizza {
    private String dough;
    private String sauce;
    private String topping;

    public Pizza(String d, String s, String t) {
        this.dough = d;
        this.sauce = s;
        this.topping = t;
    }

    public static class Builder {
        private String dough;
        private String sauce;
        private String topping;

        public Builder setDough(String d) {
            this.dough = d;
            return this;
        }

        public Builder setSauce(String s) {
            this.sauce = s;
            return this;
        }

        public Builder setTopping(String t) {
            this.topping = t;
            return this;
        }

        public Pizza build() {
            return new Pizza(dough, sauce, topping);
        }
    }

    public void display() {
        System.out.println("Pizza: " + dough + ", " + sauce + ", " + topping);
    }
}

public class Lab8_Java {
    public static void main(String[] args) {
        DatabaseConnection connection1 = DatabaseConnection.getInstance();
        DatabaseConnection connection2 = DatabaseConnection.getInstance();
        System.out.println("Czy obie instancje są takie same? " + (connection1 == connection2));

        VehicleFactory factory = new VehicleFactory();
        Vehicle v1 = factory.getVehicle("CAR");
        if (v1 != null) v1.move();
        Vehicle v2 = factory.getVehicle("BIKE");
        if (v2 != null) v2.move();

        Pizza.Builder builder = new Pizza.Builder();
        Pizza myPizza = builder.setDough("Grube")
                .setSauce("BBQ")
                .setTopping("Kurczak")
                .build();
        myPizza.display();
    }
}
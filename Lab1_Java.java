import java.util.Random;
import java.util.Arrays;
public class Parrot {
    private String[] phrases;
    private Random rand;

    public Parrot() {
        this.phrases = new String[]{"Hi", "Hello", "phrase", "car"};
        this.rand = new Random();
    }
    public Parrot(String[] phrases) {
        this.phrases = Arrays.copyOf(phrases, phrases.length);
        this.rand = new Random();
    }

    public void say() {
        say(1);
    }
    public void say(int n) {
        for (int i = 0; i < n; i++) {
            int randNum = rand.nextInt(phrases.length);
            System.out.println(phrases[randNum]);
        }
    }

    public void addPhrase(String phrase) {
        String[] newPhrases = Arrays.copyOf(phrases, phrases.length + 1);
        newPhrases[newPhrases.length - 1] = phrase;
        phrases = newPhrases;
    }
}
public class Car
{
    String make;
    String model;
    int year;
    int currentYear;
    public Car(String make, String model, int year, int currentYear)
    {
        this.make = make;
        this.model = model;
        this.year = year;
        this.currentYear = currentYear;
    }
    public void displayInfo()
    {
        System.out.println("Make: " + make);
        System.out.println("Model: " + model);
        System.out.println("Year: " + year);
    }
    public int getCarAge()
    {
        return currentYear - year;
    }
    public boolean isSameCar (Car otherCar) {
        return otherCar.year == year && otherCar.make.equals(this.make) && otherCar.model.equals(this.model);
    }
}

void main()
{
    Car car1 = new Car("abc", "nissan", 1990, 2025);
    car1.displayInfo();
    System.out.println("age: " + car1.getCarAge());
    Car car2 = new Car("abc", "toyota", 1995, 2026);
    System.out.println("same? " + car1.isSameCar(car2));
    System.out.println("Commands:");
    System.out.println("  add [word]  -> add new phrase (no spaces)");
    System.out.println("  say [n]     -> say n times");
    System.out.println("  exit        -> quit program");
    Parrot parrot = new Parrot(new String[]{"Hi", "Hello"});
    Scanner scanner = new Scanner(System.in);
    while (true) {
        String input = scanner.nextLine().trim();
        if (input.equals("exit")) break;

        if (input.startsWith("add")) {
            String phrase = input.substring(3).trim();
            if (!phrase.isEmpty()) {
                parrot.addPhrase(phrase);
            }
        } else if (input.startsWith("say")) {
            String numStr = input.substring(3).trim();
            if (!numStr.isEmpty()) {
                try {
                    int n = Integer.parseInt(numStr);
                    parrot.say(n);
                } catch (NumberFormatException e) {
                    System.out.println("Invalid number.");
                }
            } else {
                parrot.say();
            }
        }
    }
    scanner.close();
}

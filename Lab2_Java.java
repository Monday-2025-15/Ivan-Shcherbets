public class Samochod
{
    String marka;
    String model;
    int rokProdukcji;

    public Samochod(String marka, String model, int rokProdukcji)
    {
        this.marka = marka;
        this.model = model;
        this.rokProdukcji = rokProdukcji;
    }
    public Samochod()
    {
        this.marka = "Toyota";
        this.model = "Camry";
        this.rokProdukcji = 2004;
    }
}
public class Tablica
{
    public int[] tablica;

    public Tablica()
    {
        tablica = new int[10];
    }
    public Tablica(int n)
    {
        tablica = new int[n];
    }
}

void main()
{
    Samochod car;
    Tablica tab = new Tablica(5);
}
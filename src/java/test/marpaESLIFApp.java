
import org.marpa.ESLIF;

public class marpaESLIFApp {
	
	public static void main(String[] args) {
		ESLIF eSLIF = new ESLIF();
		eSLIF.init();
        System.out.println("Hello World!"); // Display the string.
		eSLIF.release();
    }
}

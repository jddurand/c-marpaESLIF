
import java.util.logging.ConsoleHandler;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.marpa.ESLIF;
import org.marpa.ESLIFOption;

public class marpaESLIFApp {
	private static final String CLASS = marpaESLIFApp.class.getName();
	private static final Logger LOGGER = Logger.getLogger(CLASS);
	
	public static void main(String[] args) {
		
		Handler ch = new ConsoleHandler();
		ch.setLevel(Level.ALL);
		LOGGER.addHandler(ch);
		LOGGER.setLevel(Level.ALL);
		LOGGER.log(Level.CONFIG, "TEST");
		
		ESLIFOption eslifOption = new ESLIFOption(LOGGER);
		ESLIF eslif = new ESLIF(eslifOption);
		eslif.init();
        System.out.println("Hello World!"); // Display the string.
        eslif.release();
    }
}

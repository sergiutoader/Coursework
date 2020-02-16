/* TOADER Sergiu-Cristian - 325CB */

import java.io.PrintWriter;
import java.util.Scanner;

/**
 * Class containing methods for each command
 *
 */
public class OperationHandler {

	/**
	 * Inserts new temperature observation in the database of the given room's
	 * device
	 * 
	 * @param input Scanner object
	 * @param house House class instance
	 */
	public void executeObserve(Scanner input, House house) {
		house.getDevice(input.next()).observe(input.nextInt(), house.getRefTimeStamp(), input.nextDouble());
	}

	/**
	 * 
	 * Inserts new humidity observation in the database of the given room's device
	 * 
	 * @param input Scanner object
	 * @param house House class instance
	 */
	public void executeObserveH(Scanner input, House house) {
		house.getDevice(input.next()).observeH(input.nextInt(), house.getRefTimeStamp(), input.nextDouble());
	}

	public void executeTriggerHeat(Scanner input, PrintWriter writer, House house) {
		input.next(); // Ignore the word 'HEAT' (since the word 'TRIGGER' has already been scanned)
		house.triggerHeat(writer);
	}

	/**
	 * Method used to execute command TEMPERATURE
	 * 
	 * @param input Scanner object
	 * @param house House class instance
	 */
	public void setNewTemperature(Scanner input, House house) {
		double newTemperature = input.nextDouble();
		house.setGlobalTemperature(newTemperature); // Set new value for globalTemperature
	}

	public void executeList(Scanner input, PrintWriter writer, House house) {
		house.getRoom(input.next()).list(input.nextInt(), input.nextInt(), house.getRefTimeStamp(), writer);
	}
}

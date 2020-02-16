/* TOADER Sergiu-Cristian - 325CB */

import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintWriter;

/**
 * Class containing main method. Used to create instances for InputData,
 * PrintWriter and House classes and call very general methods that set room
 * information and execute all commands.
 * 
 */
public class Main {

	public static void main(String[] args) {

		InputData input = null;
		PrintWriter writer = null;
		// Create instance for input and writer
		try {
			input = new InputData();
			writer = new PrintWriter(new File("therm.out"));
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
		// Create instance of the House class
		House house = input.getHouseInfo();
		house.setRooms(input.getRoomsInfo(house.getNumberOfRooms()));

		// Executing commands
		input.executeCommands(house, writer);

		// Closing input and output
		input.closeInput();
		writer.close();

	}
}

/* TOADER Sergiu-Cristian - 325CB */

import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Scanner;

/**
 * Class that scans input data and handles it, calling adequate methods for each
 * operation
 *
 */
public class InputData {

	private Scanner input;

	public InputData() throws FileNotFoundException {
		input = new Scanner(new File("therm.in"));
	}

	/**
	 * Method used to scan first line of input and create a House object
	 * 
	 * @return House object
	 */
	public House getHouseInfo() {
		int param1 = input.nextInt();
		double param2 = input.nextDouble();
		String param3 = input.next();

		// Check if parameter 3 represents a humidity value or a timestamp
		// and calls a constructor of the house accordingly
		if (param3.length() > 5) {
			return new House(param1, param2, Integer.parseInt(param3));
		} else {
			int param4 = input.nextInt();
			return new House(param1, param2, Double.parseDouble(param3), param4);
		}

	}

	/**
	 * Method that scans information about the rooms and creates an array list
	 * containing the rooms created
	 * 
	 * @param numberOfRooms The first token in the input file
	 * @return ArrayList containing the rooms created
	 */
	public ArrayList<Room> getRoomsInfo(int numberOfRooms) {
		ArrayList<Room> rooms = new ArrayList<Room>(numberOfRooms);
		for (int i = 0; i < numberOfRooms; i++) {
			rooms.add(new Room(input.next(), input.next(), input.nextDouble()));
		}
		return rooms;
	}

	/**
	 * Method that scans the input file and executes commands accordingly
	 * 
	 * @param house  House object to get information about the house
	 * @param writer PrintWriter object to write to output file
	 */
	public void executeCommands(House house, PrintWriter writer) {
		OperationHandler handler = new OperationHandler();
		while (input.hasNext()) {
			switch (input.next()) {
			case "OBSERVE":
				handler.executeObserve(input, house);
				break;
			case "OBSERVEH":
				handler.executeObserveH(input, house);
				break;
			case "TRIGGER":
				handler.executeTriggerHeat(input, writer, house);
				break;
			case "TEMPERATURE":
				handler.setNewTemperature(input, house);
				break;
			case "LIST":
				handler.executeList(input, writer, house);
				break;
			default:
				break;
			}
		}
	}

	public void closeInput() {
		input.close();
	}
}

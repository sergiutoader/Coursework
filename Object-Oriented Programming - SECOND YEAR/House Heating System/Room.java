/* TOADER Sergiu-Cristian - 325CB */

import java.io.PrintWriter;
import java.util.ArrayList;

/**
 * Class containing information about the rooms (name, surface, device). It
 * contains a method for the LIST operation
 * 
 */
public class Room {
	private String roomName;
	private Device device;
	private double surface;

	public Room(String roomName, String deviceId, double surface) {
		this.roomName = roomName;
		this.device = new Device(deviceId);
		this.surface = surface;
	}

	/**
	 * Method used for the LIST command. The method iterates through all the
	 * intervals from startInterval from stopInterval and prints all the temperature
	 * values
	 * 
	 * @param startInterval First time stamp
	 * @param stopInterval  Second time stamp
	 * @param refTimeStamp  Reference time stamp
	 * @param writer        PrintWriter object
	 */
	public void list(int startInterval, int stopInterval, int refTimeStamp, PrintWriter writer) {
		// StringBuilder object used to store the output string
		StringBuilder outputString = new StringBuilder();
		// ArrayList object used to store all the interval keys
		ArrayList<Integer> keys = new ArrayList<Integer>();

		// Only complete intervals are taken into account
		for (int key = refTimeStamp - stopInterval; key <= refTimeStamp
				- (startInterval + TimeConstants.ONE_HOUR_INTERVAL); key += TimeConstants.ONE_HOUR_INTERVAL) {
			keys.add(key / TimeConstants.ONE_HOUR_INTERVAL);
		}

		outputString.append(this.getRoomName() + " ");
		// Print each temperature from each interval from start to stop with 2 decimal
		// format
		for (int key : keys) {
			for (double temperature : this.getDevice().getTemperatureMap().get(key)) {
				outputString.append(String.format("%.2f ", temperature));
			}

		}
		// Trim last space character
		outputString.setLength(outputString.length() - 1);
		// Print string to output file
		writer.println(outputString);
	}

	// Setters & Getters

	public String getRoomName() {
		return roomName;
	}

	public void setRoomName(String roomName) {
		this.roomName = roomName;
	}

	public Device getDevice() {
		return device;
	}

	public void setDevice(Device device) {
		this.device = device;
	}

	public double getSurface() {
		return surface;
	}

	public void setSurface(double surface) {
		this.surface = surface;
	}
}

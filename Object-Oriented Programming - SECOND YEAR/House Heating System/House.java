/* TOADER Sergiu-Cristian - 325CB */

import java.io.PrintWriter;
import java.util.ArrayList;

/**
 * Class containing information about the house
 *
 */
public class House {

	private double globalTemperature;
	private double globalHumidity;

	/**
	 * ArrayList containing all the rooms in the house
	 */
	private ArrayList<Room> rooms;
	private int refTimeStamp;
	private int numberOfRooms;

	public House(int numberOfRooms, double globalTemperature, int refTimeStamp) {
		this.numberOfRooms = numberOfRooms;
		this.globalTemperature = globalTemperature;
		this.refTimeStamp = refTimeStamp;
	}

	public House(int numberOfRooms, double globalTemperature, double globalHumidity, int refTimeStamp) {
		this.numberOfRooms = numberOfRooms;
		this.globalTemperature = globalTemperature;
		this.globalHumidity = globalHumidity;
		this.refTimeStamp = refTimeStamp;
	}

	/**
	 * Method that determines the average temperature in the house of the last hour
	 * by calculating the weighted mean of the minimum temperature in each room
	 * 
	 * @return Average temperature of the house
	 */
	public double getAverageTemperature() {
		double numerator = 0;
		double denominator = 0;
		for (Room r : rooms) {
			for (int i = 0; i < TimeConstants.HOURS_PER_DAY; i++) {
				if (r.getDevice().getTemperatureInterval(i).size() != 0) {
					// first() returns the lowest temperature
					// getTemperatureInterval(i) returns interval of the most recent recorded
					// temperature
					numerator += r.getDevice().getTemperatureInterval(i).first() * r.getSurface();
					denominator += r.getSurface();
					break;
				}
			}
		}
		return numerator / denominator;
	}

	/**
	 * Method that determines the average humidity in the house of the last hour by
	 * calculating the weighted mean of the maximum humidity value in each room
	 * 
	 * @return Average humidity of the house
	 */
	public double getAverageHumidity() {
		double numerator = 0;
		double denominator = 0;
		for (Room r : rooms) {
			for (int i = 0; i < TimeConstants.HOURS_PER_DAY; i++) {
				if (r.getDevice().getHumidityInterval(i).size() != 0) {
					// last() returns the observation with the highest humidity value
					// getHumidityInterval(i) returns interval of the most recent recorded humidity
					numerator += r.getDevice().getHumidityInterval(i).last() * r.getSurface();
					denominator += r.getSurface();
					break;
				}
			}
		}
		return numerator / denominator;
	}

	/**
	 * Method that compares registered temperatures and humidity with global
	 * temperature and global humidity and triggers heat
	 * 
	 * @param writer PrintWriter object
	 */
	public void triggerHeat(PrintWriter writer) {
		if (getAverageHumidity() > getGlobalHumidity()) {
			writer.println("NO");
		} else {
			if (getAverageTemperature() < getGlobalTemperature()) {
				writer.println("YES");
			} else if (getAverageTemperature() >= getGlobalTemperature()) {
				writer.println("NO");
			}
		}
	}

	// Setters & Getters

	public Room getRoom(String roomName) {
		for (Room r : rooms) {
			if (r.getRoomName().equals(roomName)) {
				return r;
			}
		}
		return null;
	}

	public void setRooms(ArrayList<Room> rooms) {
		this.rooms = rooms;
	}

	public int getRefTimeStamp() {
		return refTimeStamp;
	}

	public void setRefTimeStamp(int refTimeStamp) {
		this.refTimeStamp = refTimeStamp;
	}

	public double getGlobalTemperature() {
		return globalTemperature;
	}

	/**
	 * Method used for TEMPERATURE command
	 * 
	 * @param globalTemperature New global temperature
	 */
	public void setGlobalTemperature(double globalTemperature) {
		this.globalTemperature = globalTemperature;
	}

	public double getGlobalHumidity() {
		return globalHumidity;
	}

	public void setGlobalHumidity(double globalHumidity) {
		this.globalHumidity = globalHumidity;
	}

	public int getNumberOfRooms() {
		return numberOfRooms;
	}

	/**
	 * Method used to find a device in the array list
	 * 
	 * @param deviceId The id of the device
	 * @return Device with the given id
	 */
	public Device getDevice(String deviceId) {
		for (Room r : rooms) {
			if (r.getDevice().getDeviceId().equals(deviceId)) {
				return r.getDevice();
			}
		}
		return null;
	}
}

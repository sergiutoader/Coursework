/* TOADER Sergiu-Cristian - 325CB */

import java.util.HashMap;
import java.util.TreeSet;

/**
 * Class containing information about each device, the database of the device
 * and methods to add information to the database
 *
 */
public class Device {

	private String deviceId;

	/**
	 * Temperatures registered are mapped on a HashMap. The key represents the
	 * interval (the number of hours which have passed since the observation has
	 * been made) and the values are the temperatures stored in order in a TreeSet
	 */
	private HashMap<Integer, TreeSet<Double>> temperatureMap;

	/**
	 * Humidity values are mapped on a hash map similarly to temperatures
	 */
	private HashMap<Integer, TreeSet<Double>> humidityMap;

	/**
	 * Creates instances for the HashMap and each TreeSet in it
	 * 
	 * @param deviceId The second token in an input line containing room info
	 */
	public Device(String deviceId) {
		this.deviceId = deviceId;
		this.temperatureMap = new HashMap<Integer, TreeSet<Double>>(TimeConstants.HOURS_PER_DAY);
		this.humidityMap = new HashMap<Integer, TreeSet<Double>>(TimeConstants.HOURS_PER_DAY);

		for (int i = 0; i < TimeConstants.HOURS_PER_DAY; i++) {
			this.temperatureMap.put(i, new TreeSet<Double>());
			this.humidityMap.put(i, new TreeSet<Double>());
		}
	}

	/**
	 * Adds new temperature in the database
	 * 
	 * @param timeStamp Third token in an OBSERVE input line
	 * @param refTimeStamp Reference time stamp
	 * @param temperature The registered value
	 */
	public void observe(int timeStamp, int refTimeStamp, double temperature) {
		// Ignore values that are recorded after the reference time stamp
		if (timeStamp > refTimeStamp) {
			return;
		}
		int key = getKey(timeStamp, refTimeStamp); // Get interval from hash map
		TreeSet<Double> aux = temperatureMap.get(key);
		aux.add(temperature); // Add observed temperature to the interval
		this.temperatureMap.put(key, aux); // Update the database
	}

	/**
	 * Adds new humidity in the database
	 * 
	 * @param timeStamp Third token in an OBSERVE input line
	 * @param refTimeStamp Reference time stamp
	 * @param humidity The registered value
	 */
	public void observeH(int timeStamp, int refTimeStamp, double humidity) {
		// Ignore values that are recorded after the reference time stamp
		if (timeStamp > refTimeStamp) {
			return;
		}
		int key = getKey(timeStamp, refTimeStamp); // Get interval from hash map
		TreeSet<Double> aux = humidityMap.get(key);
		aux.add(humidity); // Add observed humidity to the interval
		this.humidityMap.put(key, aux); // Update the database
	}

	/**
	 * Method used to determine the interval, given a certain time stamp
	 * 
	 * @param timeStamp Current time stamp
	 * @param refTimeStamp Reference time stamp
	 * @return The interval corresponding to the time stamp
	 */
	private int getKey(int timeStamp, int refTimeStamp) {
		return (refTimeStamp - timeStamp) / TimeConstants.ONE_HOUR_INTERVAL;
	}

	// Setters & Getters

	public TreeSet<Double> getTemperatureInterval(int key) {
		return this.temperatureMap.get(key);
	}

	public HashMap<Integer, TreeSet<Double>> getTemperatureMap() {
		return temperatureMap;
	}

	public void setTemperatureMap(HashMap<Integer, TreeSet<Double>> intervals) {
		this.temperatureMap = intervals;
	}

	public TreeSet<Double> getHumidityInterval(int key) {
		return this.humidityMap.get(key);
	}

	public HashMap<Integer, TreeSet<Double>> getHumidityMap() {
		return humidityMap;
	}

	public void setHumidityMap(HashMap<Integer, TreeSet<Double>> intervals) {
		this.humidityMap = intervals;
	}

	public String getDeviceId() {
		return deviceId;
	}

	public void setDeviceId(String deviceId) {
		this.deviceId = deviceId;
	}
}

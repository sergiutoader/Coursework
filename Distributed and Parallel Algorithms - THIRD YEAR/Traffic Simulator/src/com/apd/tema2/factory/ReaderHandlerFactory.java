package com.apd.tema2.factory;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Pedestrians;
import com.apd.tema2.entities.ReaderHandler;
import com.apd.tema2.intersections.*;

import java.io.BufferedReader;
import java.io.IOException;

/**
 * Returneaza sub forma unor clase anonime implementari pentru metoda de citire din fisier.
 */
public class ReaderHandlerFactory {

    public static ReaderHandler getHandler(String handlerType) {

        return switch (handlerType) {
            case "simple_semaphore" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) {
                    Main.intersection = new SimpleSemaphore();
                }
            };
            case "simple_n_roundabout" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {

                    String[] line = br.readLine().split(" ");

                    int N = Integer.parseInt(line[0]);
                    int passingTime = Integer.parseInt(line[1]);

                    Main.intersection = new SimpleIntersection(N, passingTime);
                }
            };
            case "simple_strict_1_car_roundabout" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    String[] line = br.readLine().split(" ");

                    int directions = Integer.parseInt(line[0]);
                    int spentTime = Integer.parseInt(line[1]);

                    // particularizare a task-ului 4 pentru maxCarsCount = 1
                    Main.intersection = new SimpleStrictCarIntersection(directions, spentTime, 1);
                }
            };
            case "simple_strict_x_car_roundabout" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    String[] line = br.readLine().split(" ");

                    int directions = Integer.parseInt(line[0]);
                    int spentTime = Integer.parseInt(line[1]);

                    int maxCarsCount = Integer.parseInt(line[2]);

                    Main.intersection = new SimpleStrictCarIntersection(directions, spentTime, maxCarsCount);
                }
            };
            case "simple_max_x_car_roundabout" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    String[] line = br.readLine().split(" ");

                    int directions = Integer.parseInt(line[0]);
                    int spentTime = Integer.parseInt(line[1]);

                    int maxCarsCount = Integer.parseInt(line[2]);

                    Main.intersection = new SimpleStrictCarIntersection(directions, spentTime, maxCarsCount);
                }
            };
            case "priority_intersection" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    Main.intersection = new PriorityIntersection(Main.carsNo);
                }
            };
            case "crosswalk" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    String[] line =  br.readLine().split(" ");

                    int duration = Integer.parseInt(line[0]);
                    int pedestriansCount = Integer.parseInt(line[1]);

                    Main.pedestrians = new Pedestrians(duration, pedestriansCount);
                    Main.intersection = new Crosswalk(Main.carsNo);
                }
            };
            case "simple_maintenance" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    String line = br.readLine();

                    int maxCarsNo = Integer.parseInt(line);

                    Main.intersection = new SimpleMaintenance(maxCarsNo);

                }
            };
            case "complex_maintenance" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    String[] words = br.readLine().split(" ");
                    int freeLanes = Integer.parseInt(words[0]);
                    int totalInitialLanes = Integer.parseInt(words[1]);
                    int passingCarsInOneGo = Integer.parseInt(words[2]);

                    Main.intersection = new ComplexMaintenance(freeLanes, totalInitialLanes, passingCarsInOneGo);
                }
            };
            case "railroad" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    int carsNo = Main.carsNo;
                    Main.intersection = new Railroad(carsNo);
                }
            };
            default -> null;
        };
    }

}

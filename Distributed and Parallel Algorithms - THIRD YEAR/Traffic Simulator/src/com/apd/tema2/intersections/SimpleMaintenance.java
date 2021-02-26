package com.apd.tema2.intersections;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.concurrent.ArrayBlockingQueue;

public class SimpleMaintenance implements Intersection {

    // numarul minim de masini necesare pentru a debloca intersectia
    private final int carsNo;

    // masinile ramase in intersectie
    private int totalCars;
    // lock pentru sincronizare
    private final String lock = "";

    // cozi de asteptare pentru masinile care circula pe sensul 0, respectiv 1
    private final ArrayBlockingQueue<Car> queue0;
    private final ArrayBlockingQueue<Car> queue1;

    public SimpleMaintenance(int carsNo) {
        this.carsNo = carsNo;

        this.totalCars = Main.carsNo;

        queue0 = new ArrayBlockingQueue<>(Main.carsNo);
        queue1 = new ArrayBlockingQueue<>(Main.carsNo);

    }
    // metoda apelata cand masina intra in intersectie
    public void enter(Car car) {
        System.out.println("Car " + car.getId() + " from side number " + car.getStartDirection() + " has reached the bottleneck");

        // adaugare in coada de asteptare corespunzatoare
        if(car.getStartDirection() == 0) {
            queue0.add(car);
        } else {
            queue1.add(car);
        }

        // dupa adaugare, se verifica daca se indeplineste conditia de trecere
        checkPassingPossible();
    }

    private void checkPassingPossible() {
        // verificarea se face sincronizat, pentru ca iesirile sa se produca imediat ce este posibil
        synchronized (lock) {
            // daca ambele cozi contin numarul minim de masini necesar, se scot carsNo masini din fiecare coada si se
            // scade numarul de masini scoase din numarul total de masini
            if(queue0.size() >= carsNo && queue1.size() >= carsNo) {
                for(int i = 0; i < carsNo; i++) {
                    exit(queue0.poll());
                }
                totalCars -= carsNo;
                for(int i = 0; i < carsNo; i++) {
                    exit(queue1.poll());
                }
                totalCars -= carsNo;
            }
            // deoarece se scot alternativ masini in ordinea lane 0, lane 1, lane 0, ... , la final se verifica daca
            // au mai ramas in queue0 masini. Aceste masini vor pleca din intersectie si ele.
            if(totalCars <= carsNo && totalCars != 0) {
                for(int i = 0; i < totalCars; i++) {
                    exit(queue0.poll());
                }
                totalCars = 0;
            }
        }
    }

    // iesire din intersectie
    private void exit(Car car) {
        System.out.println("Car " + car.getId() + " from side number " + car.getStartDirection() + " has passed the bottleneck");
    }
}

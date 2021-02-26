package com.apd.tema2.intersections;

import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.concurrent.Semaphore;

// sincronizarea se realizeaza cu ajutorul unui semafor - astfel maximum N masini vor putea fi simultan in intersectie
public class SimpleIntersection implements Intersection {

    private final Semaphore sem;
    private final int passingTime;

    public SimpleIntersection(int N, int waitingTime) {
        this.sem = new Semaphore(N);
        this.passingTime = waitingTime;
    }

    public Semaphore getSem() {
        return sem;
    }

    public int getPassingTime() {
        return passingTime;
    }

    // masina ajunge in intersectie
    public void reach(Car car) {
        System.out.println("Car " + car.getId() + " has reached the roundabout, now waiting...");

        // masina incearca sa intre in intersectie (daca acest lucru este posibil)
        try {
            this.getSem().acquire();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // masina intra in intersectie
    public void enter(Car car) {
        System.out.println("Car " + car.getId() + " has entered the roundabout");

        try {
            Thread.sleep(this.getPassingTime());
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    // masina iese din intersectie
    public void exit(Car car) {
        System.out.println("Car " + car.getId() + " has exited the roundabout after " +
                (this.getPassingTime() / 1000) + " seconds");

        // elibereaza semaforul, permitand altei masini sa intre
        this.getSem().release();
    }
}

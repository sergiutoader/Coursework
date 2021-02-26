package com.apd.tema2.intersections;

import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

public class SimpleSemaphore implements Intersection {

    public SimpleSemaphore() {
    }

    // masina ajunge la semafor
    public void reachSemaphore(Car car) {
        System.out.println("Car " + car.getId() + " has reached the semaphore, now waiting...");
    }

    // masina asteapta la semafor
    public void waitAtSemaphore(Car car) {
        try {
            Thread.sleep(car.getWaitingTime());
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    // masina conduce mai departe
    public void drive(Car car) {
        System.out.println("Car " + car.getId() + " has waited enough, now driving...");
    }
}

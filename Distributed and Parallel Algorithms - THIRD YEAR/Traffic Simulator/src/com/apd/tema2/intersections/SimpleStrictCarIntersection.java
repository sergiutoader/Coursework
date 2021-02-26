package com.apd.tema2.intersections;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.Semaphore;
import static java.lang.Thread.sleep;

/*
    Clasa utilizata pentru task-urile 3, 4, 5
*/
public class SimpleStrictCarIntersection implements Intersection {

    int directions;
    int timeSpent;

    private final CyclicBarrier barrier;

    private final Semaphore[] semArray;

    private final CyclicBarrier allCarsBarrier;

    public SimpleStrictCarIntersection(int directions, int timeSpent, int maxCarsCount) {
        this.directions = directions;
        this.timeSpent = timeSpent;

        this.barrier = new CyclicBarrier(maxCarsCount * directions);
        this.semArray = new Semaphore[directions];

        for(int i = 0; i < semArray.length; i++) {
            semArray[i] = new Semaphore(maxCarsCount);
        }

        allCarsBarrier = new CyclicBarrier(Main.carsNo);

    }

    public int getTimeSpent() {
        return timeSpent;
    }

    public CyclicBarrier getBarrier() {
        return barrier;
    }

    public Semaphore getSemaphore(int idx) {
        return semArray[idx];
    }

    public CyclicBarrier getAllCarsBarrier() {
        return allCarsBarrier;
    }

    // masina ajunge in intersectie
    public void reach(Car car, boolean waitForAllCars) {

        // pentru task 4 - se asteapta ca toate masinile sa intre in intersectie
        if(waitForAllCars) {
            System.out.println("Car " + car.getId() + " has reached the roundabout, now waiting...");
            try {
                this.getAllCarsBarrier().await();
            } catch (InterruptedException | BrokenBarrierException e) {
                e.printStackTrace();
            }
        } else {
            System.out.println("Car " + car.getId() + " has reached the roundabout");
        }

        // masina incearca sa intre in intersectie (daca semaforul de pe linia sa are permit disponibil)
        try {
            this.getSemaphore(car.getStartDirection()).acquire();
        } catch (Exception e) {
            e.printStackTrace();
        }

        if(waitForAllCars) {
            System.out.println("Car " + car.getId() + " was selected to enter the roundabout from lane " +
                    car.getStartDirection());
        }

        // masina asteapta pana cand toate masinile au ajuns in intersectie
        try {
            this.getBarrier().await();
        } catch (InterruptedException | BrokenBarrierException e) {
            e.printStackTrace();
        }
    }

    // masina intra in intersectie
    public void enter(Car car) {
        System.out.println("Car " + car.getId() +  " has entered the roundabout from lane " +
                car.getStartDirection());

        try {
            Thread.sleep(this.getTimeSpent());
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    // masina iese din intersectie
    public void exit(Car car, boolean waitingForAllCars) {

        System.out.println("Car " + car.getId() + " has exited the roundabout after " +
                (this.getTimeSpent() / 1000) + " seconds");

        if(waitingForAllCars) {
            try {
                this.getBarrier().await();
            } catch (InterruptedException | BrokenBarrierException e) {
                e.printStackTrace();
            }
        }

        // eliberare semafor pentru directia curenta
        this.getSemaphore(car.getStartDirection()).release();
    }

    // metode pentru task 5
    public void reachMax(Car car) {
        // masina ajunge in dreptul intersectiei
        System.out.println("Car " + car.getId() + " has reached the roundabout from lane " +
                car.getStartDirection());

        // se ocupa un loc in intersectie
        try {
            this.getSemaphore(car.getStartDirection()).acquire();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void enterMax(Car car) {
        // masina intra in intersectie
        System.out.println("Car " + car.getId() + " has entered the roundabout from lane " +
                car.getStartDirection());

        // masina parcurge intersectia
        try {
            sleep(this.getTimeSpent());
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public void exitMax(Car car) {
        // masina iese din intersectie
        System.out.println("Car " + car.getId() + " has exited the roundabout after " +
                (this.getTimeSpent() / 1000) + " seconds");

        // se elibereaza un loc in intersectie
        this.getSemaphore(car.getStartDirection()).release();

    }
}

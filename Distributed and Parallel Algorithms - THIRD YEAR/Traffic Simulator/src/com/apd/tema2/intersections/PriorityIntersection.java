package com.apd.tema2.intersections;

import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.atomic.AtomicInteger;

public class PriorityIntersection implements Intersection {

    // numarul de masini din intersectie
    private final AtomicInteger carsInIntersection;
    // lock pentru sincronizarea operatiilor de wait si notify
    private final String lock = "";

    // coada in care vor astepta masinile de prioritate scazuta
    private final ArrayBlockingQueue<Car> lowPriorityQueue;

    public PriorityIntersection(int carsNo) {
        carsInIntersection = new AtomicInteger(0);
        lowPriorityQueue = new ArrayBlockingQueue<>(carsNo);
    }

    // metoda pentru intrarea masinilor de prioritate mica in intersectie
    public void lowPriorityEnterIntersection(Car car) {
        System.out.println("Car " + car.getId() + " with low priority is trying to enter the intersection...");

        // daca exista masini in intersectie, thread-ul curent intra in modul de lock pana cand intersectia devine goala
        if(carsInIntersection.get() > 0) {
            // se adauga in coada de asteptare masina
            lowPriorityQueue.add(car);
            synchronized (lock) {
                try {
                    lock.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        } else {
            // daca nu sunt masini in intersectie, masina va trece imediat
            System.out.println("Car " + car.getId() + " with low priority has entered the intersection");
        }


    }
    // metoda pentru intrarea in intersectie a masinilor cu prioritate
    public void highPriorityEnterIntersection(Car car) {
        // se incrementeaza variabila care numara masinile din intersectie
        carsInIntersection.incrementAndGet();
        System.out.println("Car " + car.getId() + " with high priority has entered the intersection");

    }
    // metoda pentru iesirea din intersectie a masinilor cu prioritates
    public void highPriorityExitIntersection(Car car) {

        System.out.println("Car " + car.getId() + " with high priority has exited the intersection");

        // se decrementeaza counterul de masini si se verifica egalitatea cu 0
        if(carsInIntersection.decrementAndGet() == 0) {
            // se realizeaza intrarea in intersectie a masinilor cu prioritate mica
            for (Car c : lowPriorityQueue) {
                System.out.println("Car " + c.getId() + " with low priority has entered the intersection");
            }
            // se deblocheaza toate thread-urile masinilor blocate
            synchronized (lock) {
                lock.notifyAll();
            }
            // se elibereaza coada de prioritate
            lowPriorityQueue.clear();
        }
    }
}

package com.apd.tema2.intersections;

import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.atomic.AtomicInteger;

public class Railroad implements Intersection {
    // lock pentru sincronizarea operatiilor de wait si notify
    private final String lock = "";
    // retine numarul de masini in asteptare la calea ferata
    private final AtomicInteger carsWaiting;
    // numarul total de masini
    private final int carsNo;

    // cozi de asteptare pentru sensul 0 si sensul 1
    private final ArrayBlockingQueue<Car> queue0;
    private final ArrayBlockingQueue<Car> queue1;


    public Railroad(int carsNo) {
        queue0 = new ArrayBlockingQueue<>(carsNo);
        queue1 = new ArrayBlockingQueue<>(carsNo);

        carsWaiting = new AtomicInteger(0);
        this.carsNo = carsNo;
    }
    // metoda apelata cand o masina ajunge la calea ferata
    public void enter(Car car) {
        System.out.println("Car " + car.getId() + " from side number " + car.getStartDirection() + " has stopped by the railroad");
        // incrementare numar de masini care asteapta
        carsWaiting.incrementAndGet();

        // adaugare in coada de asteptare corespunzatoare
        if(car.getStartDirection() == 0) {
            queue0.add(car);
        } else if(car.getStartDirection() == 1) {
            queue1.add(car);
        }
        // fiecare masina va verifica daca se poate trece
        checkPassingPossible();
    }

    private void checkPassingPossible() {

        // daca numarul de masini care asteapta a ajuns la limita de deblocare, masinile aflate in cozile de asteptare
        // trec de calea ferata
        if(carsWaiting.get() == carsNo) {
            System.out.println("The train has passed, cars can now proceed");

            // notificarea thread-urilor in asteptare
            synchronized (lock) {
                lock.notifyAll();
            }

            // se creeaza 2 thread-uri pentru fiecare sens de mers
            ResumeDriving resumeLane0 = new ResumeDriving(queue0);
            ResumeDriving resumeLane1 = new ResumeDriving(queue1);

            // pornire thread-uri
            resumeLane0.start();
            resumeLane1.start();

            try {
                resumeLane0.join();
                resumeLane1.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            // golire cozi de asteptare
            queue0.clear();
            queue1.clear();

        } else {
            // daca nu s-a ajuns inca la limita, thread-ul este blocat
            synchronized (lock) {
                try {
                    lock.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}

// thread-uri care se ocupa de "pornirea" masinilor care asteptau la coada
class ResumeDriving extends Thread {

    private final ArrayBlockingQueue<Car> carQueue;

    public ResumeDriving(ArrayBlockingQueue<Car> queue) {
        carQueue = queue;
    }

    @Override
    public void run() {
        for(Car c : carQueue) {
            System.out.println("Car " + c.getId() + " from side number " + c.getStartDirection() + " has started driving");
        }
    }
}

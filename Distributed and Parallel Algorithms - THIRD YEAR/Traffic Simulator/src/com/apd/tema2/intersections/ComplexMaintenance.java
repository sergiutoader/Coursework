package com.apd.tema2.intersections;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class ComplexMaintenance implements Intersection {

    private final int newLanesCount;
    private final int permits;

    private final CyclicBarrier barrier;

    private final int subArraySize;

    private final List<ArrayList<OldLane>> laneCoupling;

    public ComplexMaintenance(int newLanesCount, int oldLanesCount, int permits) {
        this.newLanesCount = newLanesCount;
        this.permits = permits;

        barrier = new CyclicBarrier(Main.carsNo);

        // dimensiunea unei subliste (cate elemente va avea un Li)
        subArraySize = oldLanesCount / newLanesCount;

        // lista in care un element reprezinta Li
        laneCoupling = Collections.synchronizedList(new ArrayList<>(newLanesCount));

        // se adauga cele newLanesCount liste
        for(int i = 0; i < newLanesCount - 1; i++) {
            laneCoupling.add(new ArrayList<>(subArraySize));
        }

        // daca impartirea la subArraySize nu a fost exacta, ultima lista va fi de dimensiune mai mare
        laneCoupling.add(new ArrayList<>(subArraySize + (oldLanesCount % newLanesCount)));

        // caz particular - exista o singura linie noua - nu se va intra pe niciunul din cazurile anterioare
        if(newLanesCount == 1) {
            laneCoupling.add(new ArrayList<>(newLanesCount));
        }

        for(int i = 0; i < oldLanesCount; i++) {
            // calculul indicelui din array corespunzator liniei vechi
            int subArrayIndex = i / subArraySize;
            if(subArrayIndex == newLanesCount) {
                subArrayIndex--;
            }
            // adaugare coada noua
            laneCoupling.get(subArrayIndex).add(new OldLane(i));
        }
    }

    public void enter(Car car) {

        // calcul indice pentru masina curenta
        int subArrayIndex = car.getStartDirection() / subArraySize;
        if(subArrayIndex == newLanesCount) {
            subArrayIndex--;
        }
        // adaugare masina in coada corespunzatoare
        ArrayList<OldLane> list = laneCoupling.get(subArrayIndex);
        for (OldLane lane : list) {
            if(lane.getOldLaneNumber() == car.getStartDirection()) {
                synchronized (this) {
                    System.out.println("Car " + car.getId() + " has come from the lane number " + car.getStartDirection());
                    lane.getQueue().add(car);
                }
            }
        }

        // asteptare pana cand toate masinile au ajuns la intersectie
        try {
            barrier.await();
        } catch (InterruptedException | BrokenBarrierException e) {
            e.printStackTrace();
        }

        // masina cu indicele 0 va porni alte thread-uri care vor executa partea de distribuire a masinilor pe noile benzi
        if(car.getId() == 0) {
            startThreads();
        }
    }

    // se creeaza atatea thread-uri cate benzi noi sunt
    private void startThreads() {
        NewLane[] threadArray = new NewLane[laneCoupling.size()];

        // se trimit la thread linia curenta, lista de cozi pentru linia curenta, numarul de masini care pot intra
        // simultan de pe o banda
        for(int i = 0; i < threadArray.length; i++) {
            threadArray[i] = new NewLane(i, laneCoupling.get(i), permits);
        }

        for(int i = 0; i < threadArray.length; i++) {
            threadArray[i].start();
        }

        for(int i = 0; i < threadArray.length; i++) {
            try {
                threadArray[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}

// retine informatii despre coada de masini venita de pe o linie veche (id-ul si coada in sine)
class OldLane {

    ArrayBlockingQueue<Car> queue;

    int oldLaneNumber;

    OldLane(int oldLane) {
        this.oldLaneNumber = oldLane;
        queue = new ArrayBlockingQueue<>(Main.carsNo);
    }

    public ArrayBlockingQueue<Car> getQueue() {
        return queue;
    }

    public int getOldLaneNumber() {
        return oldLaneNumber;
    }
}

// clasa care implementeaza tranzitia de pe benzile vechi pe banda noua curenta
class NewLane extends Thread {

    int laneId;
    ArrayList <OldLane> oldLaneArray;
    int permits;

    NewLane(int laneId, ArrayList<OldLane> array, int permits) {
        this.laneId = laneId;
        this.oldLaneArray = array;
        this.permits = permits;
    }

    @Override
    public void run() {

        // se repeta procesul pana cand toate benzile vechi sunt eliberate (array nu mai contine benzi vechi
        while(!oldLaneArray.isEmpty()) {

            // se extrage din array banda urmatoare din care vor pleca masini
            OldLane o = oldLaneArray.remove(0);
            ArrayBlockingQueue<Car> q = o.getQueue();

            // daca numarul de masini care pot trece este mai mare sau egal cu numarul de masini care asteapta,
            // toate masinile vor trece iar coada va fi eliminata din lista de cozi
            if(q.size() <= permits) {
                int size = q.size();
                for(int i = 0; i < size; i++) {
                    Car c = q.poll();
                    System.out.println("Car " + c.getId() + " from the lane " + o.getOldLaneNumber() + " has entered lane number " +
                            laneId);
                }

                System.out.println("The initial lane " + o.getOldLaneNumber() +
                        " has been emptied and removed from the new lane queue");
            }
            // daca numarul de masini care pot trece este mai mic, se scot primele permits masini si se introduce coada
            // cu masinile ramase la finalul listei de cozi
            else {
                for(int i = 0; i < permits; i++) {
                    Car c = q.poll();
                    System.out.println("Car " + c.getId() + " from the lane " + o.getOldLaneNumber() + " has entered lane number " +
                            laneId);
                }

                System.out.println("The initial lane " + o.getOldLaneNumber() +
                        " has no permits and is moved to the back of the new lane queue");
                oldLaneArray.add(o);
            }
        }
    }
}
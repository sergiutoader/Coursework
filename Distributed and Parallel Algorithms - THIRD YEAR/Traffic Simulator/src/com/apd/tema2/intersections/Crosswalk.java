package com.apd.tema2.intersections;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;
import com.apd.tema2.entities.Pedestrians;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class Crosswalk implements Intersection {
    /*
       starea anterioara a masinii
       0 - tocmai a intrat in intersectie
       1 - rosu
       2 - verde
     */
    private final List<Integer> lastState;

    // instanta a pietonilor
    private final Pedestrians pedestrians;

    public Crosswalk(int carsNo) {
        // se va folosi o lista sincronizata pentru stari, pentru a sincroniza cirile si scrierile
        lastState = Collections.synchronizedList(new ArrayList<>(carsNo));
        // initial masinile nu au avut anterior nicio culoare la semafor
        for(int i = 0; i < carsNo; i++) {
            lastState.add(0);
        }

        pedestrians = Main.pedestrians;
    }

    // metoda apelata atunci cand o masina intra in intersectie
    public void enter(Car car) {

        // bucla pana cand toti pietonii au trecut
        while(pedestrians.isFinished()) {
            // daca pietonii sunt pe trecere, se va aprinde culoarea rosie, iar masinile vor sta
            if(pedestrians.isPass()) {
                // afisare daca culoarea anterioara a fost verde
                if(lastState.get(car.getId()) == 2) {
                    lastState.set(car.getId(), 1);
                    System.out.println("Car " + car.getId() + " has now red light");
                }
            }
            // daca pietonii nu sunt pe trecere, iar culoarea anterioara a masinilor a fost verde, aceasta se va schimba
            // pe verde
            else {
                if(lastState.get(car.getId()) != 2) {
                    lastState.set(car.getId(), 2);
                    System.out.println("Car " + car.getId() + " has now green light");
                }
            }
        }
        // afisare pentru situatia in care isFinished se seteaza pe true simultan cu setarea isPass pe false
        if(lastState.get(car.getId()) != 2) {
            lastState.set(car.getId(), 2);
            System.out.println("Car " + car.getId() + " has now green light");
        }
    }
}

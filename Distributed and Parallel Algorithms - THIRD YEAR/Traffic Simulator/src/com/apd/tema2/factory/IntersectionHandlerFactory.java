package com.apd.tema2.factory;

import com.apd.tema2.intersections.*;
import com.apd.tema2.Main;
import com.apd.tema2.entities.*;
import com.apd.tema2.utils.Constants;

import static java.lang.Thread.sleep;

/**
 * Clasa Factory ce returneaza implementari ale InterfaceHandler sub forma unor
 * clase anonime.
 */
public class IntersectionHandlerFactory {

    public static IntersectionHandler getHandler(String handlerType) {
        return switch (handlerType) {

            case "simple_semaphore" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {

                    SimpleSemaphore intersection = (SimpleSemaphore)Main.intersection;

                    intersection.reachSemaphore(car);
                    intersection.waitAtSemaphore(car);
                    intersection.drive(car);
                }
            };
            case "simple_n_roundabout" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {

                    SimpleIntersection intersection = (SimpleIntersection)Main.intersection;

                    intersection.reach(car);
                    intersection.enter(car);
                    intersection.exit(car);
                }
            };
            case "simple_strict_1_car_roundabout" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {

                    SimpleStrictCarIntersection intersection = (SimpleStrictCarIntersection)Main.intersection;

                    intersection.reach(car, false);
                    intersection.enter(car);
                    intersection.exit(car, false);
                }
            };
            case "simple_strict_x_car_roundabout" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {

                    SimpleStrictCarIntersection intersection = (SimpleStrictCarIntersection)Main.intersection;

                    intersection.reach(car, true);
                    intersection.enter(car);
                    intersection.exit(car, true);
                }
            };

            case "simple_max_x_car_roundabout" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {

                    try {
                        sleep(car.getWaitingTime());
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    SimpleStrictCarIntersection intersection = (SimpleStrictCarIntersection)Main.intersection;

                    intersection.reachMax(car);
                    intersection.enterMax(car);
                    intersection.exitMax(car);
                }
            };

            case "priority_intersection" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {
                    try {
                        sleep(car.getWaitingTime());
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    PriorityIntersection intersection = (PriorityIntersection) Main.intersection;

                    // masinile de prioritate 1 intra in intersectie si ies in momentul in care este liber
                    if(car.getPriority() == 1) {
                        intersection.lowPriorityEnterIntersection(car);
                    }
                    // masinile de prioritate mai mare vor petrece un timp in intersectie
                    else {
                        // intrare
                        intersection.highPriorityEnterIntersection(car);

                        // traversare
                        try {
                            sleep(Constants.PRIORITY_INTERSECTION_PASSING);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }

                        // iesire
                        intersection.highPriorityExitIntersection(car);
                    }
                }
            };

            case "crosswalk" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {
                    Crosswalk intersection = (Crosswalk) Main.intersection;
                    intersection.enter(car);
                }
            };
            case "simple_maintenance" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {
                   SimpleMaintenance intersection = (SimpleMaintenance) Main.intersection;
                   intersection.enter(car);
                }
            };
            case "complex_maintenance" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {
                    ComplexMaintenance intersection = (ComplexMaintenance) Main.intersection;
                    intersection.enter(car);
                }
            };
            case "railroad" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {
                    Railroad intersection = (Railroad)Main.intersection;
                    intersection.enter(car);
                }
            };

            default -> null;
        };
    }
}

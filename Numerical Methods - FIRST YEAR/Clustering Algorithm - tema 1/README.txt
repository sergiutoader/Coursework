Toader Sergiu-Cristian, 315CB


Task 1:
Citesc din fisierele ale caror nume sunt date ca parametru in functie numarul
de centroizi si matricea de puncte folosind functia dlmread. Pentru citirea
matricei de puncte, folosesc parametrii de offset ai functiei dlmread pentru a
ignora primele 5 linii din fisier.

===============================================================================

Task 2:
Aleg pozitia initiala a centroizilor ca fiind pozitia primelor NC puncte.
Pentru fiecare punct, determin care centroid este mai aproape, calculand
minimul distantelor fata de centroizi si retin indicele grupului intermediar
din care face parte punctul (daca centroidul al 2-lea este cel mai apropiat
de punctul al 15-lea, atunci punctul va fi repartizat in grupul al 2-lea,
deci group_index(15) ia valoarea 2). In vectorul points_count_per_group sunt
retinute cate puncte sunt in fiecare grup. Coordonatele noilor centroizi sunt
calculate ca suma coordonatelor tuturor punctelor dintr - un  grup / numarul de
puncte din grupul respectiv. Procedeul se repeta pana cand coordonatele
centroizilor la pasul precedent (retinute in prev_centroids) sunt egale cu cele
de la pasul curent.

===============================================================================

Task 3:
Avand ca parametrii de intrare centroizii si punctele, determin cel mai
apropiat centroid de fiecare punct prin calcularea distantei minime, grupez
punctele identic cu metoda de la task-ul 2 si folosind functia scatter3 cu
parametrul de culoare egal cu indicele grupului din care face parte fiecare
punct, obtin graficul punctelor cu o culoare unica pentru fiecare grup.

===============================================================================

Task 4:
Avand ca parametrii de intrare centroizii si punctele, determin cel mai
apropiat centroid de fiecare punct prin calcularea distantei minime si adaug
acea distanta la variabila cost, care este initializata cu 0.

===============================================================================

Task 5:
Citesc matricea de puncte din fisier, iterez numarul de centroizi de la 1 la 10
si determin pozitia centroizilor apeland functia de la task-ul 2. Mai departe
determin cel mai apropiat centroid fata de fiecare punct si calculez costul
prin aceeasi metoda ca si la task-ul 4. Cele 10 valori ale costurilor obtinute
si retinute in vectorul cost sunt reprezentate grafic folosind functia plot.

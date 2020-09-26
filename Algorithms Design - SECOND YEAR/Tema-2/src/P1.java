import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Queue;
import java.util.Scanner;

public class P1 {
	public static final String INPUT_FILE = "p1.in";
	public static final String OUTPUT_FILE = "p1.out";
	public static final int NMAX = 100005;

	// Numar de noduri, numar de muchii,
	// numar de orase din care se incepe cautarea
	int n, m, k;

	// Orasele din care lorzii incep cautarile
	HashSet<Integer> startingCities;
	int[] blockedCities; // Orasele care vor fi blocate
	// Vecini
	@SuppressWarnings("unchecked")
	ArrayList<Integer>[] adj = new ArrayList[NMAX];

	// Citirea / parsarea inputului
	void readInput() {
		BufferedReader in = null;
		String[] args_1 = null; // n, m, k
		String[] args_2 = null; // orasele din care se incep cautarile
		String[] args_3 = null; // permutari

		// citirea primelor 3 linii
		try {
			in = new BufferedReader(new FileReader(INPUT_FILE));
			args_1 = in.readLine().split(" ");
			args_2 = in.readLine().split(" ");
			args_3 = in.readLine().split(" ");
		} catch (Exception e) {
			System.exit(0);
		}

		n = Integer.parseInt(args_1[0]);
		m = Integer.parseInt(args_1[1]);
		k = Integer.parseInt(args_1[2]);

		// construirea vectorului de orase de inceput
		startingCities = new HashSet<Integer>();
		for (int i = 1; i <= k; i++) {
			startingCities.add(Integer.parseInt(args_2[i - 1]));
		}

		// construiera permutarii oraselor blocate
		blockedCities = new int[n - 1];
		for (int i = 0; i < n - 1; i++) {
			blockedCities[i] = Integer.parseInt(args_3[i]);
		}

		for (int i = 1; i <= n; i++) {
			adj[i] = new ArrayList<>();
		}

		// citirea restului liniilor
		for (int i = 1; i <= m; i++) {
			String[] line = null;
			try {
				line = in.readLine().split(" ");
			} catch (Exception e) {
				System.exit(0);
			}

			int x = Integer.parseInt(line[0]);
			int y = Integer.parseInt(line[1]);
			adj[x].add(y);
			adj[y].add(x);
		}

		try {
			in.close();
		} catch (Exception e) {
			System.exit(0);
		}
	}

	// Functie ce realizeaza o parcurgere bfs din nodul 1
	// Complexitate: O(n + m) (Cautarea in HashSet se realizeaza in O(1))
	
	// Intoarce false daca un nod descoperit este printre nodurile de start
	// din care lorzii isi incep cautarile
	
	// Intoarce true daca in urma parcurgerii nu a fost gasit vreun nod de
	// start
	boolean bfs(int[] color) {

		// Coada in care se introduc nodurile vizitate
		Queue<Integer> q = new LinkedList<Integer>();
		q.add(1);

		while (q.size() != 0) {
			// Se extrage urmatorul nod din coada
			int u = q.poll();
			// Se parcurg toti vecinii sai
			for (int v : adj[u]) {
				// Daca nodul este nevizitat si este un nod de start -> false
				if (color[v] == 0) {
					if (startingCities.contains(v)) { 
						return false;
					}
					// Se marcheaza ca vizitat si adaugat in coada
					color[v] = 1;
					q.add(v);
				}
			}
			// Se marcheaza ca vizitat nodul extras din coada
			color[u] = 1;
		}
		return true;
	}

	// Metoda care blocheaza primele x orase
	// (Le marcheaza ca vizitate pentru a nu fi introduse niciodata in coada)
	void blockFirstXCities(int[] color, int x) {
		for (int i = 0; i < x; i++) {
			color[blockedCities[i]] = 1;
		}
		// Marcheaza restul oraselor cu 0
		for (int i = x; i < n - 1; i++) {
			color[blockedCities[i]] = 0;
		}
	}

	// Metoda care determina numarul minim de orase ce trebuie blocate
	// Se verifica folosind o parcurgere BFS din sursa daca nodurile de
	// start alea lorzilor sunt reachable
	//
	// Numarul minim de orase se determina printr-o cautare binara
	// Cautarea se realizeaza intr-un interval [start, end], mereu in mijlocul
	// acestui interval (mid = (start + end) / 2) 
	// Se incearca prima oara mid = K/2 si se actualizeaza variabilele
	// start si end (capetele intervalului de cautare) in functie de
	// rezultatul obtinut in urma BFS pana cand start si end devin egale
	// (intervalul de cautare s-a redus la un singura valoare mid = start = end)
	// 
	// Complexitate: O((log K)*(n + m)) (complexitatea cautarii binare *
	// complexitatea unei cautari BFS)

	int solve() {
		int[] color = new int[n + 1];
		color[1] = 1;
		// capetele intervalului si mijlocul
		int start = 0, end = n - 1;
		int mid;

		while (true) {
			// mid va fi la jumatatea intervalului
			mid = start + (end - start) / 2;
			// conditia de oprire
			if (start == end) {
				return mid;
			}
			// se blocheaza primele mid orase
			blockFirstXCities(color, mid);

			// se realizeaza o parcurgere BFS pe graful avand primele
			// mid orase blocate

			// Daca nu s-a ajuns in niciun oras sursa, se incearca
			// o valoare mai mica petru mid (end devine mid)
			// Daca a fost gasit un oras sursa, se incearca o valoare mai
			// mare pentru mid (start devine mid)
			int[] temp = Arrays.copyOf(color, color.length);
			if (bfs(temp) == true) {
				end = mid;
			} else {
				start = mid + 1;
			}
		}
	}

	// functie care afiseaza output-ul
	void writeOutput(int result) {
		PrintWriter wr = null;
		try {
			wr = new PrintWriter(new File(OUTPUT_FILE));
		} catch (Exception e) {
			System.exit(0);
		}

		wr.println(result);
		wr.close();
	}

	public static void main(String[] args) {
		P1 obj = new P1();
		obj.readInput();
		obj.writeOutput(obj.solve());
	}
}
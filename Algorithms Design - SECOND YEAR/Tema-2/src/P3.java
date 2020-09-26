import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.PriorityQueue;
import java.util.Scanner;
import java.util.Stack;

public class P3 {

	public static final String INPUT_FILE = "p3.in";
	public static final String OUTPUT_FILE = "p3.out";
	public static final int NMAX = 100005;

	// Clasa pentru o muchie (contine campuri pentru nod destinatie si cost)
	// Implementeaza interfata Comparable pentru a putea introduce elemente
	// de acest tip in coada de prioritate (crescator dupa cost)
	public class Edge implements Comparable<Edge> {
		public int node;
		public double cost;

		Edge(int _node, double _cost) {
			node = _node;
			cost = _cost;
		}

		// Muchiile se sorteaza crescator dupa cost
		@Override
		public int compareTo(Edge e) {
			if (this.cost < e.cost) {
				return 1;
			} else if (this.cost > e.cost) {
				return -1;
			}
			return 0;
		}
	}

	// Numar de noduri, numar de muchii
	int n, m;
	// Energia initiala
	double e;

	// Lista de liste de muchii (muchiile adiacente fiecarui nod din graf)
	@SuppressWarnings("unchecked")
	ArrayList<Edge>[] adj = new ArrayList[NMAX];
	// Stiva pentru salvarea drumului
	Stack<Integer> drum = new Stack<Integer>();
	// Vector de parinti
	int[] p;

	// Citirea / parsarea inputului
	void readInput() {
		BufferedReader in = null;
		String[] args = null;
		// Citirea primei linii
		try {
			in = new BufferedReader(new FileReader(INPUT_FILE));
			args = in.readLine().split(" ");
		} catch (Exception e) {
			System.exit(0);
		}

		n = Integer.parseInt(args[0]);
		m = Integer.parseInt(args[1]);
		e = Double.parseDouble(args[2]);

		p = new int[n + 1];

		for (int i = 1; i <= n; i++) {
			adj[i] = new ArrayList<>();
		}

		// Citirea restului liniilor
		for (int i = 1; i <= m; i++) {
			String[] line = null;
			try {
				line = in.readLine().split(" ");
			} catch (Exception e) {
				System.exit(0);
			}

			int x = Integer.parseInt(line[0]);
			int y = Integer.parseInt(line[1]);
			double w = Double.parseDouble(line[2]);
			adj[x].add(new Edge(y, w));
		}

		try {
			in.close();
		} catch (Exception e) {
			System.exit(0);
		}
	}

	// Functie care afiseaza output-ul
	void writeOutput(double result) {

		PrintWriter wr = null;
		try {
			wr = new PrintWriter(new File(OUTPUT_FILE));
		} catch (Exception e) {
			System.exit(0);
		}

		// Afisarea energiei
		wr.println(result);

		// Afisarea in ordinea corecta a drumului
		// (in ordinea inversa adaugarii pe stiva)
		while (drum.isEmpty() == false) {
			wr.print(drum.pop() + " ");
		}
		// Afisarea nodului destinatie ca ultim nod al drumului
		wr.println(n);

		wr.close();
	}

	// Metoda care intoarce procentul maxim de energie
	// Se va folosi algoritmul Dijkstra
	// Relaxarea va fi de fapt calcularea procentului ramas
	// din energie in urma parcurgerii muchiei
	// Se va urmari calcularea drumului in care procentul
	// de energie ramane cat mai mare (practic, calcularea unui drum maxim)
	double solve() {

		// d retine valorile energiei ramase in fiecare nod
		ArrayList<Double> d = new ArrayList<>();
		d.add(-1.0);

		// Initializare parinti si distante
		// 100% energie in 0
		d.add(1.0);
		p[1] = -1;
		// 0% energie in rest
		for (int i = 2; i <= n; i++) {
			d.add(0.0);
			p[i] = -1;
		}

		// Setul va contine nodurile deja vizitate
		HashSet<Integer> visited = new HashSet<Integer>();

		// Coada de prioritate
		PriorityQueue<Edge> q = new PriorityQueue<Edge>();
		q.add(new Edge(1, 1.0));

		// Cat timp coada mai contine muchii
		while (q.isEmpty() == false) {
			// Se extrage nodul destinatie al primei muchii
			// si se adauga in setul de noduri vizitate
			Edge new_edge = q.poll();
			int u = new_edge.node;
			visited.add(u);

			// Se incearca relaxarea tuturor muchiilor adiacente nodului
			for (Edge edge : adj[u]) {
				int nod = edge.node;
				double w_u_nod = edge.cost;
				if (visited.contains(nod) == false && d.get(nod) < d.get(u) * (1 - w_u_nod / 100)) {
					// Setarea noii valori maxime a energiei
					d.set(nod, d.get(u) * (1 - w_u_nod / 100));
					// Salvarea parintelui
					p[nod] = u;
					// Adaugarea noii muchii in coada
					q.add(new Edge(nod, d.get(u) * (1 - w_u_nod / 100)));
				}
			}
		}
		// Adaugarea pe stiva a drumului pana la nod
		// Se foloseste o stiva deoarece parcurgem
		// drumul dinspre destinatie spre sursa,
		// astfel, cand vom extrage elementele din stiva,
		// acestea vor fi in ordinea corecta
		int nod = p[n];
		while (nod != -1) {
			drum.push(nod);
			nod = p[nod];
		}
		// Rezultatul va fi procentul maxim inmultit cu
		// valoarea initiala a energiei
		return d.get(n) * e;
	}

	public static void main(String[] args) {
		P3 obj = new P3();
		obj.readInput();
		obj.writeOutput(obj.solve());
	}
}
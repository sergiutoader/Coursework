import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Scanner;

public class P2 {
	public static final String INPUT_FILE = "p2.in";
	public static final String OUTPUT_FILE = "p2.out";
	public static final int NMAX = 100005;
	// Valoare pentru Infinit (toate costurile muchiilor
	// din graf vor fi mai mici)
	public static final int Infinity = 123456789;
	// Numar de noduri, numar de muchii
	int n, m;
	// Sursa si destinatie
	int src, dest;

	// Clasa pentru o muchie (contine campuri pentru nod destinatie si cost)
	public class Edge {
		public int node;
		public int cost;

		Edge(int node, int cost) {
			this.node = node;
			this.cost = cost;
		}
	}

	// Lista de liste de muchii (muchiile adiacente fiecarui nod din graf)
	@SuppressWarnings("unchecked")
	ArrayList<Edge>[] adj = new ArrayList[NMAX];

	// Citirea / parsarea inputului
	void readInput() {
		BufferedReader in = null;
		String[] args_1 = null;
		String[] args_2 = null;
		// Citirea primelor 2 linii
		try {
			in = new BufferedReader(new FileReader(INPUT_FILE));
			args_1 = in.readLine().split(" ");
			args_2 = in.readLine().split(" ");
		} catch (Exception e) {
			System.exit(0);
		}

		n = Integer.parseInt(args_1[0]);
		m = Integer.parseInt(args_1[1]);
		src = Integer.parseInt(args_2[0]);
		dest = Integer.parseInt(args_2[1]);

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
			int w = Integer.parseInt(line[2]);
			adj[x].add(new Edge(y, w));
		}

		try {
			in.close();
		} catch (Exception e) {
			System.exit(0);
		}
	}

	// Functie care afiseaza output-ul
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

	// Functie care intoarce o lista cu nodurile in ordinea sortarii topologice
	ArrayList<Integer> topoSort() {
		// Vectorul rezultat (in final contine o permutare pentru 1, 2, ... n)
		ArrayList<Integer> topsort = new ArrayList<>();

		boolean[] visited = new boolean[n + 1];

		// Pentru fiecare nod
		for (int i = 1; i <= n; i++) {
			// Daca nodul nu a fost vizitat, pornim o parcurgere DFS
			if (!visited[i]) {
				dfs(i, visited, topsort);
			}
		}

		// Rezultatul a fost obtinut in ordine inversa
		Collections.reverse(topsort);

		return topsort;
	}

	// Porneste o parcurgere DFS din node
	// Foloseste vectorul visited pentru a marca nodurile vizitate
	void dfs(int node, boolean[] visited, ArrayList<Integer> topsort) {
		visited[node] = true;

		for (Edge e : adj[node]) {
			int v = e.node;
			if (!visited[v]) {
				dfs(v, visited, topsort);
			}
		}

		// Dupa ce s-a incheiat vizitarea nodului, adaugam nodul
		// in sortarea topologica
		topsort.add(node);
	}

	int solve() {
		// Vectorul d va retine distantele de la fiecare nod la sursa
		// Initializare distante fata de nodul sursa:
		// 0 pentru sursa
		// Infinit pentru restul nodurilor
		int[] d = new int[n + 1];
		for (int i = 0; i <= n; i++) {
			if (i != src) {
				d[i] = Infinity;
			}
		}

		// Sortare topologica a nodurilor
		ArrayList<Integer> sorted = topoSort();

		// Parcurgand nodurile in ordine topologica, se realizeaza o singura
		// data relaxarea muchiilor din graf
		for (int u : sorted) {
			for (Edge edge : adj[u]) {
				int v = edge.node;
				int w_uv = edge.cost;
				if (d[v] > d[u] + w_uv) {
					d[v] = d[u] + w_uv;
				}
			}
		}
		// Rezultatul va fi memorat in d[dest]
		return d[dest];
	}

	public static void main(String[] args) {
		P2 obj = new P2();
		obj.readInput();
		obj.writeOutput(obj.solve());
	}
}
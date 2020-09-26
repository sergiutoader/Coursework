import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;

// Clasa care stocheza marginile intervalului in care se afla numarul
// de bomboane pe care trebuie sa le primeasca un elev
class Interval {
	int start;
	int end;

	Interval(int start, int end) {
		this.start = start;
		this.end = end;
	}
}

public class Bomboane {

	int n; // numar de elevi
	int m; // bomboane

	// intervalele
	Interval[] intervale;

	private static final int MOD = 1000000007;

	void readInput() throws IOException {
		BufferedReader in = null;
		String[] args = null;
		try {
			in = new BufferedReader(new FileReader("bomboane.in"));
			args = in.readLine().split(" ");
		} catch (Exception e) {
			System.exit(0);
		}

		n = Integer.parseInt(args[0]);
		m = Integer.parseInt(args[1]);
		intervale = new Interval[n];

		for (int i = 0; i < n; i++) {
			String[] words = in.readLine().split(" ");
			intervale[i] = new Interval(Integer.parseInt(words[0]), Integer.parseInt(words[1]));
		}

		try {
			in.close();
		} catch (Exception e) {
			System.exit(0);
		}
	}

	// Metoda care calculeaza rezultatul pe baza input-ului
	int solve() {

		// matrice de (n+1) * (m+1)
		// liniila i reprezinta primii i elevi
		// coloana j reprezinta j bomboane
		// astfel dp[i][j] reprezinta in cate moduri se pot imparti j bomboane la i
		// elevi
		int[][] dp = new int[n + 1][m + 1];

		// initializare linia 1 - primul elev va avea 1 mod de a primi bomboane
		// daca acel numar de bomboane se afla in intervalul elevului respectiv, si in
		// rest 0
		for (int j = 0; j < m + 1; j++) {
			if (j >= intervale[0].start && j <= intervale[0].end) {
				dp[1][j] = 1;
			}
		}

		// pentru restul de linii, numarul de moduri in care se pot imparti j bomboane
		// la
		// i elevi reprezinta suma modurilor in care se pot imparti la i-1 elevi
		// bomboane in numar care variaza de la j minus marginea superioara a
		// intervalului
		// la j minus marginea inferioara a intervalului elevului curent, cu conditia ca
		// aceste valori sa nu fie negative (j-y >=0)
		for (int i = 2; i <= n; i++) {
			int x = intervale[i - 1].start;
			int y = intervale[i - 1].end;
			for (int j = 0; j <= m; j++) {
				for (int k = j - y; k <= j - x; k++) {
					if (k >= 0) {
						dp[i][j] = (dp[i][j] % MOD + dp[i - 1][k] % MOD) % MOD;
					}

				}
			}
		}

		return dp[n][m];
	}

	void writeOutput(int result) {
		PrintWriter wr = null;
		try {
			wr = new PrintWriter(new File("bomboane.out"));
		} catch (Exception e) {
			System.exit(0);
		}

		wr.println(result);
		wr.close();
	}

	public static void main(String[] args) throws IOException {
		Bomboane obj = new Bomboane();
		obj.readInput();
		obj.writeOutput(obj.solve());
	}
}
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;

public class Bani {
	// Variabile pentru tip si numarul de bancnote
	int type;
	int n;

	private static final int MOD = 1000000007;

	void readInput() {
		BufferedReader in = null;
		String line = "";
		try {
			in = new BufferedReader(new FileReader("bani.in"));
			line = in.readLine();
		} catch (Exception e) {
			System.exit(0);
		}

		type = Integer.parseInt(line.substring(0, 1));
		n = Integer.parseInt(line.substring(2));

		try {
			in.close();
		} catch (Exception e) {
			System.exit(0);
		}
	}

	private long fastPow(long base, long exponent) {

		if (exponent == 0) {
			return 1;
		}
		if (exponent == 1) {
			return base % MOD;
		}
		// recursiv b^(exp/2)
		long temp = fastPow(base, exponent / 2);
		// restul impartirii lui (b^(exp/2))^2;
		temp = ((temp % MOD) * (temp % MOD)) % MOD; 
		if (exponent % 2 == 1) {
			// daca exponentul e impar se mai inmulteste cu base
			temp = ((temp % MOD) * (base % MOD)) % MOD;
		}

		return temp;
	}

	// Metoda care calculeaza rezultatul pe baza input-ului
	long solve() {

		// Pentru tipul 1, numarul de aranjari posibile este 5 * 2^(n-1) care se
		// poate calcula prin exponentiere logaritmica in O(logn)
		// 5 reprezinta numarul de aranjari pentru un teanc de 1 bancnota
		if (type == 1) {
			long dp = 5L * fastPow(2L, 1L * (n - 1));
			return dp % MOD;
		} else {
			// Pentru tipul 2, numarul de aranjari posibile se determina dinamic
			int[][] dp = new int[5][n];
			for (int i = 0; i < 5; i++) {
				dp[i][0] = 1;
			}

			// Se iau in acest caz fiecare bancnota separat, formandu-se o matrice
			// O(n) spatial si temporal (Matricea este de dimensiuni 5*n)
			for (int i = 1; i < n; i++) {
				// Dupa 10 urmeaza 50 sau 100
				dp[0][i] = (dp[1][i - 1] % MOD + dp[2][i - 1] % MOD) % MOD; // 10
				// Dupa 50 urmeaza 10 sau 200
				dp[1][i] = (dp[0][i - 1] % MOD + dp[3][i - 1] % MOD) % MOD; // 50
				// Dupa 100 urmeaza 10 sau 100
				dp[2][i] = (dp[0][i - 1] % MOD + dp[2][i - 1] % MOD) % MOD; // 100
				// Dupa 200 urmeaza 50 sau 100 sau 500
				dp[3][i] = ((dp[1][i - 1] % MOD + dp[2][i - 1] % MOD) % MOD
					+ dp[4][i - 1] % MOD) % MOD; // 200
				// Dupa 500 urmeaza 10 sau 200
				dp[4][i] = (dp[0][i - 1] % MOD + dp[3][i - 1] % MOD) % MOD; // 500
			}

			// Rezultatul este suma ultimei coloane
			int sum = 0;
			for (int i = 0; i < 5; i++) {
				sum = (sum % MOD + dp[i][n - 1] % MOD) % MOD;
			}

			return (long) sum;
		}
	}

	void writeOutput(long result) {
		PrintWriter wr = null;
		try {
			wr = new PrintWriter(new File("bani.out"));
		} catch (Exception e) {
			System.exit(0);
		}

		wr.println(result);
		wr.close();
	}

	public static void main(String[] args) {
		Bani obj = new Bani();
		obj.readInput();
		obj.writeOutput(obj.solve());
	}
}
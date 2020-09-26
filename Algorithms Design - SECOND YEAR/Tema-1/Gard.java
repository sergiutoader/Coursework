import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Arrays;

class Bucata implements Comparable<Bucata> {
	int start;
	int end;

	public Bucata(int start, int end) {
		this.start = start;
		this.end = end;
	}

	// sortarea se face descrescator dupa end si, in caz de egalitate,
	// crescator dupa start
	@Override
	public int compareTo(Bucata b) {
		if (this.end != b.end) {
			return b.end - this.end;
		} else {
			return this.start - b.start;
		}

	}
}

public class Gard {

	int n;
	Bucata[] bucati;

	private static final int MOD = 1000000007;

	void readInput() throws IOException {
		BufferedReader in = null;
		String firstLine = "";

		in = new BufferedReader(new FileReader("gard.in"));
		n = Integer.parseInt(in.readLine());

		bucati = new Bucata[n];
		for (int i = 0; i < n; i++) {
			String[] args = in.readLine().split(" ");
			bucati[i] = new Bucata(Integer.parseInt(args[0]), Integer.parseInt(args[1]));
		}

		in.close();
	}

	// Metoda care calculeaza rezultatul pe baza input-ului
	int solve() {

		int count = 0;

		// Sortare vector de bucati de gard ( O(n*log(n)))
		Arrays.sort(bucati);

		Bucata curr = bucati[0];
		// Daca urmatoarea bucata are indicele de start mai mare decat piesa curenta
		// Aceasta este redundanta, altfel, ea devine urmatoarea piesa curenta
		// O(n)
		for (int i = 1; i < n; i++) {
			if (bucati[i].start >= curr.start) {
				count++;
			} else {
				curr = bucati[i];
			}
		}
		// O(n*logn) temporal, O(n) spatial

		return count;
	}

	void writeOutput(int result) {
		PrintWriter wr = null;
		try {
			wr = new PrintWriter(new File("gard.out"));
		} catch (Exception e) {
			System.exit(0);
		}

		wr.println(result);
		wr.close();
	}

	public static void main(String[] args) throws IOException {
		Gard obj = new Gard();
		obj.readInput();
		obj.writeOutput(obj.solve());

	}
}
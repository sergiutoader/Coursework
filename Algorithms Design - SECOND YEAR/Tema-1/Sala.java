import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.PriorityQueue;
import java.util.Arrays;

class Gantera implements Comparable <Gantera> {
	long greutate;
	long nrRepetari;
	long scor;

	Gantera(long greutate, long nrRepetari) {
		this.greutate = greutate;
		this.nrRepetari = nrRepetari;
		this.scor = greutate * nrRepetari;
	}

	// sortare descrescator dupa produsul 
	@Override
	public int compareTo(Gantera g){
		if(g.greutate > this.greutate) {
			return 1;
		}
		else if (g.greutate == this.greutate) {
			return 0;
		}
		return -1;
	}
}

public class Sala {

	int n; // numar total de gantere
	int m; // numar de gantere folosite

	Gantera[] gantere;

	void readInput() throws IOException {
		BufferedReader in = null;
		String [] args = null;
		try {
			in = new BufferedReader(new FileReader("sala.in"));
			args = in.readLine().split(" ");	
		} catch (Exception e) {
			System.exit(0);
		}

		n = Integer.parseInt(args[0]);
		m = Integer.parseInt(args[1]);
		gantere = new Gantera[n];

		for (int i = 0; i < n; i++) {
			String [] words = in.readLine().split(" ");
			gantere[i] = new Gantera(Integer.parseInt(words[0]), Integer.parseInt(words[1]));
		}

		try {
			in.close();
		} catch (Exception e) {
			System.exit(0);
		}
	}

	long solve() {

		// sortare descrescator dupa greutate
		Arrays.sort(gantere);
	
		
		// dp retine pe pozitia <greuatea elementului i> numarul total de repetari la pasul i		
		long[] dp = new long[(int)(gantere[0].greutate) + 1];

		// repetari retine multimea de valori pentru repetari la pasul i
		 PriorityQueue<Long> repetari = new PriorityQueue<Long>(m); 
		 for(int i = 0; i < m-1; i++) {
		 	repetari.add(0L);
		 }

		// dp are pe pozitia primei greutati numarul de repetari al primului element
		dp[(int)gantere[0].greutate] = gantere[0].nrRepetari;
		// repetari are pe prima pozitie numarul de repetari al primului element
		repetari.add(gantere[0].nrRepetari);

		// max initializat cu rezultatul pentru prima gantera
		long max = gantere[0].nrRepetari * gantere[0].greutate;

		long sumMaxRepetari = dp[(int)gantere[0].greutate]; // variabila care retine valoarea maxima din dp

		for(int i = 1; i < n; i++) {
			// daca pe pozitia curenta am un nr mai mare de repetari decat minimul de repetari de la maximul din dp, verific daca
			// prin inlocuirea numarului minim de repetari de la cazul de maxim cu repetarile de la pasul curent
			// obtin un rezultat mai bun
			if(gantere[i].nrRepetari > repetari.peek()) {

				// dp la pasul urmator va fi suma maxima la care am inlocuit numarul minim de repetari cu numarul curent
				dp[(int)gantere[i].greutate] = gantere[i].nrRepetari - repetari.poll() + sumMaxRepetari;

				// modificare vector repetari
				repetari.add(gantere[i].nrRepetari);
				sumMaxRepetari = dp[(int)gantere[i].greutate];

		
			} 
			// verific daca am obtinut un rezultat mai bun cu aceasta valoare
			if(max < dp[(int)gantere[i].greutate] * gantere[i].greutate) {
				max = dp[(int)gantere[i].greutate] * gantere[i].greutate;
			} 
		}

		return max;
	}

	void writeOutput(long result) {
		PrintWriter wr = null;
		try {
			wr = new PrintWriter(new File("sala.out"));
		} catch (Exception e) {
			System.exit(0);
		}

		wr.println(result);
		wr.close();
	}

	public static void main(String[] args) throws IOException {
		Sala obj = new Sala();
		obj.readInput();
		obj.writeOutput(obj.solve());
	}
}
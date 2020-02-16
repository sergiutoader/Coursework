/* TOADER Sergiu-Cristian - 325CB */

import java.io.File;
import java.io.PrintWriter;
import java.util.Scanner;

/**
 * Clasa main. Realizeaza instantierea heap-ului, fisierelor de intrare si
 * iesire, citirea, executarea operatiilor si scrierea in fisierul de output
 *
 */
public class Main {

	public static void main(String[] args) {

		Scanner input = null;
		MaxHeap heap = new MaxHeap();
		// Instantiere scanner
		try {
			input = new Scanner(new File("queue.in"));
		} catch (Exception e) {
			System.err.println("Input file not found");
		}

		int nrPasageri = input.nextInt();
		// Instantiere obiect care retine datele de intrare
		DateIntrare date = new DateIntrare(nrPasageri);
		// Citire date de intrare
		for (int i = 0; i < nrPasageri; i++) {
			date.citireDate(input, i);
			date.adaugaPasager(i);
		}

		try {
			PrintWriter writer = new PrintWriter("queue.out");
			OperationHandler handler = new OperationHandler();
			handler.executaOperatii(input, date, writer, heap);
		} catch (Exception e) {
			System.err.println("Cannot create output file");
		}
	}
}

/* TOADER Sergiu-Cristian - 325CB */

import java.io.PrintWriter;
import java.util.Scanner;

/**
 * Clasa care implementeaza metoda executaOperatii, folosita in main
 */
public class OperationHandler {

	/**
	 * Realizeaza operatiile de insert, list, embark si delete din main
	 *
	 * @param input  Scannerul cu care se citeste din fisierul de input
	 * @param date   Obiect care retine datele de intrare citite in prima parte a
	 *               inputului
	 * @param writer Obiect de scriere in fisierul de output
	 * @param heap   Coada de prioritate
	 */
	public void executaOperatii(Scanner input, DateIntrare date, PrintWriter writer, MaxHeap heap) {

		while (input.hasNextLine()) {
			String command = input.nextLine();

			// Comanda insert
			if (command.length() >= 6 && command.substring(0, 6).equals("insert")) {

				// Determinare indice
				String parametru = command.substring(7);
				int idx = Integer.parseInt(parametru.substring(1)) - 1;

				// Inserare in functie de tipul de entitate
				switch (parametru.charAt(0)) {
				case 'g':
					heap.insert(date.getGrup(idx), date.getGrup(idx).getPunctaj());
					break;
				case 'f':
					heap.insert(date.getFamilie(idx), date.getFamilie(idx).getPunctaj());
					break;
				case 's':
					heap.insert(date.getPasagerSingur(idx), date.getPasagerSingur(idx).getPunctaj());
				}
			} else if (command.equals("embark")) {
				heap.embark();
			} else if (command.equals("list")) {
				writer.println(heap.list());
			}
			// Comanda delete
			else if (command.length() >= 6 && command.substring(0, 6).equals("delete")) {

				// Impartirea parametrilor operatiilor delete folosind metoda split din clasa
				// String
				String[] parametriDelete = command.split(" ");
				// Determinare index
				int idx = Integer.parseInt(parametriDelete[1].substring(1)) - 1;

				if (parametriDelete.length == 2) {
					// Daca nu exista parametru pentru nume, se executa metoda de eliminare din heap
					// a intregii entitati
					switch (parametriDelete[1].charAt(0)) {
					case 'g':
						heap.delete(date.getGrup(idx));
						break;
					case 'f':
						heap.delete(date.getFamilie(idx));
						break;
					default:
						heap.delete(date.getPasagerSingur(idx));
					}
				} else if (parametriDelete.length == 3) {
					// Daca exista parametru pentru nume se elimina pasagerul din entitate
					switch (parametriDelete[1].charAt(0)) {
					case 'g':
						date.getGrup(idx).setDeletePasager(true);
						date.getGrup(idx).setNumePasagerSters(parametriDelete[2]);

						heap.delete(date.getGrup(idx));

						date.getGrup(idx).setDeletePasager(false);
						date.getGrup(idx).setNumePasagerSters(null);
						break;
					case 'f':
						date.getFamilie(idx).setDeletePasager(true);
						date.getFamilie(idx).setNumePasagerSters(parametriDelete[2]);

						heap.delete(date.getFamilie(idx));

						date.getFamilie(idx).setDeletePasager(false);
						date.getFamilie(idx).setNumePasagerSters(null);
						break;
					default:
						date.getPasagerSingur(idx).setDeletePasager(true);
						date.getPasagerSingur(idx).setNumePasagerSters(parametriDelete[2]);

						heap.delete(date.getPasagerSingur(idx));
						
						date.getPasagerSingur(idx).setDeletePasager(false);
						date.getPasagerSingur(idx).setNumePasagerSters(null);
					}
				}

			}
		}
		writer.close();
	}

}

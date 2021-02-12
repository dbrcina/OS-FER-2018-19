package hr.fer.os.lab4;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Scanner;
import java.util.Set;
import java.util.TreeSet;

public class Simulacija {

	private String[] znakovi;
	private int slobodnaMjesta;

	public Simulacija(int size) {
		this.znakovi = new String[size];
		postaviPolje(znakovi);
		this.slobodnaMjesta = znakovi.length;
	}

	public int getSize() {
		return znakovi.length;
	}

	public String[] getZnakovi() {
		return znakovi;
	}

	public int getSlobodnaMjesta() {
		return slobodnaMjesta;
	}

	public void setZahtjev(Zahtjev z) {
		List<Rupa> rupe = new ArrayList<>();
		Set<Rupa> rupeOdg = new TreeSet<>();
		int j = 0;
		for (int i = 0; i < znakovi.length; i++) {
			if (znakovi[i].equals("-")) {
				while (znakovi[i + j].equals("-") && (i + j) < znakovi.length - 1) {
					j++;
				}
				if (i + j >= znakovi.length - 1) {
					j++;
				}
				rupe.add(new Rupa(i, j));
				if (j == slobodnaMjesta) {
					break;
				}
				i += j;
				j = 0;
			}
		}
		for (Rupa r : rupe) {
			if (r.getLen() >= z.getCapacity()) {
				rupeOdg.add(r);
			}
		}
		if (!rupeOdg.isEmpty()) {
			for (int i = 0; i < z.getCapacity(); i++) {
				Iterator<Rupa> it = rupeOdg.iterator();
				znakovi[it.next().getIndex() + i] = z.getId();
				slobodnaMjesta--;
			}
		} else {
			defragmentacija(znakovi);
			System.out.println("Spremnik nakon defragmentacije");
			for (int i = 0; i < znakovi.length; i++) {
				if (znakovi[i].equals("-")) {
					for (j = 0; j < z.getCapacity(); j++) {
						znakovi[i + j] = z.getId();
						slobodnaMjesta--;
					}
					break;
				}
			}
		}
	}

	public void oslobodiZahtjev(Zahtjev z) {
		boolean osloboden = true;
		for (int i = 0; i < znakovi.length; i++) {
			if (znakovi[i].equals(z.getId())) {
				for (int j = 0; j < z.getCapacity(); j++) {
					znakovi[i + j] = "-";
					slobodnaMjesta++;
				}
				break;
			}
		}
		System.out.printf("\nZahtjev %s %s osloboden.\n", z.getId(), osloboden ? "je" : "nije");
	}

	private static void defragmentacija(String[] znakovi) {
		System.out.println("\nDefragmentiram spremnik!");
		for (int i = 0; i < znakovi.length - 1; i++) {
			for (int j = 0; j < znakovi.length - 1 - i; j++) {
				if (znakovi[j].equals("-") && !znakovi[j + 1].equals("-")) {
					String pom = znakovi[j];
					znakovi[j] = znakovi[j + 1];
					znakovi[j + 1] = pom;
				}
			}
		}
		for (int i = 0; i < znakovi.length; i++) {
			System.out.printf("%2d ",i);
		}
		System.out.println();
		for (int i = 0; i <znakovi.length; i++) {
			System.out.printf("%2s ",znakovi[i]);
		}
		System.out.println("\n");
	}
	private static void postaviPolje(String[] znakovi) {
		for (int i = 0, n = znakovi.length; i < n; i++) {
			znakovi[i] = "-";
		}
	}

	private static class Rupa implements Comparable<Rupa> {
		private Integer atIndex;
		private Integer len;

		public Rupa(int atIndex, int len) {
			this.atIndex = atIndex;
			this.len = len;
		}

		public int getLen() {
			return len;
		}

		public int getIndex() {
			return atIndex;
		}

		@Override
		public int compareTo(Rupa o) {
			return this.len.compareTo(o.getLen());
		}

	}

	private static class Zahtjev {
		private final String id;
		private int capacity;

		public Zahtjev(String id, int capacity) {
			this.id = id;
			this.capacity = capacity;
		}

		public int getCapacity() {
			return capacity;
		}

		public String getId() {
			return id;
		}
	}

	public static void main(String[] args) {
		if (args.length != 1) {
			System.err.println("Unesen je krivi broj parametara preko komandnog retka!");
		}
		Simulacija s = new Simulacija(Integer.parseInt(args[0]));
		pokreniSimulaciju(s);
	}

	public static void pokreniSimulaciju(Simulacija s) {
		System.out.println("Zapo�eta je simulacija sa " + s.getSize() + " memorije.");
		ispis(s);
		System.out.println("Za novi zahtjev pritisni Z, za osloba�anje memorije O i KRAJ za prekid simulacije." + "\n");
		try (Scanner sc = new Scanner(System.in)) {
			Integer id = 0;
			List<Zahtjev> zahtjevi = new ArrayList<>();
			while (sc.hasNext()) {
				String unos = sc.next();
				if (unos.equals("KRAJ") || unos.equals("kraj")) {
					System.out.println("\nZavr�ena je simulacija!");
					System.exit(0);
				}
				if (unos.equals("Z") || unos.equals("z")) {
					int capacity = (int) Math.ceil(Math.random() * 5 + 1);
					Zahtjev z = new Zahtjev(id.toString(), capacity);
					System.out.println(
							"\nGeneriran je zahtjev " + z.getId() + " za " + z.getCapacity() + " spremni�ka mjesta.");
					if (s.getSlobodnaMjesta() >= z.getCapacity()) {
						s.setZahtjev(z);
						ispis(s);
						zahtjevi.add(z);
					} else {
						System.out.println("Nema dovoljno memorije za zahtjev " + z.getId() + ".\n");
					}
					id++;
				} else if (unos.equals("O") || unos.equals("o")) {
					System.out.println("\nKoji zahtjev treba osloboditi?\n");
					Integer index = sc.nextInt();
					for (int i = 0, n = zahtjevi.size(); i < n; i++) {
						Zahtjev z = zahtjevi.get(i);
						if (z.getId().equals(index.toString())) {
							s.oslobodiZahtjev(z);
							zahtjevi.remove(i);
							break;
						}
					}
					ispis(s);
				} else {
					System.err.println("\nUneseno je nesto krivo! Ponovi postupak!\n");
				}
			}
		}
	}

	public static void ispis(Simulacija s) {
		int size = s.getZnakovi().length;
		for (int i = 0; i < size; i++) {
			System.out.printf("%2d ", i);
		}
		System.out.println();
		for (int i = 0; i < size; i++) {
			System.out.printf("%2s ", s.getZnakovi()[i]);
		}
		System.out.println("\n");
	}

}

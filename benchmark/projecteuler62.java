import java.util.*;

class ProjectEuler62 {

	public static void main(String[] args) {
		long t = System.nanoTime();
		long res = code();
		long e = System.nanoTime() - t;
		System.out.println(res);
		System.out.println("Time: " + (e / 1000000) + "ms");
	}

	private static boolean isPermutation(String str1, String str2) {
		if (str1.length() != str2.length()) return false;
		char[] a = str1.toCharArray();
		char[] b = str2.toCharArray();
		Arrays.sort(a);
		Arrays.sort(b);
		return Arrays.equals(a, b);
	}

	private static long code() {

		List<String> singles = new ArrayList<String>();
		List<List<String>> groups = new ArrayList<List<String>>();
		int i = 405;
		int l = 8;

		while (true) {
			i++;

		    String n = "" + (long) Math.pow(i, 3);
			if (n.length() > l) {
				l++;
				singles.clear();
			}
		    boolean added = false;

		    for (List<String> g : groups) {
		        if (isPermutation(g.get(0), n)) {
					g.add(n);
		            added = true;
		            if (g.size() == 5) {
		            	return Long.parseLong(g.get(0));
		            }
		        }
		    }
			for (int j = 0; j < singles.size(); ++j) {
				if (isPermutation(singles.get(j), n)) {
					List<String> newg = new ArrayList<String>();
					newg.add(singles.get(j));
					newg.add(n);
					groups.add(newg);
					singles.remove(j);
					added = true;
				}
			}
		    if (!added) {
				singles.add(n);
		    }
		}
	}
}

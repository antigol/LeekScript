import java.util.*;

class cubic_permutations {

	public static void main(String[] args) {
		long s = 0;
		for (int i = 0; i < 60; ++i) {
			s += code(i);
		}
		System.out.println(s);
	}

	public static long code(int p) {
		Map<String, Integer> groups = new TreeMap<String, Integer>();
		Map<String, String> first = new TreeMap<String, String>();
		int i = 1 + p;

		while (i++ > 0) {

		    String n = "" + (long) Math.pow(i, 3);
			char[] ar = n.toCharArray();
			Arrays.sort(ar);
			String ns = String.valueOf(ar);

			if (groups.containsKey(ns)) {
				groups.put(ns, groups.get(ns) + 1);
				if (groups.get(ns) == 5) {
					return Long.parseLong(first.get(ns));
				}
			} else {
				groups.put(ns, 1);
				first.put(ns, n);
			}
		}
		return 0;
	}
}

import java.util.Arrays;
import java.util.Random;

public class branch_prediction {
    public static void main(String[] args) {

        int arraySize = 32768;
        int data[] = new int[arraySize];
        Random rnd = new Random(0);
        for (int c = 0; c < arraySize; ++c) {
            data[c] = rnd.nextInt() % 256;
		}
        Arrays.sort(data);

        long sum = 0;

        for (int i = 0; i < 70000; ++i) {
            for (int c = 0; c < arraySize; ++c) {
                if (data[c] >= 128)
                    sum += data[c];
            }
        }
        System.out.println(sum);
    }
}

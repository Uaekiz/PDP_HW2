import java.util.*;
import java.io.*;

public class MatrixBenchmark {

    static final String FILE_PATH = "datasets/valid_xxlarge.txt"; // TEST ETMEK ICIN BURAYI DEGISTIR

    // ===================== DOSYA OKUMA VE DEMO =====================

    static int[][] readMatrix(BufferedReader br) throws Exception {
        String line;
        while ((line = br.readLine()) != null && line.trim().isEmpty()) {}
        if (line == null) throw new Exception("Hata: Beklenen matris bulunamadi (EOF).");

        String[] dims = line.trim().split("\\s+");
        if (dims.length != 2) throw new Exception("Hata: Gecersiz matris boyutlari.");
        
        int rows, cols;
        try {
            rows = Integer.parseInt(dims[0]);
            cols = Integer.parseInt(dims[1]);
        } catch (NumberFormatException e) {
            throw new Exception("Hata: Boyutlar sayisal degil.");
        }

        int[][] matrix = new int[rows][cols];
        for (int i = 0; i < rows; i++) {
            line = br.readLine();
            if (line == null) throw new Exception("Hata: Satir verisi eksik.");
            
            String[] elements = line.trim().split("\\s+");
            if (elements.length == 1 && elements[0].isEmpty()) elements = new String[0];

            if (elements.length < cols) throw new Exception("Hata: Satirda eksik eleman (Missing elements in row).");
            if (elements.length > cols) throw new Exception("Hata: Satirda fazla eleman (Extra elements in row).");
            
            for (int j = 0; j < cols; j++) {
                try {
                    matrix[i][j] = Integer.parseInt(elements[j]);
                } catch (NumberFormatException e) {
                    throw new Exception("Hata: Sayisal olmayan deger tespit edildi (Non-numeric value detected).");
                }
            }
        }
        return matrix;
    }

    static boolean runFileDemo(String filePath) {
        System.out.println("=== DEMO ASAMASI (DOSYA OKUMA) ===");
        System.out.println("Okunacak Dosya: " + filePath);
        
        try (BufferedReader br = new BufferedReader(new FileReader(filePath))) {
            int[][] A = readMatrix(br);
            int[][] B = readMatrix(br);
            
            int n = A.length;
            int m = A[0].length;
            int p = B.length;
            int q = B[0].length;

            if (m != p) {
                throw new Exception("Hata: Carpim icin matris boyutlari uyumsuz (m != p).");
            }

            // Sure olcumu baslatiliyor
            long start = System.nanoTime();

            int[][] C = new int[n][q];
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < q; j++) {
                    for (int k = 0; k < m; k++) {
                        C[i][j] += A[i][k] * B[k][j];
                    }
                }
            }

            // Sure olcumu bitiriliyor
            long end = System.nanoTime();
            double durationMs = (end - start) / 1_000_000.0;

            System.out.println("Basarili: Dosya okundu ve matrisler basariyla carpildi.");
            System.out.println("Sonuc Matrisi Boyutu: " + n + "x" + q);
            System.out.printf("Carpim Suresi: %.3f ms%n", durationMs);
            return true;

        } catch (FileNotFoundException e) {
            System.out.println("Hata: Dosya bulunamadi veya acilamadi.");
            return false;
        } catch (Exception e) {
            System.out.println(e.getMessage());
            System.out.println("Dosya islemi durduruldu.");
            return false;
        }
    }

    // ===================== GENERATORS =====================
    
    static int[][] generateArrayMatrix(int rows, int cols) {
        Random rand = new Random();
        int[][] matrix = new int[rows][cols];
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                matrix[i][j] = rand.nextInt(10);
        return matrix;
    }

    static ArrayList<ArrayList<Integer>> generateArrayListMatrix(int rows, int cols) {
        Random rand = new Random();
        ArrayList<ArrayList<Integer>> matrix = new ArrayList<>();
        for (int i = 0; i < rows; i++) {
            ArrayList<Integer> row = new ArrayList<>();
            for (int j = 0; j < cols; j++) row.add(rand.nextInt(10));
            matrix.add(row);
        }
        return matrix;
    }

    static int multiplyElements(int a, int b) { return a * b; }

    static void multiplyArrayDirect(int[][] A, int[][] B, int[][] C, int n, int m, int p) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < p; j++) {
                C[i][j] = 0;
                for (int k = 0; k < m; k++) C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    static void multiplyArrayFunction(int[][] A, int[][] B, int[][] C, int n, int m, int p) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < p; j++) {
                C[i][j] = 0;
                for (int k = 0; k < m; k++) C[i][j] += multiplyElements(A[i][k], B[k][j]);
            }
        }
    }

    static void multiplyArrayListDirect(ArrayList<ArrayList<Integer>> A, ArrayList<ArrayList<Integer>> B, ArrayList<ArrayList<Integer>> C, int n, int m, int p) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < p; j++) {
                int sum = 0;
                for (int k = 0; k < m; k++) sum += A.get(i).get(k) * B.get(k).get(j);
                C.get(i).set(j, sum);
            }
        }
    }

    static void multiplyArrayListFunction(ArrayList<ArrayList<Integer>> A, ArrayList<ArrayList<Integer>> B, ArrayList<ArrayList<Integer>> C, int n, int m, int p) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < p; j++) {
                int sum = 0;
                for (int k = 0; k < m; k++) sum += multiplyElements(A.get(i).get(k), B.get(k).get(j));
                C.get(i).set(j, sum);
            }
        }
    }

    // ===================== BENCHMARK =====================

    static void benchmarkArray(int rows, int cols, int p, boolean useFunction) {
        int[][] A = generateArrayMatrix(rows, cols);
        int[][] B = generateArrayMatrix(cols, p);
        int[][] C = new int[rows][p];
        double total = 0;
        for (int i = 0; i < 5; i++) {
            long start = System.nanoTime();
            if (useFunction) multiplyArrayFunction(A, B, C, rows, cols, p);
            else multiplyArrayDirect(A, B, C, rows, cols, p);
            total += (System.nanoTime() - start) / 1_000_000.0;
        }
        System.out.printf("%-10s %-20s %-20s %dx%d -> %.3f ms%n", "Java", "Array", useFunction ? "Function Call" : "Direct", rows, cols, total / 5);
    }

    static void benchmarkArrayList(int rows, int cols, int p, boolean useFunction) {
        var A = generateArrayListMatrix(rows, cols);
        var B = generateArrayListMatrix(cols, p);
        var C = generateArrayListMatrix(rows, p);
        double total = 0;
        for (int i = 0; i < 5; i++) {
            long start = System.nanoTime();
            if (useFunction) multiplyArrayListFunction(A, B, C, rows, cols, p);
            else multiplyArrayListDirect(A, B, C, rows, cols, p);
            total += (System.nanoTime() - start) / 1_000_000.0;
        }
        System.out.printf("%-10s %-20s %-20s %dx%d -> %.3f ms%n", "Java", "ArrayList", useFunction ? "Function Call" : "Direct", rows, cols, total / 5);
    }

    public static void main(String[] args) {
        runFileDemo(FILE_PATH);
        System.out.println();

        // Basarili/Basarisiz fark etmeksizin her durumda sorar
        Scanner scanner = new Scanner(System.in);
        System.out.print("Demo asamasi tamamlandi. Benchmark testlerine gecmek istiyor musunuz? (E/H): ");
        String choice = scanner.nextLine();
        if (!choice.equalsIgnoreCase("e")) {
            System.out.println("Program kullanici tarafindan sonlandirildi.");
            scanner.close();
            return;
        }
        System.out.println();
        scanner.close();

        System.out.println("=== BENCHMARK ASAMASI ===");
        System.out.println("Language | Implementation | Computation Type | Size | Avg. Time (ms)");
        System.out.println("----------------------------------------------------------------------");

        int[][] sizes = {{50,80}, {120,150}, {300,400}, {900,700}};
        for (int[] s : sizes) {
            benchmarkArray(s[0], s[1], s[0], false);
            benchmarkArray(s[0], s[1], s[0], true);
            benchmarkArrayList(s[0], s[1], s[0], false);
            benchmarkArrayList(s[0], s[1], s[0], true);
        }
    }
}
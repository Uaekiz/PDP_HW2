#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <stdexcept>
#include <cstdlib>

using namespace std;
using namespace chrono;

// ===================== DOSYA OKUMA VE DEMO =====================

vector<vector<int>> readMatrix(ifstream& file) {
    string line;
    while (getline(file, line)) {
        if (!line.empty()) break;
    }
    if (line.empty()) throw runtime_error("Hata: Beklenen matris bulunamadi (EOF).");

    stringstream ss(line);
    int rows, cols;
    if (!(ss >> rows >> cols)) throw runtime_error("Hata: Gecersiz matris boyutlari.");

    vector<vector<int>> matrix(rows, vector<int>(cols));
    for (int i = 0; i < rows; i++) {
        if (!getline(file, line)) throw runtime_error("Hata: Satir verisi eksik.");
        stringstream rowStream(line);
        string value;
        int count = 0;
        
        while (rowStream >> value) {
            if (count >= cols) throw runtime_error("Hata: Satirda fazla eleman (Extra elements in row).");
            try {
                matrix[i][count] = stoi(value);
            } catch (...) {
                throw runtime_error("Hata: Sayisal olmayan deger tespit edildi (Non-numeric value detected).");
            }
            count++;
        }
        if (count < cols) throw runtime_error("Hata: Satirda eksik eleman (Missing elements in row).");
    }
    return matrix;
}

bool runFileDemo(const string& filepath) {
    cout << "=== DEMO ASAMASI (DOSYA OKUMA) ===" << endl;
    cout << "Okunacak Dosya: " << filepath << endl;
    
    ifstream file(filepath);
    if (!file.is_open()) {
        cout << "Hata: Dosya bulunamadi veya acilamadi." << endl;
        return false;
    }

    try {
        vector<vector<int>> A = readMatrix(file);
        vector<vector<int>> B = readMatrix(file);

        int n = A.size();
        int m = A[0].size();
        int p = B.size();
        int q = B[0].size();

        if (m != p) {
            throw runtime_error("Hata: Carpim icin matris boyutlari uyumsuz (m != p).");
        }

        // Sure olcumu baslatiliyor
        auto start = high_resolution_clock::now();

        vector<vector<int>> C(n, vector<int>(q, 0));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < q; j++) {
                for (int k = 0; k < m; k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }

       // Sure olcumu bitiriliyor
        auto stop = high_resolution_clock::now();
        // Degisken adini duration yerine carpimSuresiMs yaptik ve std::milli kullandik
        double carpimSuresiMs = std::chrono::duration<double, std::milli>(stop - start).count();

        cout << "Basarili: Dosya okundu ve matrisler basariyla carpildi." << endl;
        cout << "Sonuc Matrisi Boyutu: " << n << "x" << q << endl;
        cout << "Carpim Suresi: " << fixed << setprecision(3) << carpimSuresiMs << " ms" << endl;
        
        file.close();
        return true;
    } 
    catch (const exception& e) {
        cout << e.what() << endl;
        cout << "Dosya islemi durduruldu." << endl;
        file.close();
        return false;
    }
}

// ===================== GENERATORS & MULTIPLICATION =====================

int** generateDynamicMatrix(int rows, int cols) {
    int **matrix = new int*[rows];
    for (int i = 0; i < rows; i++) {
        matrix[i] = new int[cols];
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = rand() % 10;
        }
    }
    return matrix;
}

void freeDynamicMatrix(int **matrix, int rows) {
    for (int i = 0; i < rows; i++) delete[] matrix[i];
    delete[] matrix;
}

vector<vector<int>> generateVectorMatrix(int rows, int cols) {
    vector<vector<int>> matrix(rows, vector<int>(cols));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) matrix[i][j] = rand() % 10;
    }
    return matrix;
}

inline int multiplyElements(int a, int b) { return a * b; }

void multiplyDynamicDirect(int **A, int **B, int **C, int n, int m, int p) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            C[i][j] = 0;
            for (int k = 0; k < m; k++) C[i][j] += A[i][k] * B[k][j];
        }
    }
}

void multiplyDynamicFunction(int **A, int **B, int **C, int n, int m, int p) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            C[i][j] = 0;
            for (int k = 0; k < m; k++) C[i][j] += multiplyElements(A[i][k], B[k][j]);
        }
    }
}

void multiplyVectorDirect(vector<vector<int>> &A, vector<vector<int>> &B, vector<vector<int>> &C, int n, int m, int p) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            C[i][j] = 0;
            for (int k = 0; k < m; k++) C[i][j] += A[i][k] * B[k][j];
        }
    }
}

void multiplyVectorFunction(vector<vector<int>> &A, vector<vector<int>> &B, vector<vector<int>> &C, int n, int m, int p) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            C[i][j] = 0;
            for (int k = 0; k < m; k++) C[i][j] += multiplyElements(A[i][k], B[k][j]);
        }
    }
}

// ===================== BENCHMARK =====================

void benchmarkDynamic(int rows, int cols, int p, bool useFunction) {
    int **A = generateDynamicMatrix(rows, cols);
    int **B = generateDynamicMatrix(cols, p);
    int **C = generateDynamicMatrix(rows, p);

    double total = 0;
    for (int i = 0; i < 5; i++) {
        auto start = high_resolution_clock::now();
        if (useFunction) multiplyDynamicFunction(A, B, C, rows, cols, p);
        else multiplyDynamicDirect(A, B, C, rows, cols, p);
        auto stop = high_resolution_clock::now();
        total += std::chrono::duration<double, std::milli>(stop - start).count();
    }
    
    cout << left << setw(10) << "C++" << setw(20) << "Dynamic Array" << setw(20) << (useFunction ? "Function Call" : "Direct")
         << rows << "x" << cols << " -> " << fixed << setprecision(3) << total / 5 << endl;

    freeDynamicMatrix(A, rows);
    freeDynamicMatrix(B, cols);
    freeDynamicMatrix(C, rows);
}

void benchmarkVector(int rows, int cols, int p, bool useFunction) {
    auto A = generateVectorMatrix(rows, cols);
    auto B = generateVectorMatrix(cols, p);
    vector<vector<int>> C(rows, vector<int>(p));

    double total = 0;
    for (int i = 0; i < 5; i++) {
        auto start = high_resolution_clock::now();
        if (useFunction) multiplyVectorFunction(A, B, C, rows, cols, p);
        else multiplyVectorDirect(A, B, C, rows, cols, p);
        auto stop = high_resolution_clock::now();
        total += std::chrono::duration<double, std::milli>(stop - start).count();
    }
    
    cout << left << setw(10) << "C++" << setw(20) << "Vector" << setw(20) << (useFunction ? "Function Call" : "Direct")
         << rows << "x" << cols << " -> " << fixed << setprecision(3) << total / 5 << endl;
}

// ===================== MAIN =====================

int main() {
    string FILE_PATH = "datasets/valid_xxlarge.txt"; // TEST ETMEK ICIN BURAYI DEGISTIR
    
    runFileDemo(FILE_PATH);
    cout << endl;

    // Basarili/Basarisiz fark etmeksizin her durumda sorar
    cout << "Demo asamasi tamamlandi. Benchmark testlerine gecmek istiyor musunuz? (E/H): ";
    char choice;
    cin >> choice;
    if (choice != 'E' && choice != 'e') {
        cout << "Program kullanici tarafindan sonlandirildi." << endl;
        return 0;
    }
    cout << endl;

    cout << "=== BENCHMARK ASAMASI ===" << endl;
    cout << "Language | Implementation | Computation Type | Size | Avg. Time (ms)" << endl;
    cout << "----------------------------------------------------------------------" << endl;

    vector<pair<int,int>> sizes = {{50,80}, {120,150}, {300,400}, {900,700}};
    
    for (auto s : sizes) {
        int rows = s.first;
        int cols = s.second;
        benchmarkDynamic(rows, cols, rows, false);
        benchmarkDynamic(rows, cols, rows, true);
        benchmarkVector(rows, cols, rows, false);
        benchmarkVector(rows, cols, rows, true);
    }
    return 0;
}
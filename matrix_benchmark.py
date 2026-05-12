import random
import time
import os

FILE_PATH = "datasets/invalid_dimension_1.txt" # TEST ETMEK ICIN BURAYI DEGISTIR

# ===================== DOSYA OKUMA VE DEMO =====================

def read_matrix(lines, start_index):
    while start_index < len(lines) and not lines[start_index].strip():
        start_index += 1
        
    if start_index >= len(lines):
        raise ValueError("Hata: Beklenen matris bulunamadi (EOF).")

    dims = lines[start_index].strip().split()
    if len(dims) != 2:
        raise ValueError("Hata: Gecersiz matris boyutlari.")
        
    rows, cols = int(dims[0]), int(dims[1])
    matrix = []
    start_index += 1

    for _ in range(rows):
        if start_index >= len(lines):
            raise ValueError("Hata: Satir verisi eksik.")
            
        row_line = lines[start_index].strip().split()
        if len(row_line) < cols:
            raise ValueError("Hata: Satirda eksik eleman (Missing elements in row).")
        if len(row_line) > cols:
            raise ValueError("Hata: Satirda fazla eleman (Extra elements in row).")

        row = []
        for value in row_line:
            if not value.lstrip('-').isdigit():
                raise ValueError("Hata: Sayisal olmayan deger tespit edildi (Non-numeric value detected).")
            row.append(int(value))
            
        matrix.append(row)
        start_index += 1

    return matrix, start_index

def run_file_demo(filepath):
    print("=== DEMO ASAMASI (DOSYA OKUMA) ===")
    print(f"Okunacak Dosya: {filepath}")
    
    if not os.path.exists(filepath):
        print("Hata: Dosya bulunamadi veya acilamadi.")
        return False

    try:
        with open(filepath, 'r') as file:
            lines = file.readlines()

        A, next_idx = read_matrix(lines, 0)
        B, _ = read_matrix(lines, next_idx)

        m = len(A[0])
        p = len(B)

        if m != p:
            raise ValueError("Hata: Carpim icin matris boyutlari uyumsuz (m != p).")

        C = multiply_direct(A, B)
        print("Basarili: Dosya okundu ve matrisler basariyla carpildi.")
        return True
        
    except ValueError as e:
        print(e)
        print("Dosya islemi durduruldu.")
        return False
    except Exception as e:
        print(f"Bilinmeyen hata: {e}")
        return False

# ===================== GENERATOR & MULTIPLICATION =====================

def generate_matrix(rows, cols):
    return [[random.randint(0, 9) for _ in range(cols)] for _ in range(rows)]

def multiply_elements(a, b):
    return a * b

def multiply_direct(A, B):
    rows, cols, common = len(A), len(B[0]), len(B)
    C = [[0 for _ in range(cols)] for _ in range(rows)]
    for i in range(rows):
        for j in range(cols):
            for k in range(common):
                C[i][j] += A[i][k] * B[k][j]
    return C

def multiply_function(A, B):
    rows, cols, common = len(A), len(B[0]), len(B)
    C = [[0 for _ in range(cols)] for _ in range(rows)]
    for i in range(rows):
        for j in range(cols):
            for k in range(common):
                C[i][j] += multiply_elements(A[i][k], B[k][j])
    return C

# ===================== BENCHMARK =====================

def benchmark(rows, cols, p, use_function):
    A = generate_matrix(rows, cols)
    B = generate_matrix(cols, p)
    total = 0
    for _ in range(5):
        start = time.perf_counter()
        if use_function:
            multiply_function(A, B)
        else:
            multiply_direct(A, B)
        total += (time.perf_counter() - start) * 1000
        
    print(f"{'Python':<10} {'List':<20} {'Function Call' if use_function else 'Direct':<20} {rows}x{cols} -> {total/5:.3f} ms")

# ===================== MAIN =====================

if __name__ == "__main__":
    demo_success = run_file_demo(FILE_PATH)
    print()

    if not demo_success:
        choice = input("Dosya okuma basarisiz oldu. Benchmark testlerine devam etmek istiyor musunuz? (E/H): ")
        if choice.lower() != 'e':
            print("Program kullanici tarafindan sonlandirildi.")
            exit()
        print()

    print("=== BENCHMARK ASAMASI ===")
    print("Language | Implementation | Computation Type | Size | Avg. Time (ms)")
    print("-" * 70)

    sizes = [(50, 80), (120, 150), (300, 400), (900, 700)]
    for rows, cols in sizes:
        benchmark(rows, cols, rows, False)
        benchmark(rows, cols, rows, True)
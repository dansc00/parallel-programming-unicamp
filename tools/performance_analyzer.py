def calcular_speedup_eficiencia(tempos_serial, tempos_paralelo, num_threads):
    if not (len(tempos_serial) == len(tempos_paralelo) == len(num_threads)):
        raise ValueError("Todas as listas devem ter o mesmo tamanho.")

    speedups = []
    eficiencias = []

    for i in range(len(tempos_serial)):
        T1 = tempos_serial[i]
        Tp = tempos_paralelo[i]
        P = num_threads[i]

        speedup = T1 / Tp if Tp != 0 else 0
        eficiencia = speedup / P if P != 0 else 0

        speedups.append(speedup)
        eficiencias.append(eficiencia)

    return speedups, eficiencias

# Leitura das entradas do usuário
print("Digite os tempos de execução serial (separados por espaço):")
tempos_serial = list(map(float, input().split()))

print("Digite os tempos de execução paralela (separados por espaço):")
tempos_paralelo = list(map(float, input().split()))

print("Digite o número de threads usados (separados por espaço):")
num_threads = list(map(int, input().split()))

# Cálculo
speedups, eficiencias = calcular_speedup_eficiencia(tempos_serial, tempos_paralelo, num_threads)

# Resultado
print("\nResultados:")
for i in range(len(speedups)):
    print(f"[Execução {i+1}] Speedup: {speedups[i]:.2f} | Eficiência: {eficiencias[i]:.2f}")

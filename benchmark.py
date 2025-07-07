import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("benchmark_results.csv")

plt.figure(figsize=(12, 6))

plt.plot(data["n"], data["time_insert"], label="Inserción", marker="o")

plt.plot(data["n"], data["time_range"], label="Consulta por rango", marker="s")

plt.xlabel("Número de puntos (n)")
plt.ylabel("Tiempo (μs)")
plt.title("Desempeño de Inserción y Consulta por Rango en QuadTree")
plt.legend()
plt.grid(True)
plt.yscale("log")
plt.tight_layout()

plt.savefig("benchmark_quadtree_insert_range.png")
plt.show()

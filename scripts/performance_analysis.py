import sys
from pathlib import Path
import numpy as np
from graph_plotter import GraphPlotter

class PerformanceAnalysis:

    def __init__(self, id, serialDir, parallelDir):
        self.id = id
        serialPath = Path(serialDir)
        parallelPath = Path(parallelDir)

        # métricas de execução retiradas dos arquivos .out
        self.serialMetrics = self.getExecutionMetrics(serialPath)
        self.parallelMetrics = self.getExecutionMetrics(parallelPath)

        # listas de métricas de desempenho
        self.speedups = self.getSpeedups(self.serialMetrics.get("means"), self.parallelMetrics.get("means"))
        self.efficiencies = self.getEfficiencies(self.speedups, self.parallelMetrics.get("threads"))

    # retorna chave para ordenar saídas
    @staticmethod
    def outKey(path: Path) -> int:
        with open(path) as f:
            return int(f.readline().split()[0])

    # retorna dicionário de métricas de execução
    def getExecutionMetrics(self, dirPath):
        inputs   = []
        means    = []
        stdDevs  = []
        maxs     = []
        mins     = []
        errors   = []
        threads  = []          # número de threads por arquivo

        try:
            for filePath in sorted(dirPath.glob("*.out"), key=self.outKey):
                with open(filePath) as f:
                    lines = [ln.strip() for ln in f if ln.strip()]

                first = lines[0]
                second = lines[1]
                inputs.append(int(first))
                threads.append(int(second))

                # tempos de execução
                t = np.array([float(ln) for ln in lines[2:]])

                means.append(t.mean()  if t.size else 0)
                stdDevs.append(t.std() if t.size else 0)
                maxs.append(t.max()    if t.size else 0)
                mins.append(t.min()    if t.size else 0)
                errors.append(stdDevs[-1]/np.sqrt(t.size) if t.size else 0)

        except Exception as e:
            print(f"File read error: {e}")
            sys.exit(1)

        return {
            "inputs":   inputs,
            "means":    means,
            "stdDevs":  stdDevs,
            "maxs":     maxs,
            "mins":     mins,
            "errors":   errors,
            "threads":  threads,
        }

    def getEfficiencies(self, speedups, threads):
        return [spd/th if th > 0 else 0 for spd, th in zip(speedups, threads)]

    def getSpeedups(self, serialTimes, parallelTimes):
        return [st/sp if sp > 0 else 0 for st, sp in zip(serialTimes, parallelTimes)]

    def plotSpeedupGraph(self, speedups, title, xLabel, yLabel):
        plot = GraphPlotter(title=title, xLabel=xLabel, yLabel=yLabel, legendPosition="right")
        plot.plotLineGraph(x=self.serialMetrics.get("inputs"), y=speedups, color="blue", plotLabel="Speedup", xScale="log")
        plot.saveGraph(f"../images/{self.id}-speedup.png")

    def plotEfficiencyGraph(self, efficiencies, title, xLabel, yLabel):
        plot = GraphPlotter(title=title, xLabel=xLabel, yLabel=yLabel, legendPosition="right")
        plot.plotLineGraph(x=self.serialMetrics.get("inputs"), y=efficiencies, color="red", plotLabel="Efficiency", xScale="log")
        plot.saveGraph(f"../images/{self.id}-efficiency.png")

    def plotTimeGraph(self, serialTimes, parallelTimes, title, xLabel, yLabel):
        plot = GraphPlotter(title=title, xLabel=xLabel, yLabel=yLabel, legendPosition="right")
        plot.plotLineGraph(x=self.serialMetrics.get("inputs"), y=serialTimes, color="yellow", plotLabel="Serial time", xScale="log")
        plot.plotLineGraph(x=self.serialMetrics.get("inputs"), y=parallelTimes, color="green", plotLabel="Parallel time", xScale="log")
        plot.saveGraph(f"../images/{self.id}-time.png")

    def printMetrics(self, OUTPUTS):
        inputs = self.serialMetrics.get("inputs")
        serialMeans = self.serialMetrics.get("means")
        serialStdDevs = self.serialMetrics.get("stdDevs")
        serialMaxs = self.serialMetrics.get("maxs")
        serialMins = self.serialMetrics.get("mins")
        serialErrors = self.serialMetrics.get("errors")

        parallelMeans = self.parallelMetrics.get("means")
        parallelStdDevs = self.parallelMetrics.get("stdDevs")
        parallelMaxs = self.parallelMetrics.get("maxs")
        parallelMins = self.parallelMetrics.get("mins")
        parallelErrors = self.parallelMetrics.get("errors")

        serialPlaces = self.getDecimalPlacesList(serialErrors)
        parallelPlaces = self.getDecimalPlacesList(parallelErrors)

        for i in range(OUTPUTS):
            print(f"{self.id}:")
            print(f"Dimensão do grid: {inputs[i]}")
            print(f"Tempo médio de execução serial: ({serialMeans[i]:.{serialPlaces[i]}f} ± {serialErrors[i]:.{serialPlaces[i]}f}) s")
            print(f"Tempo máximo de execução serial: {serialMaxs[i]} s")
            print(f"Tempo mínimo de execução serial: {serialMins[i]} s")
            print(f"Desvio padrão de execução serial: {serialStdDevs[i]} s")
            print("----------------------------------------------------------------------------------------------------------")
            print(f"Tempo médio de execução paralela: ({parallelMeans[i]:.{parallelPlaces[i]}f} ± {parallelErrors[i]:.{parallelPlaces[i]}f}) s")
            print(f"Tempo máximo de execução paralela: {parallelMaxs[i]} s")
            print(f"Tempo mínimo de execução paralela: {parallelMins[i]} s")
            print(f"Desvio padrão de execução paralela: {parallelStdDevs[i]} s")
            print()

    # retorna quantidade correta de casas decimais para representação (value ± error)
    @staticmethod
    def getDecimalPlacesList(errors):
        placesList = []
        for error in errors:
            if error == 0:
                placesList.append(0)
                continue
        
            places = np.floor(np.log10(np.abs(error)))
            placesList.append(max(0, int(-places)))

        return placesList

if __name__ == "__main__":

    matrixGrid = PerformanceAnalysis("matrix-fix-random", "../serial-outs/matrix/", "../parallel-outs/matrix/")
    matrixGrid.plotSpeedupGraph(matrixGrid.speedups, "Matrix Grid: Speedup Graph", "Grid size (log scale)", "Speedup")
    matrixGrid.plotEfficiencyGraph(matrixGrid.efficiencies, "Matrix Grid: Efficiency Graph", "Grid size (log scale)", "Efficiency")
    matrixGrid.plotTimeGraph(matrixGrid.serialMetrics.get("means"), matrixGrid.parallelMetrics.get("means"), "Matrix Grid: Serial Time vs Parallel Time",
                             "Grid size (log scale)", "Time (s)")
    matrixGrid.printMetrics(len(matrixGrid.serialMetrics.get("means")))

    setGrid = PerformanceAnalysis("set-fix-random", "../serial-outs/set/", "../parallel-outs/set/")
    setGrid.plotSpeedupGraph(setGrid.speedups, "Set Grid: Speedup Graph", "Grid size (log scale)", "Speedup")
    setGrid.plotEfficiencyGraph(setGrid.efficiencies, "Set Grid: Efficiency Graph", "Grid size (log scale)", "Efficiency")
    setGrid.plotTimeGraph(setGrid.serialMetrics.get("means"), setGrid.parallelMetrics.get("means"), "Set Grid: Serial Time vs Parallel Time",
                          "Grid size (log scale)", "Time (s)")
    setGrid.printMetrics(len(setGrid.serialMetrics.get("means")))
 
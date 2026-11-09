import json
import sys
from graph_plotter import GraphPlotter

# performance graph plotter using hyperfine benchmarks
class HyperfineHandler:

    def __init__(self, path, flag):
        self.path = path # benchmarks source path
        self.flag = flag # flag option

    # read hyperfine benchmark in the input path
    def readBenchmark(self, path):
        # read hyperfine json
        try:
            with open(path, "r") as file:
                benchmark = json.load(file)

        except Exception as e:
            print("File read error")
            sys.exit(1)

        return benchmark 

    # return serial time average
    def getSerialTime(self, path):
        benchmark = self.readBenchmark(path)
        tSerial = benchmark['results'][0]['mean'] # serial time average
        return tSerial

    # return parallel time average
    def getParallelTime(self, path):
        benchmark = self.readBenchmark(path)
        tParallel = benchmark['results'][1]['mean'] # parallel time average
        return tParallel

    # return efficiency
    def getEfficiency(self, speedup, n):
        return speedup/n

    # return speedup
    def getSpeedup(self, tSerial, tParallel):
        return tSerial/tParallel

    # return speedups
    def getSpeedups(self, path):
        tSerial = self.getSerialTime(path)
        tParallel = self.getParallelTime(path)

        return self.getSpeedup(tSerial, tParallel)

    # generate performance graphs using hyperfine benchmarks
    def hyperfinePlot(self):

        try:
            MAX_BENCHMARKS = int(input("Maximum number of benchmarks: "))
            xLabel = input("x label: ")
            xData = list((map(float, input("x data (a b c ...): ").split())))
            print()

        except ValueError:
            print("Invalid input")
            sys.exit(1)
        
        efficiencies = [0] * len(xData) # efficiencies list
        speedups = [0] * len(xData) # speedups list
        serialTimes = [0] * len(xData) # serial times averages
        parallelTimes = [0] * len(xData) # parallel times averages
        
        paths = [] # benchmarks list

        for i in range(MAX_BENCHMARKS):
            paths.append(self.path+"benchmark"+str(i+1)+".json")

        if self.flag == "--speedup":
            for i in range(len(xData)):
                speedups[i] = self.getSpeedups(paths[i])
            
            plotter = GraphPlotter(xLabel=xLabel, yLabel="Speedup")
            plotter.plotLineGraph(xData, speedups, color="blue", plotLabel="Speedup")

        elif self.flag == "--efficiency":
            try:
                threads = list((map(float, input("Number of threads for each input (a b c ...): ").split())))
                print()

            except ValueError:
                print("Invalid input")
                sys.exit(1)
                
            for i in range(len(xData)):
                efficiencies[i] = self.getEfficiency(self.getSpeedups(paths[i]), threads[i])
            
            plotter = GraphPlotter(xLabel=xLabel, yLabel="Efficiency")
            plotter.plotLineGraph(xData, efficiencies, color="red", plotLabel="Efficiency")
        
        elif self.flag == "--time":
            for i in range(len(xData)):
                serialTimes[i] = self.getSerialTime(paths[i])
                parallelTimes[i] = self.getParallelTime(paths[i])
            
            plotter = GraphPlotter(xLabel=xLabel, yLabel="Time (s)")
            plotter.plotLineGraph(xData, serialTimes, color="yellow", plotLabel="Serial Time")
            plotter.plotLineGraph(xData, parallelTimes, color="green", plotLabel="Parallel Time")
        
        else:
            print("Invalid input")
            sys.exit(1)

        plotter.showGraph()

if __name__ == "__main__":

    def inputPlotWarning():

        print("usage: ")
        print("python3 hyperfine_handler.py --path <hyperfine_benchmark_dir_source> --speedup (to plot speedup graph, using hyperfine benchmarks)")
        print("python3 hyperfine_handler.py --path <hyperfine_benchmark_dir_source> --efficiency (to plot efficiency graph, using hyperfine benchmarks)")
        print("python3 hyperfine_handler.py --path <hyperfine_benchmark_dir_source> --time (to plot comparative time graph, using hyperfine benchmarks)")
        print("OBS: HYPERFINE BENCHMARKS MUST BE IN THE FORMAT benchmark1.json, benchmark2.json, benchmark3.json...")

    def plotPerformance(args):

        if len(args) < 4:
            inputPlotWarning()
        else:
            if args[1].strip() == "--path" and args[2].strip():
                path = args[2].strip()

                if args[3].strip() == "--speedup":
                    flag = args[3].strip()
                    hyp = HyperfineHandler(path, flag)
                    hyp.hyperfinePlot()
                    
                elif args[3].strip() == "--efficiency":
                    flag = args[3].strip()
                    hyp = HyperfineHandler(path, flag)
                    hyp.hyperfinePlot()                

                elif args[3].strip() == "--time":
                    flag = args[3].strip()
                    hyp = HyperfineHandler(path, flag)
                    hyp.hyperfinePlot()     
                else:
                    inputPlotWarning()
            else:
                inputPlotWarning()

    plotPerformance(sys.argv)


    
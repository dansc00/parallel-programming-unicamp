import matplotlib.pyplot as plt
import json
import sys

def plotGraph(x, y, marker, linestyle, color, label, xLabel, yLabel, title, grid):
    plt.plot(x, y, marker=marker, linestyle=linestyle, color=color, label=label)
    plt.xlabel(xLabel)
    plt.ylabel(yLabel)
    plt.title(title)
    plt.grid(grid)

def showGraph():
    plt.legend()
    plt.show()

# read benchmark in the input path
def readBenchmark(path):
     # read hyperfine json
    try:
        with open(path, "r") as file:
            benchmark = json.load(file)

    except Exception as e:
        print("File read error")
        sys.exit(1)

    return benchmark 

# return serial time average
def getSerialTime(path):
    benchmark = readBenchmark(path)
    tSerial = benchmark['results'][0]['mean'] # serial time average
    return tSerial

# return parallel time average
def getParallelTime(path):
    benchmark = readBenchmark(path)
    tParallel = benchmark['results'][1]['mean'] # parallel time average
    return tParallel

# return efficiency
def getEfficiency(speedup, n):
    return speedup/n

# return speedup
def getSpeedup(tSerial, tParallel):
    return tSerial/tParallel

# return speedups
def getSpeedups(path):
    tSerial = getSerialTime(path)
    tParallel = getParallelTime(path)

    return getSpeedup(tSerial, tParallel)

# generate graphs using ../hyperfine directory
def defaultDataInput():

    try:
        MAX_THREADS = int(input("Number of threads: "))

    except ValueError:
        print("Invalid input")
        sys.exit(1)

    print("Graph options\ns to Speedup x Threads graph\ne to Efficiency x Threads graph\nt to Serial/Parallel times x Threads graph")
    flag = input()
    
    numThreads = [i for i in range(2, MAX_THREADS+2, 2)] # number of threads
    efficiencies = [0] * len(numThreads) # efficiencies list
    speedups = [0] * len(numThreads) # speedups list

    serialTimes = [0] * len(numThreads) # serial times averages
    parallelTimes = [0] * len(numThreads) # parallel times averages
    
    paths = ["../hyperfine/benchmark2.json",
             "../hyperfine/benchmark4.json",
             "../hyperfine/benchmark6.json",
             "../hyperfine/benchmark8.json",
             "../hyperfine/benchmark10.json",
             "../hyperfine/benchmark12.json",
             "../hyperfine/benchmark14.json",
             "../hyperfine/benchmark16.json"] # paths list

    if flag == "s":
        for i in range(len(numThreads)):
            speedups[i] = getSpeedups(paths[i])
            
        plotGraph(numThreads, speedups, "o", "-", "blue", "Speedup", "Threads", "Speedup", "", True)
        showGraph()

    elif flag == "e":
        for i in range(len(numThreads)):
            efficiencies[i] = getEfficiency(getSpeedups(paths[i]), numThreads[i])
        
        plotGraph(numThreads, efficiencies, "o", "-", "red", "Efficiency", "Threads", "Efficiency", "", True)
        showGraph()
    
    elif flag == "t":
        for i in range(len(numThreads)):
            serialTimes[i] = getSerialTime(paths[i])
            parallelTimes[i] = getParallelTime(paths[i])
        
        plotGraph(numThreads, serialTimes, "o", "-", "yellow", "Serial Time", "Threads", "Time (s)", "", True)
        plotGraph(numThreads, parallelTimes, "o", "-", "green", "Parallel Time", "Threads", "Time (s)", "", True)
        showGraph()
    
    else:
        print("Invalid input")
        sys.exit(1)

# dynamic user input to type of graph and data
def userDataInput():

    try:
        nPlots = int(input("Number of plots: ")) # number of plots
    
    except ValueError:
        print("Invalid input")
        sys.exit(1)

    # plots list
    xAxisList = [[] for i in range(nPlots)]
    yAxisList = [[] for i in range(nPlots)]

    # input axis data
    for i in range(nPlots):
        print(f"Plot{i+1}: ")
        try:
            xData = list((map(float, input("x data (a b c ...): ").split())))
            yData = list((map(float, input("y data (a b c ...): ").split())))
        
        except ValueError:
            print("Invalid input")
            sys.exit(1)
        
        xAxisList[i] = xData
        yAxisList[i] = yData
        print()
    
    # input axis label and title
    try:
        xLabel = input("x label: ")
        yLabel = input("y label: ")
        title = input("Graph title: ")
    
    except ValueError:
        print("Invalid input")
        sys.exit(1)

    print()
    # plotting graph
    for i in range(nPlots):
        x = xAxisList[i]
        y = yAxisList[i]

        try:
            plotColor = input(f"Plot{i+1} color: ")
            plotLabel = input(f"Plot{i+1} label: ")
        
        except ValueError:
            print("Invalid input")
            sys.exit(1)

        plotGraph(x, y, "o", "-", plotColor, plotLabel, xLabel, yLabel, title, True)
        print()
        
    showGraph()

if __name__ == "__main__":

    if len(sys.argv) < 2 or sys.argv[1] == "-d":
        defaultDataInput()

    elif sys.argv[1] == "-u":
        userDataInput()       

    else:
        print("usage: ")
        print("python3 plot.py -u (to user input)")
        print("python3 plot.py -d (to default input using hyperfine benchmarks)")

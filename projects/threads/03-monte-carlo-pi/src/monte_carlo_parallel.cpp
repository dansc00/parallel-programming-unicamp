#include <cassert>
#include <cstdlib>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

// Function to generate random numbers between -1 and 1
double random_number() {
  thread_local std::random_device rd;
  thread_local std::mt19937 gen(rd());
  thread_local std::uniform_real_distribution<double> dis(-1.0, 1.0);
  return dis(gen);
}

// Function to estimate pi using the Monte Carlo method
void calculate_pi(int &thread_hits, int num_iterations) {
  int hits = 0;
  for (int i = 0; i < num_iterations; ++i) {

    double x = random_number();
    double y = random_number();

    if (x * x + y * y <= 1.0) { // inside the circle
      ++hits;
    }
  }

  thread_hits += hits;

  std::cout << "hits: " << hits << " of " << num_iterations << std::endl;
}

int main(int argc, char *argv[]) {
  // check that the the correct number of arguments are being passed via command line
  if (argc != 3) { 
    std::cerr << "Usage: ./program_name num_iterations num_threads"
              << std::endl;
    return 1;
  }

  const int num_iterations = std::atoi(argv[1]); // convert string num_iterations to int
  const int num_threads = std::atoi(argv[2]); // convert string num_threads to int

  assert(num_iterations > 0); // iterations check
  assert(num_threads > 0); // threads check

  int count = 0; // sum of points inside the circle of each thread

  std::vector<int> threads_hits(num_threads, 0); // initialize threads_hits vector with 0 in num_threads positions 
  std::vector<std::thread> threads; // threads vector

  for (int i = 0; i < num_threads; ++i) { // create threads
    threads.push_back(std::thread(calculate_pi, std::ref(threads_hits[i]), num_iterations/num_threads));
  }

  for(auto &t : threads){
    t.join();
  }

  for(int th : threads_hits){ // sum of each threads contribution
    count += th;
  }

  std::cout << "count: " << count << " of " << num_iterations << std::endl;
  double pi = 4.0 * (double)count / (double)num_iterations;
  std::cout << "Used " << num_iterations << " points to estimate pi: " << pi << std::endl;

  return 0;
}

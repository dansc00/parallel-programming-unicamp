#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <omp.h>
#include <filesystem>
#define NUM_THREADS 1

int indetation_level = 0;
void _log_indetation()
{
    for (int i = 0; i < indetation_level; ++i) {
        std::cerr << "  ";
    }
}

void log_start_prop(const std::string &start = ""){
    _log_indetation();
    std::cerr << start << ":\n";
    indetation_level += 1;
}
void log_start_arr_item(){
    _log_indetation();
    std::cerr << "-\n";
    indetation_level++;
}
void log_end(){
    indetation_level--;
}
void log_time(const std::string &message, double start_time)
{
    _log_indetation();
    std::cerr << message  << ": " << omp_get_wtime() - start_time << "\n";
}

struct pair_hash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};


void printMatrix(
    const std::unordered_set<std::pair<int, int>, pair_hash>& alive_cells,
    int rows,
    int cols
) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (alive_cells.find({i, j}) != alive_cells.end()) {
                std::cout << "X" << " ";
            } else {
                std::cout << "." << " ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << "\n" << std::endl;
}


void process(
    const std::unordered_set<std::pair<int, int>, pair_hash>& alive_cells_current,
    std::unordered_set<std::pair<int, int>, pair_hash>& alive_cells_next,
    int rows,
    int cols
) {
    std::unordered_map<std::pair<int, int>, int, pair_hash> cells_counter;
    auto time = omp_get_wtime();
    for (auto current_cell : alive_cells_current) {
        for (int ii = -1; ii <= 1; ++ii) {
            for (int jj = -1; jj <= 1; ++jj) {
                if (ii != 0 || jj != 0) {
                    if ((current_cell.first+ii >= 0 && current_cell.first+ii < rows) &&
                        (current_cell.second+jj >= 0 && current_cell.second+jj < cols)) {
                        
                        std::pair<int, int> being_checked = {current_cell.first+ii, current_cell.second+jj};
                        cells_counter.try_emplace(being_checked, 0);
                        cells_counter[being_checked]++;
                    }
                }
            }
        }
    }

    log_time("process_counter", time);
    time = omp_get_wtime();

    time = omp_get_wtime();
    // Uma célula continua viva se houver entre 2 ou 3 vizinhos vivos
    // ou se estiver morta e houver exatamente 3 vizinhos vivos

    for (auto current_coordinate: cells_counter)
    {
        auto t = omp_get_thread_num();
        if ( current_coordinate.second == 3) {
            alive_cells_next.emplace(current_coordinate.first);
        }
        if (current_coordinate.second == 2) {
            if (alive_cells_current.find(current_coordinate.first) != alive_cells_current.end())
            {
                alive_cells_next.emplace(current_coordinate.first);
            }
        }
    }
    log_time("process_next", time);
}

bool writeOutput(double time, int n_iter, std::string filename) {
    if (n_iter <= 0) {
        std::cerr << "Erro ao gerar arquivo de saída: n_iter inválido\n";
        return false;
    }   

    std::string out_dir = "../serial-outs/set/";
    std::filesystem::path out_path = out_dir + std::filesystem::path(filename).stem().string() + ".out";

    std::ofstream out(out_path, std::ios::app);
    if (!out) {
        std::cerr << "Erro ao abrir " << out_path << '\n';
        return false;
    }
   
    out << std::fixed << time << '\n';
    return true;
}

int main(int argc, char* argv[]) {
    auto time_total = omp_get_wtime();
    int rows, cols, n_iter;
    std::string line;
    
    if (argc < 2) {
        std::cerr << "Preciso de um arquivo para rodar.\n";
        return 1;
    }
    std::string filename = argv[1];
    bool is_interative = false;
    if (argc >= 3){
        if (argv[2][0] == 'i')
            is_interative = true;
    }
    std::string full_path = "../tests/" + filename;
    std::ifstream file(full_path);
    if (!file) {
        std::cerr << "Não foi possível abrir o arquivo.\n";
        return 1;
    }
    
    if (!std::getline(file, line)) return 1;
    n_iter = std::stoi(line);
    // Lê o número de linhas e colunas a partir do arquivo de testes
    if (!std::getline(file, line)) return 1;
    rows = std::stoi(line);
    if (!std::getline(file, line)) return 1;
    cols = std::stoi(line);

    std::unordered_set<std::pair<int, int>, pair_hash> next;
    std::unordered_set<std::pair<int, int>, pair_hash> current;

    // Seta o estado inicial a partir do arquivo de testes
    auto time = omp_get_wtime();
    while (std::getline(file, line)) {
        int x, y;
        char ignore;

        std::stringstream ss(line);
        ss >> ignore >> x >> ignore >> y >> ignore;
        std::pair<int, int> coordinate = {x, y};
        current.emplace(coordinate);
    }

    
    log_time("startup", time);
    //printMatrix(current, rows, cols);
    log_start_prop("interations");
    omp_set_num_threads(NUM_THREADS);

    for (int i = 0; i < n_iter || is_interative; ++i)
    {
        log_start_arr_item();

        if (is_interative) {
            printMatrix(current, rows, cols);
            std::cout << "Pressione Enter para continuar...\n";
            auto r = std::cin.get();
            if (r == EOF) {
                std::cout << "Saindo...\n";
                break;
            }
            if (r == 'q') {
                std::cout << "Saindo...\n";
                break;
            }
        }
        auto time = omp_get_wtime();

        process(current, next, rows, cols);

        log_time("process", time);
        if(!writeOutput(omp_get_wtime() - time, n_iter, filename))
            std::cerr << "Falha ao escrever arquivo de saída";

        std::swap(current, next);
        next.clear();
        log_end();

    }
    log_end();
    time = omp_get_wtime();
    printMatrix(current, rows, cols);
    log_time("print", time);
    log_time("total", time_total);

    return 0;
}
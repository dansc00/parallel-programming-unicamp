# include <fstream>
# include <iostream>
# include <memory>
# include <sstream>
# include <string>
# include <utility>
# include <omp.h>
# include <vector>
//# include <gperftools/profiler.h>
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

void printMatrix(std::unique_ptr<bool[]>& matrix, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int coordinate = 1 + (i+1)*(cols+2) + j;
            if (matrix[coordinate] == true) {
                std::cout << "X" << " ";
            } else {
                std::cout << "." << " ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << "\n" << std::endl;
}

typedef std::vector<std::pair<int, int>> CellVector;

void reduce(std::vector<CellVector> &vec, CellVector &ret ) {
    std::vector<int> positions(omp_get_max_threads());
    int position = 0;
    for (int i = 0; i < omp_get_max_threads(); ++i) {
        positions[i] = position;
        position += vec[i].size();
    }
    ret.resize(position);
    #pragma omp parallel for
    for (int i = 0; i < vec.size(); ++i) {
        
        for (int k = 0; k < vec[i].size(); ++k) {
            ret[positions[i]+k] = vec[i][k];
        }
    }
}


void process(bool *cells, bool *current, CellVector &alives, bool *next, CellVector *next_alives, int rows, int cols)
{
    std::vector<CellVector> aux_vec(omp_get_max_threads());
    std::vector<CellVector> next_alive_vec(omp_get_max_threads());
    CellVector aux;
    for (int k = 0; k < alives.size(); ++k) {
        cells[1 + (alives[k].first+1)*(cols+2) + alives[k].second] = true;
    }
    
    auto time = omp_get_wtime();
    #pragma omp parallel for
    for (int k = 0; k < alives.size(); ++k) {
        int tid = omp_get_thread_num();
        auto alive = alives[k];
        int i = alive.first;
        int j = alive.second;
        int coordinate = 1 + (i+1)*(cols+2) + j;
        int score = 0;
        for (int ii = -1; ii <= 1; ++ii) {
            for (int jj = -1; jj <= 1; ++jj) {
                if (ii != 0 || jj != 0) {
                    int coord_to_check = 1 + (i+ii+1)*(cols+2) + j + jj;
                    if(!cells[coord_to_check]) {
                        aux_vec[tid].emplace_back(i+ii, j+jj);
                        cells[coord_to_check] = true; 
                    }
                    if (current[coord_to_check]) score++;
                }
            }
        }

        // Uma célula continua viva se houver entre 2 ou 3 vizinhos vivos
        // ou se estiver morta e houver exatamente 3 vizinhos vivos
        if ((current[coordinate] && (score == 2 || score == 3)) ||
            (!current[coordinate] && score == 3)) {
            next_alive_vec[tid].emplace_back(i, j);
            next[coordinate] = true;
        }
    }

    std::cerr << "Time to update alive cells: " << omp_get_wtime() - time << " seconds\n";
    time = omp_get_wtime();
    reduce(aux_vec, aux);
    std::cerr << "Time to reduce aux: " << omp_get_wtime() - time << " seconds\n";
    time = omp_get_wtime();
    #pragma omp parallel for
    for (int k = 0; k < aux.size(); ++k) {
        int tid = omp_get_thread_num();
        auto cell = aux[k];
        int i = cell.first;
        int j = cell.second;
        int coordinate = 1 + (i+1)*(cols+2) + j;
        int score = 0;
        for (int ii = -1; ii <= 1; ++ii) {
            for (int jj = -1; jj <= 1; ++jj) {
                if (ii != 0 || jj != 0) {
                    int coord_to_check = 1 + (i+ii+1)*(cols+2) + j + jj;
                    if (current[coord_to_check]) score++;
                }
            }
        }

        // Uma célula continua viva se houver entre 2 ou 3 vizinhos vivos
        // ou se estiver morta e houver exatamente 3 vizinhos vivos
        if ((current[coordinate] && (score == 2 || score == 3)) ||
            (!current[coordinate] && score == 3)) {
            next_alive_vec[tid].emplace_back(i, j);
            next[coordinate] = true;
        }
    }
    std::cerr << "Time to process aux: " << omp_get_wtime() - time << " seconds\n";
    time = omp_get_wtime();
    reduce(next_alive_vec, *next_alives);
    std::cerr << "Time to reduce next alive: " << omp_get_wtime() - time << " seconds\n";
}

int main(int argc, char* argv[]) {
    //ProfilerStart("../gperftools/serial-matrix.prof");
    int rows, cols, n_iter, matrix_size;
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

    matrix_size = (rows + 2) * (cols + 2);
    auto next = std::make_unique<bool[]>(matrix_size);
    auto current = std::make_unique<bool[]>(matrix_size);
    auto cells = std::make_unique<bool[]>((rows + 2) * (cols + 2));
    for (int i = 0; i < (rows + 2)*(cols + 2); ++i) {
        next[i] = false;
        current[i] = false;
    }
    auto time = omp_get_wtime();
    // Seta o estado inicial a partir do arquivo de testes
    std::vector<std::pair<int, int>> alives;
    std::vector<std::pair<int, int>> next_alives;
    while (std::getline(file, line)) {
        int x, y;
        char ignore;

        std::stringstream ss(line);
        ss >> ignore >> x >> ignore >> y >> ignore;
        int coordinate = 1 + (x+1)*(cols+2) + y;
        alives.emplace_back(x, y);
        current[coordinate] = true;
    }
    std::cerr << "Time to read initial state: " << omp_get_wtime() - time << " seconds\n";

    //printMatrix(current, rows, cols);
    for (int i = 0; i < n_iter || is_interative; ++i) {

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
        process(cells.get(), current.get(), alives, next.get(), &next_alives, rows, cols);
        std::swap(current, next);
        std::swap(alives, next_alives);
        next_alives.clear();
        std::fill(next.get(), next.get()+matrix_size, false);
        std::fill(cells.get(), cells.get()+matrix_size, false);
    }
    time = omp_get_wtime();
    printMatrix(current, rows, cols);
    std::cerr << "Time to print: " << omp_get_wtime() - time << " seconds\n";
    //ProfilerStop();

    return 0;
}
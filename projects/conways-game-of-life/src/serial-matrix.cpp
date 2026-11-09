# include <fstream>
# include <iostream>
# include <memory>
# include <sstream>
# include <string>
# include <utility>
# include <omp.h>
#include <filesystem>
#define NUM_THREADS 1
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


void process(bool *current, bool *next, int rows, int cols) {
    auto time = omp_get_wtime();
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
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

                next[coordinate] = true;
            }
        }
    }
    log_time("process", time);
}

bool writeOutput(double time, int n_iter, std::string filename) {
    if (n_iter <= 0) {
        std::cerr << "Erro ao gerar arquivo de saída: n_iter inválido\n";
        return false;
    }   

    std::string out_dir = "../serial-outs/matrix/";
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
        std::cerr << "Não foi possível abrir o arquivo\n";
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
    for (int i = 0; i < (rows + 2)*(cols + 2); ++i) {
        next[i] = false;
        current[i] = false;
    }

    // Seta o estado inicial a partir do arquivo de testes
    auto time = omp_get_wtime();
    while (std::getline(file, line)) {
        int x, y;
        char ignore;

        std::stringstream ss(line);
        ss >> ignore >> x >> ignore >> y >> ignore;
        int coordinate = 1 + (x+1)*(cols+2) + y;
        current[coordinate] = true;
    }
    log_time("startup_read", time);
    log_time("startup", time);
    omp_set_num_threads(NUM_THREADS);
    
    //printMatrix(current, rows, cols);
    log_start_prop("interations");
    for (int i = 0; i < n_iter || is_interative; ++i)
    {
        log_start_arr_item();
        if (is_interative)
        {

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
        time = omp_get_wtime();
        process(current.get(), next.get(), rows, cols);
        if(!writeOutput(omp_get_wtime() - time, n_iter, filename))
            std::cerr << "Falha ao escrever arquivo de saída";

        std::swap(current, next);
        std::fill(next.get(), next.get()+matrix_size, false);
        // printMatrix(current, rows, cols);
        log_end();
    }
    log_end();

    time = omp_get_wtime();
    printMatrix(current, rows, cols);
    log_time("print", time);
    log_time("total", time_total);

    return 0;
}
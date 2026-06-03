#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Erro: Forneca o arquivo de log do traceroute.\n";
        cerr << "Uso: " << argv[0] << " input_1.log\n"; 
        return 1;
    }

    string filename = argv[1];
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Erro: Nao foi possivel abrir o arquivo '" << filename << "' para leitura.\n";
        return 1;
    }

    cout << "Inicializando leitura de rotas a partir de: " << filename << "...\n";
    
    // TODO: (no proximo commit eu continuo)

    file.close();
    return 0;
}
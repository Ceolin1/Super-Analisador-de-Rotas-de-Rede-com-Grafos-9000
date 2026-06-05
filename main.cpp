#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct RotaValida {
    string origem;
    string destino;
};

vector<RotaValida> extrair_rotas(const string& filename) {
    vector<RotaValida> rotas;
    ifstream file(filename);
    string line;

    if (!file.is_open()) {
        cerr << "Erro: Nao foi possivel abrir o arquivo '" << filename << "'.\n";
        return rotas;
    }

    if (!getline(file, line)) {
        return rotas;
    }

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        vector<string> colunas;

        while (getline(ss, token, ',')) {
            colunas.push_back(token);
        }

        if (colunas.size() < 6) continue;

        string hop_from = colunas[4];
        string hop_to = colunas[5];

        if (hop_to == "*") continue;

        if (hop_from.empty() || hop_to.empty()) continue;

        RotaValida rota_limpa = {hop_from, hop_to};
        rotas.push_back(rota_limpa);
    }

    file.close();
    return rotas;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Erro: Forneca o arquivo de log do traceroute.\n";
        cerr << "Uso: " << argv[0] << " input_1.log\n";
        return 1;
    }

    string filename = argv[1];
    cout << "Inicializando leitura de rotas a partir de: " << filename << "...\n";
    
    vector<RotaValida> rotas_filtradas = extrair_rotas(filename);

    if (rotas_filtradas.empty()) {
        cerr << "Erro: Nenhuma rota valida encontrada no arquivo.\n";
        return 1;
    }

    cout << "Leitura concluida com sucesso!\n";
    cout << "Total de conexoes validas e filtradas: " << rotas_filtradas.size() << "\n";
    
    return 0;
}
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

struct RotaValida {
    string origem;
    string destino;
};

struct Grafo {
    unordered_map<string, vector<string>> lista_adj;
    int num_arestas = 0;
};

void adicionar_aresta(Grafo& g, const string& origem, const string& destino) {
    auto& vizinhos = g.lista_adj[origem];
    
    if (find(vizinhos.begin(), vizinhos.end(), destino) == vizinhos.end()) {
        vizinhos.push_back(destino);
        g.num_arestas++;
    }
    
    if (g.lista_adj.find(destino) == g.lista_adj.end()) {
        g.lista_adj[destino] = vector<string>();
    }
}

vector<RotaValida> extrair_rotas(const string& filename) {
    vector<RotaValida> rotas;
    ifstream file(filename);
    string line;

    if (!file.is_open()) return rotas;
    if (!getline(file, line)) return rotas;

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        vector<string> colunas;

        while (getline(ss, token, ',')) colunas.push_back(token);

        if (colunas.size() < 6) continue;

        string hop_from = colunas[4];
        string hop_to = colunas[5];

        if (hop_to == "*") continue;
        if (hop_from.empty() || hop_to.empty()) continue;

        rotas.push_back({hop_from, hop_to});
    }

    file.close();
    return rotas;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Erro: Forneca o arquivo de log.\nUso: " << argv[0] << " input_1.log\n";
        return 1;
    }

    string filename = argv[1];
    vector<RotaValida> rotas_filtradas = extrair_rotas(filename);

    if (rotas_filtradas.empty()) {
        cerr << "Erro: Nenhuma rota valida encontrada.\n";
        return 1;
    }

    Grafo grafo_rede;
    
    for (const auto& rota : rotas_filtradas) {
        adicionar_aresta(grafo_rede, rota.origem, rota.destino);
    }

    cout << "Grafo de roteamento inicializado!\n";
    cout << "Vertices unicos (IPs): " << grafo_rede.lista_adj.size() 
         << " | Arestas: " << grafo_rede.num_arestas << "\n";
    
    return 0;
}
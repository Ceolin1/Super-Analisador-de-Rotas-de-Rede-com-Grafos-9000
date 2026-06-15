#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cstdlib> 

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

void exportar_grafo(const Grafo& g, const string& log_filename, int formato) {
    string dot_filename = log_filename + ".dot";
    ofstream file(dot_filename);
    
    if (!file.is_open()) {
        cerr << "Erro ao criar arquivo .dot\n";
        return;
    }
    
    file << "digraph G {\n";
    for (const auto& par : g.lista_adj) {
        const string& origem = par.first;
        for (const string& destino : par.second) {
            file << "  \"" << origem << "\" -> \"" << destino << "\";\n";
        }
    }
    file << "}\n";
    file.close();

    string output_file;
    string command;

    if (formato == 1) { // Tela
        output_file = log_filename + ".png";
        command = "dot -Tpng " + dot_filename + " -o " + output_file;
        system(command.c_str());
        
        // Comando condicional de SO para abrir a imagem automaticamente que eu achei na internet
        #ifdef _WIN32
            command = "start " + output_file;
        #elif __APPLE__
            command = "open " + output_file;
        #else
            command = "xdg-open " + output_file;
        #endif
        system(command.c_str());
    } 
    else if (formato == 2) { // PNG
        output_file = log_filename + ".png";
        command = "dot -Tpng " + dot_filename + " -o " + output_file;
        system(command.c_str());
        cout << "Arquivo " << output_file << " gerado com sucesso\n";
    } 
    else if (formato == 3) { // PDF
        output_file = log_filename + ".pdf";
        command = "dot -Tpdf " + dot_filename + " -o " + output_file;
        system(command.c_str());
        cout << "Arquivo " << output_file << " gerado com sucesso\n";
    } 
    else {
        cout << "Formato invalido!\n";
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
    
    int opcao;
    do {
        cout << "\n======================================================\n";
        cout << "1. Exibir Grafo Completo\n";
        cout << "2. Encontrar Menor Caminho\n";
        cout << "3. Calcular o Diametro do Grafo\n";
        cout << "4. Identificar Roteadores Criticos\n";
        cout << "0. Sair\n";
        cout << "======================================================\n";
        cout << "Escolha uma opcao: ";
        
        if (!(cin >> opcao)) {
            cin.clear();
            cin.ignore(10000, '\n');
            opcao = -1;
        }

        switch(opcao) {
            case 1: {
                cout << "Selecione o formato de saida do Graphviz:\n";
                cout << "1. Tela\n2. Imagem (PNG)\n3. Documento (PDF)\nOpcao: ";
                int form;
                if (cin >> form) {
                    exportar_grafo(grafo_rede, filename, form);
                } else {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Entrada invalida.\n";
                }
                break;
            }
            case 2: cout << "\n(Algoritmo de Menor Caminho)\n"; break;
            case 3: cout << "\n(Calculo de Diametro)\n"; break;
            case 4: cout << "\n(Identificar hubs de rede)\n"; break;
            case 0: cout << "\nSaindo da aplicacao...\n"; break;
            default: cout << "\nOpcao invalida! Tente novamente.\n";
        }
    } while (opcao != 0);

    return 0;
}
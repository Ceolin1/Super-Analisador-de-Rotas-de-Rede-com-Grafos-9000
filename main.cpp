#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cstdlib>
#include <queue>

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

vector<string> encontrar_menor_caminho(const Grafo& g, const string& origem, const string& destino) {
    if (g.lista_adj.find(origem) == g.lista_adj.end() || g.lista_adj.find(destino) == g.lista_adj.end()) {
        return {};
    }

    queue<vector<string>> q;
    unordered_map<string, bool> visitado;

    q.push({origem});
    visitado[origem] = true;

    while (!q.empty()) {
        vector<string> caminho = q.front();
        q.pop();

        string vertice_atual = caminho.back();

        if (vertice_atual == destino) {
            return caminho;
        }

        if (g.lista_adj.find(vertice_atual) != g.lista_adj.end()) {
            for (const string& vizinho : g.lista_adj.at(vertice_atual)) {
                if (!visitado[vizinho]) {
                    visitado[vizinho] = true;
                    vector<string> novo_caminho = caminho;
                    novo_caminho.push_back(vizinho);
                    q.push(novo_caminho);
                }
            }
        }
    }
    return {};
}

int calcular_diametro(const Grafo& g) {
    int max_diametro = 0;

    for (const auto& par : g.lista_adj) {
        const string& origem = par.first;
        queue<pair<string, int>> q;
        unordered_map<string, bool> visitado;

        q.push({origem, 0});
        visitado[origem] = true;

        while (!q.empty()) {
            string vertice_atual = q.front().first;
            int dist = q.front().second;
            q.pop();

            if (dist > max_diametro) {
                max_diametro = dist;
            }

            if (g.lista_adj.find(vertice_atual) != g.lista_adj.end()) {
                for (const string& vizinho : g.lista_adj.at(vertice_atual)) {
                    if (!visitado[vizinho]) {
                        visitado[vizinho] = true;
                        q.push({vizinho, dist + 1});
                    }
                }
            }
        }
    }
    return max_diametro;
}

void identificar_roteadores_criticos(const Grafo& g) {
    unordered_map<string, int> in_degree;
    
    for (const auto& par : g.lista_adj) {
        if (in_degree.find(par.first) == in_degree.end()) {
            in_degree[par.first] = 0;
        }
        for (const string& destino : par.second) {
            in_degree[destino]++;
        }
    }

    vector<pair<string, int>> ranking(in_degree.begin(), in_degree.end());
    
    sort(ranking.begin(), ranking.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
        return a.second > b.second;
    });

    cout << "\nTop 5 Roteadores Criticos (Maior Grau de Entrada):\n";
    int limite = min(5, (int)ranking.size());
    for (int i = 0; i < limite; ++i) {
        cout << i + 1 << ". " << ranking[i].first << " - " << ranking[i].second << " conexoes de entrada\n";
    }
}

void exportar_grafo(const Grafo& g, const string& log_filename, int formato, const vector<string>& caminho = {}) {
    string dot_filename = log_filename + ".dot";
    ofstream file(dot_filename);
    
    if (!file.is_open()) {
        cerr << "Erro ao criar arquivo .dot\n";
        return;
    }
    
    file << "digraph G {\n";
    
    for (const string& ip : caminho) {
        file << "  \"" << ip << "\" [style=filled, fillcolor=lightblue, color=blue, penwidth=2.0];\n";
    }

    for (const auto& par : g.lista_adj) {
        const string& origem = par.first;
        for (const string& destino : par.second) {
            bool in_path = false;
            for (size_t i = 0; i + 1 < caminho.size(); ++i) {
                if (caminho[i] == origem && caminho[i+1] == destino) {
                    in_path = true;
                    break;
                }
            }

            if (in_path) {
                file << "  \"" << origem << "\" -> \"" << destino << "\" [color=red, penwidth=2.0];\n";
            } else {
                file << "  \"" << origem << "\" -> \"" << destino << "\";\n";
            }
        }
    }
    file << "}\n";
    file.close();

    string output_file;
    string command;

    if (formato == 1) {
        output_file = log_filename + ".png";
        command = "dot -Tpng " + dot_filename + " -o " + output_file;
        system(command.c_str());

        // Comando condicional de SO para abrir a imagem automaticamente
        #ifdef _WIN32
            command = "start " + output_file;
        #elif __APPLE__
            command = "open " + output_file;
        #else
            command = "xdg-open " + output_file;
        #endif
        system(command.c_str());
    } 
    else if (formato == 2) {
        output_file = log_filename + ".png";
        command = "dot -Tpng " + dot_filename + " -o " + output_file;
        system(command.c_str());
        cout << "Arquivo " << output_file << " gerado com sucesso\n";
    } 
    else if (formato == 3) {
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
            case 2: {
                string ip_origem, ip_destino;
                cout << "Digite o IP de Origem: ";
                cin >> ip_origem;
                cout << "Digite o IP de Destino: ";
                cin >> ip_destino;

                vector<string> caminho = encontrar_menor_caminho(grafo_rede, ip_origem, ip_destino);

                if (caminho.empty()) {
                    cout << "Alerta: Nao ha conectividade entre " << ip_origem << " e " << ip_destino << ".\n";
                } else {
                    int saltos = caminho.size() - 1;
                    cout << "Caminho encontrado (" << saltos << " saltos):\n";
                    for (size_t i = 0; i < caminho.size(); ++i) {
                        cout << caminho[i];
                        if (i < caminho.size() - 1) cout << " -> ";
                    }
                    cout << "\n\nSelecione o formato de saida do Graphviz:\n";
                    cout << "1. Tela\n2. Imagem (PNG)\n3. Documento (PDF)\nOpcao: ";
                    int form;
                    if (cin >> form) {
                        exportar_grafo(grafo_rede, filename, form, caminho);
                    } else {
                        cin.clear();
                        cin.ignore(10000, '\n');
                        cout << "Entrada invalida.\n";
                    }
                }
                break;
            }
            case 3: {
                cout << "\nCalculando diametro da topologia...\n";
                int diametro = calcular_diametro(grafo_rede);
                cout << "O diametro do grafo e: " << diametro << " saltos.\n";
                break;
            }
            case 4: {
                identificar_roteadores_criticos(grafo_rede);
                break;
            }
            case 0: cout << "\nSaindo da aplicacao...\n"; break;
            default: cout << "\nOpcao invalida! Tente novamente.\n";
        }
    } while (opcao != 0);

    return 0;
}
//Fatto da: Francesco Alexander Salcuni , Daniele Tallo, Tommaso Chiti

#include<iostream>
#include<utility>
#include<vector>
#include<queue>

#define INT_MAX 1000000

struct Nodo {
    int id;
    std::vector<std::pair<int,int>> raggiungibili;

    Nodo(int id) : id(id) {}
};

struct Arco {
    int sorgente;
    int destinazione;
    int peso;

    Arco(int sorgente, int destinazione, int peso) 
        : sorgente(sorgente), destinazione(destinazione), peso(peso) {}
};

void inizializzo_il_grafo(std::vector<Nodo>& nodi, const std::vector<Arco>& archi) {
    for (const Arco& arco : archi) {
        nodi[arco.sorgente].raggiungibili.push_back(std::make_pair(arco.destinazione, arco.peso));
        nodi[arco.destinazione].raggiungibili.push_back(std::make_pair(arco.sorgente, arco.peso));
    }
}

void stampa_grafo(const std::vector<Nodo>& nodi) {
    for (const Nodo& nodo : nodi) {
        std::cout << "Nodo: " << nodo.id << std::endl;
        for (const auto& raggiungibile : nodo.raggiungibili) {
            std::cout << "Raggiungibile: " << raggiungibile.first << " con peso: " << raggiungibile.second << std::endl;
        }
    }
}

void algoritmo_Prim(const std::vector<Nodo>& nodi) {
    int n = nodi.size();
    std::vector<int> costi_minimi(n, INT_MAX );
    std::vector<bool> inclusi(n, false);
    std::vector<int> padre(n, -1);
    std::priority_queue<std::pair<int,int>, std::vector<std::pair<int,int>>, std::greater<std::pair<int,int>>> codaprior;
    costi_minimi[0] = 0;
    codaprior.push(std::make_pair(0, 0));

    while(!codaprior.empty()) {
        int u = codaprior.top().second;
        codaprior.pop();
        if(inclusi[u]) continue;
        inclusi[u] = true;

        for(const auto& [v,peso] : nodi[u].raggiungibili) {
            if(!inclusi[v] && peso < costi_minimi[v]) {
                costi_minimi[v] = peso;
                padre[v] = u;
                codaprior.push(std::make_pair(costi_minimi[v], v));
            }
        }
    }

    std::cout << "Albero di copertura minimo:" << std::endl;
    for(int i = 1; i < n; ++i) {
        if(padre[i]!=-1){
            std::cout<< "Arco: " << padre[i] << "-" << i << "con il peso:" << costi_minimi[i] << std::endl;
        }
    }

}



int main() {
    std::vector<Nodo> nodi;
    std::vector<Arco> archi;

    for (int i = 0; i <= 9; ++i) {
        nodi.emplace_back(i);
    }

    archi.emplace_back(0, 1, 1);
    archi.emplace_back(0, 2, 1);
    archi.emplace_back(0, 3, 1);
    archi.emplace_back(1, 4, 1);
    archi.emplace_back(1, 5, 1);
    archi.emplace_back(2, 6, 1);
    archi.emplace_back(2, 5, 6); // 
    archi.emplace_back(2, 7, 1);
    archi.emplace_back(3, 8, 1);
    archi.emplace_back(3, 9, 1);
    archi.emplace_back(8, 9, 7); //

    inizializzo_il_grafo(nodi, archi);
    stampa_grafo(nodi);
    algoritmo_Prim(nodi);

    return 0;
}



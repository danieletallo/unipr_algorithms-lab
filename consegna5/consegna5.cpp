// Fatto da Daniele Tallo, Francesco Alexander Salcuni e Tommaso Chiti

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

const int INFTY = 1000000;
const int N_NODES = 6;

typedef struct node {
    int val;
    int w;
    struct node* next;
} node_t;

typedef struct list {
    node_t* head;
} list_t;

list_t* graph[N_NODES];
int V_dist[N_NODES];
int V_prev[N_NODES];
bool V_visitato[N_NODES];
int heap[N_NODES];
int heap_elements[N_NODES];
int heap_size = 0; /// dimensione attuale dell'heap

void init_graph(int n) {
    for (int i = 0; i < n; ++i) {
        graph[i] = new list_t;
        graph[i]->head = NULL;
        V_dist[i] = INFTY;
        V_prev[i] = -1;
        V_visitato[i] = false;
    }
}

void add_edge(int u, int v, int w) {
    node_t* new_node = new node_t;
    new_node->val = v;
    new_node->w = w;
    new_node->next = graph[u]->head;
    graph[u]->head = new_node;

    // aggiungo l'altro nel "senso inverso" così da avere un grafo non orientato
    new_node = new node_t;
    new_node->val = u;
    new_node->w = w;
    new_node->next = graph[v]->head;
    graph[v]->head = new_node;
}

void print_graph(int n) {
    for (int i = 0; i < n; ++i) {
        cout << "Nodo: " << i << endl;
        node_t* temp = graph[i]->head;

        while (temp != NULL) {
            cout << "  Raggiungibile: " << temp->val << " con peso: " << temp->w << endl;
            temp = temp->next;
        }
    }
}

// int extract_min(int n) {
//     int min_index = -1;
//     int min_value = INFTY;

//     for (int i = 0; i < n; ++i) {
//         if (V_visitato[i] == false && V_dist[i] < min_value) {
//             min_value = V_dist[i];
//             min_index = i;
//         }
//     }

//     return min_index;
// }

// Inizio Gestione estrazione min tramite min heap (come in consegna4)

int child_L_idx(int n) {
    if (2 * n + 1 >= heap_size)
        return -1;
    return 2 * n + 1;
}

int is_leaf(int n) {
    return (child_L_idx(n) == -1);
}

int parent_idx(int n) {
    if (n == 0)
        return -1;
    return (n - 1) / 2;
}

int child_R_idx(int n) {
    if (2 * n + 2 >= heap_size)
        return -1;
    return 2 * n + 2;
}

void heapmin_insert(int elem) { 
    if (heap_size < N_NODES) {
        int i = heap_size;
        heap_size++;

        heap[i] = V_dist[elem];
        heap_elements[i] = elem;

        while (i != 0) {                          // non sono sulla radice
            if (heap[parent_idx(i)] <= heap[i]) { /// proprieta' dell' heap e' rispettata -> esco
                return;
            }

            /// il nodo ha un genitore sicuramente >   --> swap
            // scambio anche gli elementi
            int t = heap[parent_idx(i)];
            int t_elem = heap_elements[parent_idx(i)];
            heap[parent_idx(i)] = heap[i];
            heap_elements[parent_idx(i)] = heap_elements[i];
            heap[i] = t;
            heap_elements[i] = t_elem;
            i = parent_idx(i);
        }

    } else
        printf("Heap pieno!\n");
}

int heap_remove_min() {

    if (heap_size <= 0) {
        printf("Errore: heap vuoto\n");
        return -1;
    }

    int minimo = heap[0];

    // faccio swap degli elementi e delle distanze
    int t = heap[0];
    int t_elem = heap_elements[0];
    heap[0] = heap[heap_size - 1];
    heap_elements[0] = heap_elements[heap_size - 1];
    heap[heap_size - 1] = t;
    heap_elements[heap_size - 1] = t_elem;

    // elimino il minimo (ora in fondo all'array)
    heap_size--;
    int i = 0; // indice di lavoro (parto dalla root)

    while (!is_leaf(i)) { /// garantisco di fermarmi alla foglia

        int con_chi_mi_scambio = -1;

        /// controllo il nodo i con il suo figlio L
        if (heap[i] > heap[child_L_idx(i)]) { // il nodo i e' piu' grande
            /// attivare uno swap (la proprieta' heap non e' rispettata)
            con_chi_mi_scambio = child_L_idx(i);
            
            if (child_R_idx(i) >= 0 && // esiste il nodo destro
                heap[child_L_idx(i)] > heap[child_R_idx(i)]) {
                con_chi_mi_scambio = child_R_idx(i);
            }
        } else { // il nodo e' piu' piccolo del figlio L -> non faccio swap col sx ma controllo comunque il dx

            if (child_R_idx(i) >= 0) {                // esiste il figlio R
                if (heap[i] > heap[child_R_idx(i)]) { /// attivo lo swap
                    con_chi_mi_scambio = child_R_idx(i);
                } else
                    break;
            } else
                break;
        }

        /// swap tra i e con_chi_mi_scambio
        // scambio anche gli elementi
        int t = heap[i];
        int t_elem = heap_elements[i];
        heap[i] = heap[con_chi_mi_scambio];
        heap_elements[i] = heap_elements[con_chi_mi_scambio];
        heap[con_chi_mi_scambio] = t;
        heap_elements[con_chi_mi_scambio] = t_elem;
        i = con_chi_mi_scambio;
    }

    return minimo;
}

// Fine Gestione estrazione min tramite min heap

void prim(int n) {
    V_dist[0] = 0;
    heapmin_insert(0);

    for (int i = 0; i < n; ++i) {
        // int u = extract_min(n);
        int u = heap_elements[0];   // prendo l'elemento con distanza minima
        heap_remove_min();          // rimuovo l'elemento minimo
        if (u == -1) break;     // non ho trovato minimi
        V_visitato[u] = true;

        node_t* temp = graph[u]->head;
        while (temp != NULL) {
            int v = temp->val;
            int w = temp->w;

            if (V_visitato[v] == false && w < V_dist[v]) {
                V_dist[v] = w;
                heapmin_insert(v);
                V_prev[v] = u;
            }
            temp = temp->next;
        }
    }

    cout << "Albero di copertura minimo:" << endl;
    for (int i = 1; i < n; ++i) {
        if (V_prev[i] != -1) {
            cout << "Arco: " << V_prev[i] << " - " << i << " con il peso: " << V_dist[i] << endl;
        }
    }
}

int main() {
    init_graph(N_NODES);

    add_edge(0, 1, 1);
    add_edge(0, 3, 12);
    add_edge(0, 4, 6);
    add_edge(0, 5, 5);
    add_edge(0, 2, 3);
    add_edge(1, 3, 13);
    add_edge(2, 4, 14);
    add_edge(2, 5, 7);
    add_edge(3, 4, 3);
    add_edge(4, 5, 2);

    print_graph(N_NODES);
    prim(N_NODES);

    return 0;
}
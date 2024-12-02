// Fatto da Daniele Tallo, Francesco Alexander Salcuni e Tommaso Chiti

#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

/// struct per il nodo della lista
typedef struct node {
    int val; /// prossimo nodo
    float w; /// peso dell'arco
    struct node *next;
} node_t;

/// struct per la lista
typedef struct list {
    node *head;
} list_t;

const int INFTY = 1000000;
const int MAX_SIZE = 256; /// allocazione statica
int heap[MAX_SIZE];
int heap_elements[MAX_SIZE];
int heap_size = 0; /// dimensione attuale dell'heap
int details = 0;

int *V;          // elenco dei nodi del grafo
int *V_visitato; // nodo visitato?
int *V_prev;     // nodo precedente dalla visita
float *V_dist;   // distanza da sorgente
list_t **E; /// array di puntatori a le liste di adiacenza per ogni nodo
int n_nodi;
list_t *global_ptr_ref = nullptr;

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
    if (heap_size < MAX_SIZE) {
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


void increase_key(int indice_nodo, int key) {

    if (indice_nodo < 0 || indice_nodo >= heap_size) {
        printf("Nodo non esistente\n");
        return;
    }

    if (heap[indice_nodo] > key) {
        printf("la chiave non e' piu' grande!\n");
        return;
    }

    heap[indice_nodo] = key;

    int i = indice_nodo;
    while (i != 0) {                          // non sono sulla radice
        if (heap[parent_idx(i)] >= heap[i]) { /// proprieta' dell' heap e' rispettata -> esco
            if (details)
                printf("Il genitore ha valore %d >= del nodo %d, esco\n", heap[parent_idx(i)], heap[i]);
            return;
        }

        if (details)
            printf("Il genitore ha valore %d < del nodo %d, swap\n", heap[parent_idx(i)], heap[i]);
        /// il nodo ha un genitore sicuramente <   --> swap
        int t = heap[parent_idx(i)];
        heap[parent_idx(i)] = heap[i];
        heap[i] = t;
        i = parent_idx(i);
    }
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

int get_address(void *node) {
    return (int)((long)node - (long)global_ptr_ref);
}

list_t *list_new() {
    list_t *l = new list;
    if (details) {
        printf("Lista creata\n");
    }

    l->head = NULL; //// perche' non e' l.head ?
    if (details) {
        printf("Imposto a NULL head\n");
    }

    return l;
}

void list_insert_front(list_t *l, int elem, float w) {
    node_t *new_node = new node_t;
    new_node->next = NULL;
    new_node->val = elem;
    new_node->w = w;
    new_node->next = l->head;
    l->head = new_node;
}

void shortest_path(int n) {

    V_dist[n] = 0;
    heapmin_insert(n);

    int q_size = n_nodi; /// contatore degli elementi in coda (V_visitato)

    while (q_size != 0) {
        /// trova il minimo in coda
        float best_dist = INFTY;
        int best_idx = -1;

        best_idx = heap_elements[0];
        best_dist = heap_remove_min();

        if (best_idx >= 0) {
            /// estrai dalla coda
            int u = best_idx;
            V_visitato[u] = 1;
            q_size--;

            /// esploro la lista di adiacenza
            node_t *elem = E[u]->head;
            while (elem != NULL) {
                int v = elem->val; /// arco u --> v

                /// alt ← dist[u] + Graph.Edges(u, v)
                float alt = V_dist[u] + elem->w; /// costo per arrivare al nuovo nodo passando per u
                if (alt < V_dist[v]) {           // il percorso sorgente ---> u --> v migliora il percorso attuale sorgente --> v
                    V_dist[v] = alt;
                    heapmin_insert(v);
                    V_prev[v] = u;
                }
                elem = elem->next;
            }

        } else { /// coda non vuota E nodi non raggiungibili ---> FINITO
            q_size = 0;
        }
    }
}

bool bellman_ford(int source_node) {
    int n_nodes = n_nodi;

    // Esegui n_nodes - 1 iterazioni per il Bellman-Ford
    for (int i = 0; i < n_nodes - 1; i++) {
        bool need_to_relax = false;

        // Esegui shortest_path per trovare percorsi minimi
        shortest_path(source_node);

        // Controlla se ci saranno relax
        for (int u = 0; u < n_nodes; u++) {
            node_t *elem = E[u]->head;

            while (elem != NULL) {
                int v = elem->val;
                float alt = V_dist[u] + elem->w;
                
                if (alt < V_dist[v]) {
                    need_to_relax = true;
                }
                elem = elem->next;
            }
        }

        // Se non servirà fare nessun relax, il ciclo è ottimale e non c'è bisogno di ulteriori iterazioni
        if (!need_to_relax) {
            break;
        }

        if (i == n_nodes - 2 && need_to_relax == true) {
            printf("Ciclo negativo trovato!\n");
            return true;
        }
    }

    printf("Nessun ciclo negativo!\n");
    return false; 
}


int main() {
    n_nodi = 5;

    // init nodi
    V = new int[n_nodi];
    V_visitato = new int[n_nodi];
    V_prev = new int[n_nodi];
    V_dist = new float[n_nodi];

    // init archi
    E = new list_t *[n_nodi];

    // costruzione grafo
    for (int i = 0; i < n_nodi; i++) {
        V[i] = i;
        V_visitato[i] = 0; // flag = non visitato
        V_prev[i] = -1;    // non c'e' precedente
        V_dist[i] = INFTY; // infinito
        E[i] = list_new();
        if (i == 0) global_ptr_ref = E[i];

    }

    // SHORTEST PATH

    list_insert_front(E[0], 1, 3);
    list_insert_front(E[0], 2, 2);
    list_insert_front(E[1], 3, 4);
    list_insert_front(E[2], 3, 5);
    list_insert_front(E[2], 4, 1);
    list_insert_front(E[3], 4, 1);

    shortest_path(0);

    for (int i = 0; i < n_nodi; i++) {
        printf("Nodo %d, distanza %f\n", V[i], V_dist[i]);
    }



    // BELLMAN-FORD con ciclo negativo

    // list_insert_front(E[0], 1, -3);
    // list_insert_front(E[0], 2, -2);
    // list_insert_front(E[1], 3, -4);
    // list_insert_front(E[2], 3, -5);
    // list_insert_front(E[3], 4, -1);
    // list_insert_front(E[4], 2, -1);

    list_insert_front(E[0], 1, 3);
    list_insert_front(E[0], 2, 2);
    list_insert_front(E[1], 3, 4);
    list_insert_front(E[2], 3, -5);
    list_insert_front(E[2], 4, 1);
    list_insert_front(E[3], 4, 1);

    bellman_ford(0);
    
    for (int i = 0; i < n_nodi; i++) {
        printf("Nodo %d, distanza %f\n", V[i], V_dist[i]);
    }

    return 0;
}

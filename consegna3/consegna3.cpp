// Fatto da Daniele Tallo, Francesco Alexander Salcuni e Tommaso Chiti

#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <algorithm>
#include <vector>

using namespace std;

int details = 0;

int n = 0; /// dimensione dell'array

/// file di output per grafo
int n_operazione = 0; /// contatore di operazioni per visualizzare i vari step
int ct_visit = 0; // contatore durante visita

/// struct per il nodo della lista
typedef struct node {
    int val;
    struct node *next;
} node_t;

/// struct per la lista
typedef struct list {
    node *head;
} list_t;

struct cl_struct {
    int length;
    vector<int> nodes;
};

int *V;          // elenco dei nodi del grafo
int *V_visitato; // nodo visitato?
cl_struct *lunghezza;
list_t **E; /// array di puntatori a le liste di adiacenza per ogni nodo
int n_nodi;

list_t *global_ptr_ref = NULL; /// usato per memorizzare il puntatore alla prima lista allocata

int get_address(void *node) {
    return (int)((long)node - (long)global_ptr_ref);
}

void print_cl_struct(cl_struct &c) {
    cout << "lunghezza: " << c.length << '\n';
    for (auto i : c.nodes)
        cout << i << " ";
    cout << '\n';
}

list_t *list_new(void) {
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


void list_insert_front(list_t *l, int elem) {
    /// inserisce un elemento all'inizio della lista
    node_t *new_node = new node_t;
    new_node->next = NULL;

    new_node->val = elem;

    new_node->next = l->head;

    l->head = new_node;
}

void print_array(int *A, int dim) {
    for (int j = 0; j < dim; j++) {
        printf("%d ", A[j]);
    }
    printf("\n");
}


int has_cycle(int n, int path_length, cl_struct &c) {

    if (details) printf("DFS: lavoro sul nodo %d (visitato %d)\n", n, V_visitato[n]);

    if (V_visitato[n] == 1) {
        c.length = path_length;
        lunghezza[n] = c;
        return 1;  // c'e' il ciclo
    }
        
    if (V_visitato[n] == 2) return 0;  // trovato un nuovo percorso alternativo (non c'e' il ciclo)

    V_visitato[n] = 1; // prima volta che incontro questo nodo
    if (details) printf("Visito il nodo %d (val %d)\n", n, V[n]);

    /// esploro la lista di adiacenza
    int t = 0;
    node_t *elem = E[n]->head;
    while (elem != NULL) { /// elenco tutti i nodi nella lista
        c.nodes.push_back(n);
        t += has_cycle(elem->val, path_length + 1, c);
        elem = elem->next;
    }

    V_visitato[n] = 2; // abbandono il nodo per sempre

    return t;
}

int main(int argc, char **argv) {
    int i, test;

    n_nodi = 5;
    V = new int[n_nodi];          //(int*)malloc(n_nodi*sizeof(int));
    V_visitato = new int[n_nodi]; //(int*)malloc(n_nodi*sizeof(int));
    E = new list_t *[n_nodi]; //(list_t**)malloc(n_nodi*sizeof(list_t*));
    lunghezza = new cl_struct[n_nodi]{};

    // inizializzazione
    for (int i = 0; i < n_nodi; i++) {
        V[i] = 2 * i;
        V_visitato[i] = 0; // flag = non visitato
        E[i] = list_new();
        if (i == 0) global_ptr_ref = E[i];
    }

    // creo il grafo, ad ogni nodo associo le posizioni dei nodi adiacenti
    list_insert_front(E[0], 1);
    list_insert_front(E[1], 0);
    list_insert_front(E[2], 3);
    list_insert_front(E[3], 4);
    list_insert_front(E[4], 2);

    int t = 0;
    for (int i = 0; i < n_nodi; i++) {
        cl_struct c{};
        t += has_cycle(i, 0, c);
        printf("test %d: ciclo = %d\n", i, t);
    }

    int max = 0;
    vector<int> nodes_max{};

    // scorro le varie lunghezze dei cicli associate ai nodi
    // quando trovo la lunghezza massima, salvo i nodi associati al ciclo
    for(int i = 0; i < n_nodi; ++i)
    {
        if (lunghezza[i].length > max)
        {
            max = lunghezza[i].length;
            nodes_max = lunghezza[i].nodes;
        }
    }
        
    cout << "Lunghezza max: " << max << endl;
    cout << "Nodi della lunghezza max : ";
    for(auto node : nodes_max) cout << V[node] << " ";

    return 0;
}

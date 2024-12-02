// Fatto da Daniele Tallo e Francesco Alexander Salcuni

#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

using namespace std;

// compilazione: g++ lezione8-13-tree-dot.c
//

// Alberi binari

// Obiettivo:
// 1) struttura dati, allocazione nodo, inserimento manuale nodi
// 2) creazione albero random

// 3) visita inorder
// 4) visita preorder, postorder

// 5) delete albero
// 6) euler tour: stampa e ricostruzione albero da lista nodi

// 7) flip albero

// 8) height - depth
// 9) isBalanced
// 10) isComplete


int ct_swap = 0;
int ct_cmp = 0;
int ct_op = 0; /// operazioni per la ricerca

int max_dim = 0;
int ntests = 1;
int ndiv = 1;
int details = 0;
int graph = 0;

int n = 0; /// dimensione dell'array

/// file di output per grafo
ofstream output_graph;
int n_operazione = 0; /// contatore di operazioni per visualizzare i vari step

ofstream output_visit;
ifstream input_visit;

//////////////////////////////////////////////////
/// Definizione della struttura dati tree
//////////////////////////////////////////////////

/// struct per il nodo dell'albero
typedef struct node {
    int val;
    struct node *L;
    struct node *R;
} node_t;

//////////////////////////////////////////////////
/// Fine Definizione della struttura dati tree
//////////////////////////////////////////////////

/// Questo e' un modo per stampare l'indirizzo node relativamente ad un altro di riferimento.
/// Permette di ottenere offset di piccola dimensione per essere facilmente visualizzati
/// Nota: il metodo non e' robusto e potrebbe avere comportamenti indesiderati su architetture diverse
/// L'alternativa corretta' e' utilizzare %p di printf: es. printf("%p\n",(void*) node);
/// con lo svantaggio di avere interi a 64 bit poco leggibili

node_t *global_ptr_ref = NULL; /// usato per memorizzare il puntatore alla prima lista allocata

int get_address(void *node) {
    if (node == NULL)
        return 0;
    return (int)((long)node - (long)global_ptr_ref);
}

/// stampa il codice del nodo per dot
void print_node_code(node_t *n) {
    output_graph << "node_" << get_address(n) << "_" << n_operazione;
}

void node_print_graph(node_t *n) {

    print_node_code(n);
    output_graph << "\n[label=<\n<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\" >\n<TR> <TD CELLPADDING=\"3\" BORDER=\"0\"  ALIGN=\"LEFT\" bgcolor=\"#f0f0f0\" PORT=\"id\">";
    output_graph << get_address(n) << "</TD> </TR><TR>\n<TD PORT=\"val\" bgcolor=\"#a0FFa0\">";
    output_graph << n->val << "</TD>\n <TD PORT=\"L\" ";
    if (n->L == NULL)
        output_graph << "bgcolor=\"#808080\"> NULL";
    else
        output_graph << "> " << get_address(n->L);
    output_graph << "</TD>\n <TD PORT=\"R\" ";
    if (n->R == NULL)
        output_graph << "bgcolor=\"#808080\"> NULL";
    else
        output_graph << "> " << get_address(n->R);
    output_graph << "</TD>\n</TR></TABLE>>];\n";

    /// visualizzazione figli sullo stesso piano
    if (n->L != NULL && n->R != NULL) {
        output_graph << "rank = same; ";
        print_node_code(n);
        output_graph << ";";
        print_node_code(n->L);
        output_graph << ";\n";
    }

    // mostro archi uscenti

    if (n->L != NULL) { /// disegno arco left
        print_node_code(n);
        output_graph << ":L:c -> ";
        print_node_code(n->L);
        output_graph << ":id ;\n";
    }

    if (n->R != NULL) { /// disegno arco R
        print_node_code(n);
        output_graph << ":R:c -> ";
        print_node_code(n->R);
        output_graph << ":id ;\n";
    }
}

void tree_print_rec_graph(node_t *n) {
    if (n != NULL) {
        node_print_graph(n);
        tree_print_rec_graph(n->L);
        tree_print_rec_graph(n->R);
    }
}

void tree_print_graph(node_t *n) {
    /// stampa ricorsiva del nodo
    tree_print_rec_graph(n);
    n_operazione++;
}

void printTreeStructure(node_t *root, int level = 0) {
    if (root == NULL)
        return;

    // Stampa il nodo corrente
    for (int i = 0; i < level; ++i)
        cout << "|  ";
    cout << "|--" << root->val << endl;

    // Stampa i sottoalberi
    printTreeStructure(root->L, level + 1);
    printTreeStructure(root->R, level + 1);
}

void node_print(node_t *n) {
    if (n == NULL)
        printf("Puntatore vuoto\n");
    else
        printf("allocato in %d [Val: %d, L: %d, R: %d]\n",
               get_address(n),
               n->val,
               get_address(n->R),
               get_address(n->L));
}

node_t *node_new(int elem) { /// crea nuovo nodo
    node_t *t = new node_t;
    if (details) {
        printf("nodo creato\n");
    }

    t->val = elem;
    t->L = NULL;
    t->R = NULL;
    if (details) {
        printf("Imposto a NULL children\n");
    }

    return t;
}

void tree_insert_child_L(node_t *n, int elem) {
    /// inserisco il nuovo nodo con contenuto elem
    /// come figlio Left del nodo n

    /// creo nodo
    n->L = node_new(elem);
}

void tree_insert_child_R(node_t *n, int elem) {
    /// inserisco il nuovo nodo con contenuto elem
    /// come figlio Right del nodo n
    n->R = node_new(elem);
}

int max_nodes = 10;
int n_nodes = 0;

void insert_random_rec(node_t *n) {
    //// inserisce in modo random un nodo L e R e prosegue ricorsivamente
    /// limito i nodi interni totali, in modo da evitare alberi troppo grandi

    printf("inserisco %d\n", n_nodes);

    if (n_nodes++ >= max_nodes) /// limito il numero di nodi
        return;
    printf("inserisco %d\n", n_nodes);

    float probabilita = 0.8; /// tra 0 e 1

    if (rand() % 100 < probabilita * 100) { // se numero random e' minore della probabilita' -> aggiungo nodo R con valore a caso
        tree_insert_child_R(n, rand() % 100);
    }
    if (rand() % 100 < probabilita * 100) { // se numero random e' minore della probabilita' -> aggiungo nodo L con valore a caso
        tree_insert_child_L(n, rand() % 100);
    }
    if (n->L != NULL)
        insert_random_rec(n->L);
    if (n->R != NULL)
        insert_random_rec(n->R);
}

int ct_visit = 0;

void inOrder(node_t *n) {

    if (n->L != NULL) {
        if (graph) {
            // scrivo arco con numero operazione di visita
            print_node_code(n);
            output_graph << ":id:w -> ";
            print_node_code(n->L);
            output_graph << ":id:w [ color=blue, fontcolor=blue, penwidth=3, label = \"" << ct_visit++ << "\"]\n";
        }

        inOrder(n->L);

        if (graph) {
            // scrivo arco con numero operazione di visita
            print_node_code(n->L);
            output_graph << ":id:e -> ";
            print_node_code(n);
            output_graph << ":val:s [ color=blue, fontcolor=blue, penwidth=3, label = \"" << ct_visit++ << "\"]\n";
        }
    }

    if (details)
        printf("%d ", n->val);

    if (graph) {
        node_print_graph(n);
    }

    if (n->R != NULL) {
        if (graph) {
            // scrivo arco con numero operazione di visita
            print_node_code(n);
            output_graph << ":val:s -> ";
            print_node_code(n->R);
            output_graph << ":id:w [ color=blue, fontcolor=blue, penwidth=3, label = \"" << ct_visit++ << "\"]\n";
        }
        inOrder(n->R);
        if (graph) {
            // scrivo arco con numero operazione di visita
            print_node_code(n->R);
            output_graph << ":id:e -> ";
            print_node_code(n);
            output_graph << ":id:e [ color=blue, fontcolor=blue, penwidth=3, label = \"" << ct_visit++ << "\"]\n";
        }
    }
}

void preOrder(node_t *n) {

    if (details)
        printf("%d ", n->val);

    if (graph) {
        node_print_graph(n);
    }

    if (n->L != NULL) {
        if (graph) {
            // scrivo arco con numero operazione di visita
            print_node_code(n);
            output_graph << ":id:w -> ";
            print_node_code(n->L);
            output_graph << ":id:w [ color=blue, fontcolor=blue, penwidth=3, label = \"" << ct_visit++ << "\"]\n";
        }

        preOrder(n->L);

        if (graph) {
            // scrivo arco con numero operazione di visita
            print_node_code(n->L);
            output_graph << ":id:e -> ";
            print_node_code(n);
            output_graph << ":val:s [ color=blue, fontcolor=blue, penwidth=3, label = \"" << ct_visit++ << "\"]\n";
        }
    }

    if (n->R != NULL) {
        if (graph) {
            // scrivo arco con numero operazione di visita
            print_node_code(n);
            output_graph << ":val:s -> ";
            print_node_code(n->R);
            output_graph << ":id:w [ color=blue, fontcolor=blue, penwidth=3, label = \"" << ct_visit++ << "\"]\n";
        }
        preOrder(n->R);
        if (graph) {
            // scrivo arco con numero operazione di visita
            print_node_code(n->R);
            output_graph << ":id:e -> ";
            print_node_code(n);
            output_graph << ":id:e [ color=blue, fontcolor=blue, penwidth=3, label = \"" << ct_visit++ << "\"]\n";
        }
    }
}

void postOrder(node_t *n) {

    if (n->L != NULL) {
        if (graph) {
            // scrivo arco con numero operazione di visita
            print_node_code(n);
            output_graph << ":id:w -> ";
            print_node_code(n->L);
            output_graph << ":id:w [ color=blue, fontcolor=blue, penwidth=3, label = \"" << ct_visit++ << "\"]\n";
        }

        postOrder(n->L);

        if (graph) {
            // scrivo arco con numero operazione di visita
            print_node_code(n->L);
            output_graph << ":id:e -> ";
            print_node_code(n);
            output_graph << ":val:s [ color=blue, fontcolor=blue, penwidth=3, label = \"" << ct_visit++ << "\"]\n";
        }
    }

    if (n->R != NULL) {
        if (graph) {
            // scrivo arco con numero operazione di visita
            print_node_code(n);
            output_graph << ":val:s -> ";
            print_node_code(n->R);
            output_graph << ":id:w [ color=blue, fontcolor=blue, penwidth=3, label = \"" << ct_visit++ << "\"]\n";
        }
        postOrder(n->R);
        if (graph) {
            // scrivo arco con numero operazione di visita
            print_node_code(n->R);
            output_graph << ":id:e -> ";
            print_node_code(n);
            output_graph << ":id:e [ color=blue, fontcolor=blue, penwidth=3, label = \"" << ct_visit++ << "\"]\n";
        }
    }

    if (details)
        printf("%d ", n->val);

    if (graph) {
        node_print_graph(n);
    }
}

void EulerOrder(node_t *n) {

    if (details)
        printf("%d ", n->val);

    output_visit << n->val << "\n";

    if (graph) {
        node_print_graph(n);
    }

    if (n->L != NULL) {
        if (graph) {
            // scrivo arco con numero operazione di visita
            print_node_code(n);
            output_graph << ":id:w -> ";
            print_node_code(n->L);
            output_graph << ":id:w [ color=blue, fontcolor=blue, penwidth=3, label = \"" << ct_visit++ << "\"]\n";
        }

        EulerOrder(n->L);

        if (graph) {
            // scrivo arco con numero operazione di visita
            print_node_code(n->L);
            output_graph << ":id:e -> ";
            print_node_code(n);
            output_graph << ":val:s [ color=blue, fontcolor=blue, penwidth=3, label = \"" << ct_visit++ << "\"]\n";
        }
    }

    if (details)
        printf("%d ", n->val);
    output_visit << n->val << "\n";

    if (n->R != NULL) {
        if (graph) {
            // scrivo arco con numero operazione di visita
            print_node_code(n);
            output_graph << ":val:s -> ";
            print_node_code(n->R);
            output_graph << ":id:w [ color=blue, fontcolor=blue, penwidth=3, label = \"" << ct_visit++ << "\"]\n";
        }
        EulerOrder(n->R);
        if (graph) {
            // scrivo arco con numero operazione di visita
            print_node_code(n->R);
            output_graph << ":id:e -> ";
            print_node_code(n);
            output_graph << ":id:e [ color=blue, fontcolor=blue, penwidth=3, label = \"" << ct_visit++ << "\"]\n";
        }
    }

    if (details)
        printf("%d ", n->val);
    output_visit << n->val << "\n";
}

/// crea una copia dell'albero in input, scambiando i sottoalberi L e R
node_t *flip(node_t *n) {
    node_t *n1 = node_new(n->val);

    /// chiamate ricorsive

    return n1;
}

// costruzione albero in base ai valori stampati dalla visita di eulero
// la funzione restituisce il puntatore alla radice dell'albero
// Es. input: 32 74 74 64 76 76 44 44 44 76 64 64 74 32 85 85 2 36 36 36 2 85 85 85 2 85 32
node_t *build_euler() {

    return NULL;
}

int parse_cmd(int argc, char **argv) {
    /// controllo argomenti
    int ok_parse = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][1] == 'v') {
            details = 1;
            ok_parse = 1;
        }
        if (argv[i][1] == 'g') {
            graph = 1;
            ok_parse = 1;
        }
    }

    if (argc > 1 && !ok_parse) {
        printf("Usage: %s [Options]\n", argv[0]);
        printf("Options:\n");
        printf("  -verbose: Abilita stampe durante l'esecuzione dell'algoritmo\n");
        printf("  -graph: creazione file di dot con il grafo dell'esecuzione (forza d=1 t=1)\n");
        return 1;
    }

    return 0;
}

//1- Richiesta da consegna : Verificare se l' albero è bilanciato

int max(int a, int b) {     //funzione che restituisce il massimo tra due numeri (propedeutica per height)
    return (a > b) ? a : b;
}

int height(node_t *n) {        //calcola l'altezza dell'albero  (propedeutica per isBalanced)
    if (n == NULL)
        return 0;
    return 1 + max(height(n->L), height(n->R));
}

bool isBalanced(node_t *root) {      //controlla se l'albero è bilanciato
    if (root == NULL)            //caso base  
        return true;

    int lh = height(root->L);
    int rh = height(root->R);
    
    if (abs(lh - rh) <= 1 && isBalanced(root->L) && isBalanced(root->R))
        return true;
    
    return false;
}

bool is_node_equal(node_t* node1, node_t* node2) {
    if (node1 == NULL && node2 == NULL)
        return true;

    if (node1 == NULL || node2 == NULL)
        return false;

    std::cout << "Confronto " << node1->val << " con " << node2->val << endl;
    return (node1->val == node2->val) &&
           is_node_equal(node1->L, node2->L) &&
           is_node_equal(node1->R, node2->R);
}

bool is_tree_inside(node_t *node1, node_t *node2) {
    if (node1 == NULL)
        return true;

    if (node2 != NULL) {
        if (is_node_equal(node1, node2))
            return true;

        return is_tree_inside(node1, node2->L) || is_tree_inside(node1, node2->R);
    }

    return false;
}

int main(int argc, char **argv) {
    int i, test;

    if (parse_cmd(argc, argv))
        return 1;

    // init random
    srand((unsigned)time(NULL));

    if (graph) {
        output_graph.open("graph.dot");
        /// preparo header
        output_graph << "digraph g" << endl;
        output_graph << "{ " << endl;
        output_graph << "node [shape=none]" << endl;
        output_graph << "rankdir=\"TB\"" << endl;
        ;
        output_graph << "edge[tailclip=false,arrowtail=dot];" << endl;
    }

    // tree_print_graph(root);
    // tree_insert_child_L(root, 2);
    // tree_print_graph(root);
    // tree_insert_child_L(root->L, 3);
    // tree_print_graph(root);
    // tree_insert_child_L(root->L->L, 4);
    // tree_print_graph(root);
    // tree_insert_child_L(root->L->L->L, 5);
    // tree_print_graph(root);

    // creo albero random
    // insert_random_rec(root);

    // CONSEGNA 2.1 (bilanciato?)
    node_t *root = node_new(1);
    global_ptr_ref = root;

    tree_insert_child_L(root, 2);
    tree_insert_child_R(root, 3);
    tree_insert_child_L(root->L, 4);
    tree_insert_child_R(root->L, 7);
    tree_insert_child_L(root->L->R, 8);
    tree_insert_child_R(root->R, 5);
    printTreeStructure(root);   // stampo l'albero per avere una visione chiara

    // Controllo bilanciamento
    if (isBalanced(root))
        std::cout << "L'albero e' bilanciato." << endl;
    else
        std::cout << "L'albero e' bilanciato!" << endl;


    // CONSEGNA 2.2 (albero 1 <= albero 2 ?)

    // albero 1
    std::cout << "--- Albero 1 ---" << endl;
    node_t *root1 = node_new(1);

    tree_insert_child_R(root1, 3);
    tree_insert_child_R(root1->R, 5);
    printTreeStructure(root1);   // stampo l'albero per avere una visione chiara

    // albero 2
    std::cout << "--- Albero 2 ---" << endl;
    
    node_t *root2 = node_new(1);
    tree_insert_child_L(root2, 9);
    tree_insert_child_R(root2, 1);
    tree_insert_child_L(root2->L, 6);
    tree_insert_child_R(root2->L, 7);
    tree_insert_child_R(root2->R, 3);
    tree_insert_child_R(root2->R->R, 5);
    printTreeStructure(root2);   // stampo l'albero per avere una visione chiara

    if (is_tree_inside(root1, root2))
        std::cout << "L'albero 1 e' contenuto nel 2! =)" << endl;
    else
        std::cout << "L'albero 1 NON e' contenuto nel 2! =(" << endl;

    return 0;
}

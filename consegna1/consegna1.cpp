// Fatto da Daniele Tallo e Francesco Alexander Salcuni

#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

// compilazione: g++ consegna1-loader.c
//

// Il programma carica il file data.txt contenente 100 righe con dati da ordinare in modo crescente
// ./a.out
// In output viene mostrato il numero di accessi in read alla memoria per eseguire il sorting di ciascuna riga

// Obiettivo:
// Creare un algoritmo di sorting che minimizzi la somma del numero di accessi per ogni sorting di ciascuna riga del file

int ct_swap = 0;
int ct_cmp = 0;
int ct_read = 0;

int max_dim = 0;
int ntests = 100;
int ndiv = 1;
int details = 0;
int graph = 0;

int n = 0; /// dimensione dell'array

void print_array(int *A, int dim) {
    for (int j = 0; j < dim; j++) {
        printf("%d ", A[j]);
    }
    printf("\n");
}

void swap(int &a, int &b) {
    int tmp = a;
    a = b;
    b = tmp;
    /// aggiorno contatore globale di swap
    ct_swap++;
}

int partition(int *A, int p, int r) {
    /// copia valori delle due meta p..q e q+1..r
    ct_read++;
    int x = A[r];
    int i = p - 1;

    for (int j = p; j < r; j++) {
        ct_cmp++;
        ct_read++;
        if (A[j] <= x) {
            i++;
            ct_read++;
            ct_read++;
            swap(A[i], A[j]);
        }
    }
    ct_read++;
    ct_read++;
    swap(A[i + 1], A[r]);

    return i + 1;
}

void quick_sort(int *A, int p, int r) {
    /// gli array L e R sono utilizzati come appoggio per copiare i valori: evita le allocazioni nella fase di merge
    if (p < r) {
        int q = partition(A, p, r);
        quick_sort(A, p, q - 1);
        quick_sort(A, q + 1, r);
    }
}

void insertion_sort(int *A, int p, int r) {
    for (int i = p + 1; i <= r; ++i) {
        int key = A[i];
        ct_read++;
        int j = i - 1;

        while (j >= p && A[j] > key) {
            ct_read++;
            ct_cmp++;
            A[j + 1] = A[j];
            ct_read+=2;
            j = j - 1;
        }
        A[j + 1] = key;
        ct_read++;
    }
}

void shell_sort(int *A, int p, int r) {
    int n = r - p + 1;
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = p + gap; i <= r; ++i) {
            int temp = A[i];
            ct_read++;
            int j;
            for (j = i; j >= p + gap && A[j - gap] > temp; j -= gap) {
                A[j] = A[j - gap];
                ct_read += 2;
                ct_cmp++;
            }
            A[j] = temp;
        }
    }
}

void gnome_sort(int *A, int left, int right) {
    int old_left = left;

    while (left <= right) {
        if (left == old_left)
            left++;
        if (A[left] >= A[left - 1]) {
            ct_read += 2;
            left++;
        } else {
            swap(A[left], A[left - 1]);
            ct_read += 2;
            left--;
        }
    }
}

void merge(int arr[], int left, int middle1, int right) {
    // Allocazione dinamica di array temporanei per contenere le due parti della sequenza
    int n1 = middle1 - left + 1;
    int n2 = right - middle1;

    int* L = new int[n1];
    int* M = new int[n2];

    for (int i = 0; i < n1; i++) {
        L[i] = arr[left + i];
        ct_read++;
    }
        
    for (int j = 0; j < n2; j++) {
        M[j] = arr[middle1 + 1 + j];
        ct_read++;
    }

    int i = 0, j = 0;
    int idx = left;

    while (i < n1 && j < n2) {
        if (L[i] <= M[j]) {
            arr[idx] = L[i];
            ct_read++;
            i++;
        } else {
            arr[idx] = M[j];
            ct_read++;
            j++;
        }
        idx++;
    }

    while (i < n1) {
        arr[idx] = L[i];
        ct_read++;
        i++;
        idx++;
    }

    while (j < n2) {
        arr[idx] = M[j];
        ct_read++;
        j++;
        idx++;
    }
    
    delete[] L;
    delete[] M;
}

void compare_array(int *A_new, int *A_old) {
    for (int i = 0; i < 1000; ++i) {
        if (A_new[i] != A_old[i]) {
            std::cout << "1 ";
        }
        else {
            std::cout << "0 ";
        }
    }
}

int parse_cmd(int argc, char **argv) {

    /// parsing argomento
    max_dim = 1000;

    for (int i = 1; i < argc; i++) {
        if (argv[i][1] == 'd')
            ndiv = atoi(argv[i] + 3);
        if (argv[i][1] == 't')
            ntests = atoi(argv[i] + 3);
        if (argv[i][1] == 'v')
            details = 1;
        if (argv[i][1] == 'g') {
            graph = 1;
            ndiv = 1;
            ntests = 1;
        }
    }

    return 0;
}

int main(int argc, char **argv) {
    int i, test;
    int *A;
    int *B; /// buffer per visualizzazione algoritmo

    if (parse_cmd(argc, argv))
        return 1;

    /// allocazione array
    A = new int[max_dim];

    n = max_dim;

    // srand((unsigned)time(NULL));
    // //creazione file input: NON USARE PIU' --> il file data.txt ufficiale Ã¨ stato allegato, per permettere confronti equivalenti
    //  FILE *f = fopen("data.txt", "w+");
    //  int size=100;
    //  for (int j = 0; j < size; j++) {
    //      for (int i = 0; i < n; i++) {
    //          int v = 0;             
    //          v=(int)(100000*exp(-(0.0+i-n/2)*(0.0+i-n/2)/n/n*64));
    //          v+=rand()%(i+1);
    //          fprintf(f, "%d,", v);
    //      }
    //      fprintf(f, "\n");
    //  }
    //  fclose(f);

    ifstream input_data;
    input_data.open("data.txt");

    int read_min = -1;
    int read_max = -1;
    long read_avg = 0;

    //// lancio ntests volte per coprire diversi casi di input random
    for (test = 0; test < ntests; test++) {

        /// inizializzazione array: numeri random con range dimensione array
        for (i = 0; i < n; i++) {
            char comma;
            input_data >> A[i];
            input_data >> comma;
        }

        if (details) {
            printf("caricato array di dimensione %d\n", n);
            print_array(A, n);
        }

        ct_swap = 0;
        ct_cmp = 0;
        ct_read = 0;

        // algoritmi di sorting
        shell_sort(A, 0, 190);                        // 0   -> 190
        gnome_sort(A, 191, 500);                      // 191 -> 500
        shell_sort(A, 501, 999);                      // 501 -> 999

        int start = 0, end = 999;
        int middle1 = 190;
        int middle2 = 500;

        merge(A, start, middle1, middle2);          // 1° e 2°
        merge(A, start, middle2, end);              // unisco il precedente col 3°

        if (details) {
            printf("Output:\n");
            print_array(A, n);
        }

        /// statistiche
        read_avg += ct_read;
        if (read_min < 0 || read_min > ct_read)
            read_min = ct_read;
        if (read_max < 0 || read_max < ct_read)
            read_max = ct_read;
        printf("Test %d %d\n", test, ct_read);
    }

    printf("N test: %d, Min: %d, Med: %.1f, Max: %d\n",
           ntests,
           read_min, (0.0 + read_avg) / ntests, read_max);

    delete[] A;

    return 0;
}
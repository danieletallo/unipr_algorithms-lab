// Fatto da Francesco Alexander Salcuni, Daniele Tallo, Tommaso Chiti

#include <cmath>
#include <cstdio>
#include <string>
#include <optional>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;
using char_container = std::vector<char>;

int is_match(char temp1, char temp2) {
    if (temp1 == temp2 || (std::isalpha(temp1) && std::isalpha(temp2))) 
        return 1;
    else 
        return 0;
}

int score(char temp1, char temp2) {
    if(temp1==temp2) return 10;
    if(std::isalpha(temp1) && std::isalpha(temp2)) return 8;
    if(std::isdigit(temp1) && std::isdigit(temp2)) return 8;
    if(std::isalnum(temp1) && std::isalnum(temp2)) return 4;
    if(std::isalnum(temp1) && (temp2 == '{'
        || temp2 == '}' 
        || temp2 == '(' 
        || temp2 == ')' 
        || temp2 == '[' 
        || temp2 == ']' 
        || temp2 == '<' 
        || temp2 == '>'
        || temp2 == ';')
        ) return -12;
    if(std::isalnum(temp2) && (temp1 == '{'
        || temp1 == '}' 
        || temp1 == '(' 
        || temp1 == ')' 
        || temp1 == '[' 
        || temp1 == ']' 
        || temp1 == '<' 
        || temp1 == '>'
        || temp1 == ';')
        ) return -12;
    
    return 0;
}

void print_result(int score_aggr, int size) {
    if (score_aggr == size * 10)
        std::cout << "I file sono identici" << std::endl;
    else if(score_aggr == size * -12)
        std::cout << "I file sono completamente diversi" << std::endl;
    else if(score_aggr < 0 && score_aggr >= -30)
        std::cout << "I file sono mediamente diversi" << std::endl;
    else if(score_aggr < -30)
        std::cout << "I file sono molto diversi" << std::endl;
    else if(score_aggr >= 0 && score_aggr < 30)
        std::cout << "I file sono mediamente uguali" << std::endl;
    else if(score_aggr >= 30)
        std::cout << "I file sono molto simili" << std::endl;
    else
        std::cout << "I file sono uguali" << std::endl;
}

void mismatch(const char_container& cc1, const char_container& cc2) {
    std::string buffer1{};
    std::string buffer2{};
    int score_aggr{};

    auto it1 = cc1.begin();
    auto it2 = cc2.begin();

    // finchè non finisco uno dei files
    while (it1 != cc1.end() && it2 != cc2.end()) {
        std::string row1{}, row2{};
        std::string diff1{}, diff2{};
        int row_score_aggr{};

        // costruisco la riga del file1 (non considerando il newline)
        while (it1 != cc1.end() && *it1 != '\n') {
            row1 += *it1++;
        }
        if (it1 != cc1.end() && *it1 == '\n') it1++; // skippa il newline

        // costruisco la riga del file2 (non considerando il newline)
        while (it2 != cc2.end() && *it2 != '\n') {
            row2 += *it2++;
        }
        if (it2 != cc2.end() && *it2 == '\n') it2++; // skippa il newline

        // confronto le 2 righe carattere per carattere
        // se una delle due righe è già finita, uso uno spazio
        for (int i = 0; i < max(row1.size(), row2.size()); ++i) {
            char c1 = (i < row1.size()) ? row1[i] : ' ';
            char c2 = (i < row2.size()) ? row2[i] : ' ';

            // se i caratteri sono diversi, li aggiungo alle stringhe diff
            // se, però, non si tratta di uno spazio aggiunto "artificialmente"
            if (c1 != c2) {
                if (i < row1.size()) {
                    diff1 += c1;
                }
                if (i < row2.size()) {
                    diff2 += c2;
                }
            }
            // altrimenti aggiungo uno spazio per mostrare in che posizione effettivamente le righe siano diverse
            else {
                diff1 += ' ';
                diff2 += ' ';
            }

            // calcolo score e lo aggiungo al punteggio della riga
            int current_score = score(c1, c2);
            row_score_aggr += current_score;
            // std::cout << c1 << " " << c2 << " " << current_score << std::endl;
        }

        // dato che non confronto i newline, li aggiungo manualmente
        buffer1 += diff1 + "\n";
        buffer2 += diff2 + "\n";
        score_aggr += row_score_aggr;
    }

    // aggiungo caratteri rimanenti dalle righe rimanenti del file più lungo
    buffer1.append(it1, cc1.end());
    buffer2.append(it2, cc2.end());

    std::cout <<"DIFFERENZE\n" << "File1:\n" << buffer1 << "\nFile2:\n" << buffer2 << std::endl;
    
    // stampo il risultato di "similitudine" tra i due files sulla base del file più corto
    // in modo da considerare la similitudine soltanto tra lo stesso numero di righe
    print_result(score_aggr, min(cc1.size(), cc2.size()));
}

void allinea(int** M, const char_container& cc1, const char_container& cc2) {
    int max_length = 0;
    int end_pos = 0;

    for (int i = 0; i < cc1.size() + 1; i++)
        M[i] = new int[cc2.size() + 1];

    for (int i = 0; i < cc1.size() + 1; i++)
        M[i][0] = 0;

    for (int j = 0; j < cc2.size() + 1; j++)
        M[0][j] = 0;

    for (int i = 1; i < cc1.size() + 1; i++) {
        for (int j = 1; j < cc2.size() + 1; j++) {
            int match = is_match(cc1[i - 1], cc2[j - 1]);

            if (match)
                M[i][j] = 1 + M[i - 1][j - 1];
            else
                M[i][j] = 0;

            // tengo traccia della lunghezza max
            if (M[i][j] > max_length) {
                max_length = M[i][j];
                end_pos = i;
            }
        }
    }

    // stampo la matrice
    // for (int i = 0; i < cc1.size() + 1; i++) {
    //     printf("%2d %c: ", i, i > 0 ? cc1[i - 1] : ' ');

    //     for (int j = 0; j < cc2.size() + 1; j++) {
    //         printf("%2d ", M[i][j]);
    //     }
    //     printf("\n");
    // }

    // stampo la longest common substring
    if (max_length > 0) {
        printf("Longest Common Substring: \n");
        for (int i = end_pos - max_length; i < end_pos; i++) {
            printf("%c", cc1[i]);
        }
        printf("\n");
    } else {
        printf("No Common Substring Found.\n");
    }
}

char_container load_file(const std::string& path) {
    std::string line;
    char_container cc;
    std::fstream source;

    source.open(path);

    if (!source.is_open()) {
        std::cerr << "Something went wrong!";
        return {};
    }

    while(std::getline(source, line)) {
        for (const char c : line) {
            cc.push_back(c);
        }
        cc.push_back('\n');
    }

    return cc;
}

void print(const char_container& cc) {
    for (const char c : cc) {
        std::cout << c;
    }
}

int main() {
   
    std::string path1 = R"(C:\Users\39338\Desktop\Universita\2 ANNO\2anno_2semestre\Lab. Algoritmi\consegne\consegna6\source1.cpp)";
    std::string path2 = R"(C:\Users\39338\Desktop\Universita\2 ANNO\2anno_2semestre\Lab. Algoritmi\consegne\consegna6\source2.cpp)";

    auto cc1 = load_file(path1);
    auto cc2 = load_file(path2);
    
    int** M;
    if( cc1.size() > cc2.size() ) {
        M = new int *[cc1.size() + 1];    
    } else {
        M = new int *[cc2.size() + 1];
    }

    // 1 - i pezzi di codice più inalterato (ricerca longest common substring), con eventuale tolleranza per variazioni del nome delle variabili
    allinea(M, cc1, cc2);

    // 2 - le parti di codice cambiato, dando priorità alla struttura mantenuta (quindi parentesi e punto e virgola)
    mismatch(cc1, cc2);

    delete [] M;

    return 0;
}

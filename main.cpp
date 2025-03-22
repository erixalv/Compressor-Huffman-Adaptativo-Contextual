#include <iostream>
#include "text_processing.h"
#include "No.h"
#include "LinkedList.h"
#include <fstream>
#include <unordered_set>
#include <string>


using namespace std;


int main() {

    TextProcessing::preprocessText();
    int numChar = TextProcessing::countDistinctCharacters();

    string fileName = "./input.txt";

    ifstream file(fileName);
    if (!file) {
        cerr << "Erro ao abrir o arquivo: " << fileName << endl;
        return 1;
    }

    unordered_set<char> caracteresUnicos;
    char caractere;

    // Lendo o arquivo caractere por caractere
    while (file.get(caractere)) {
        caracteresUnicos.insert(caractere);
    }

    file.close();

    string caracteresArmazenados;
    for (char c : caracteresUnicos) {
        caracteresArmazenados += c;
    }

    LinkedList *eqv = new LinkedList();

    for (int i = 0; i < numChar; i++) {
        eqv->insereOrdenado(caracteresArmazenados[i], 1);
    }

    eqv->recalculaCodificacoes();
    eqv->printList();

}
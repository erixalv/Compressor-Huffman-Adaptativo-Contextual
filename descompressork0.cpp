#include <iostream>
#include <fstream>
#include <bitset>
#include <unordered_set>
#include "ArvHuffman.h"
#include "LinkedList.h"
#include "text_processing.h"

using namespace std;

int main() {
    int numChar = TextProcessing::countDistinctCharacters();

    string fileAlf = "input.txt";
    ifstream fileAlfabeto(fileAlf);

    unordered_set<char> caracteresUnicos;
    char caractere;

    // Lendo o arquivo caractere por caractere
    while (fileAlfabeto.get(caractere)) {
        caracteresUnicos.insert(caractere);
    }

    string caracteresArmazenados;
    for (char c : caracteresUnicos) {
        caracteresArmazenados += c;
    }

    //Criação da Lista K = -1
    LinkedList *eqv = new LinkedList();

    for (int i = 0; i < numChar; i++) {
        eqv->insereOrdenado(caracteresArmazenados[i], 1);
    }

    eqv->recalculaCodificacoes();



    string inputFile = "codificadok0.bin";
    string outputFile = "descomprimidok0.txt";
    
    ifstream fileIn(inputFile, ios::binary);
    ofstream fileOut(outputFile);
    
    if (!fileIn) {
        cerr << "Erro ao abrir o arquivo: " << inputFile << endl;
        return 1;
    }
    
    // Lê o arquivo binário e converte para uma string de bits
    string bitString = "";
    char byte;
    while (fileIn.read(&byte, sizeof(char))) {
        bitString += bitset<8>(byte).to_string();
    }

    string cod;
    string decod;

    ArvHuffman* k0;
    int countOp = 0;
    
    for(int i = 0; i < bitString.size(); i++) {
        cod += bitString[i];

        if(countOp == 1){
            if(k0->containsCode(cod)) {
                decod += k0->getSymbolByCode(cod);
                cout << decod;
                k0->update(k0->getSymbolByCode(cod));
                cod.clear();
            }
            else {
                if(eqv->buscaLetra(cod) != '\0') {
                    decod += eqv->buscaLetra(cod);
                    cout << decod;
                    k0->update(eqv->buscaLetra(cod));
                    eqv->remove(eqv->buscaLetra(cod));
                    cod.clear();
                }
            }
        }

        if(eqv->buscaLetra(cod) != '\0' && countOp == 0) {
            decod += eqv->buscaLetra(cod);
            cout << decod;
            k0->update(eqv->buscaLetra(cod));
            eqv->remove(eqv->buscaLetra(cod));
            cod.clear();

            countOp = 1;
        }
    }

    cout << "DEPOIS DO FOR" << endl;

    fileOut << decod;

    fileAlfabeto.close();
    fileIn.close();
    fileOut.close();
}

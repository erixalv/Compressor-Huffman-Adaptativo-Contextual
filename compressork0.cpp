#include <iostream>
#include "text_processing.h"
#include "No.h"
#include "LinkedList.h"
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <string>
#include "ArvHuffman.h"
#include <vector>
#include <memory>
#include <bitset>
#include <chrono>
#include <cmath>



using namespace std;




int main() {

    //Processamento do texto e retorno do número de caracteres para a criação da lista de K = -1
    TextProcessing::preprocessText();
    int numChar = TextProcessing::countDistinctCharacters();

    string fileName = "input.txt";

    ifstream file(fileName);
    ofstream fileOut("codificadok0.bin", ios::binary);

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

    //Retorno da leitura para o começo do arquivo
    file.clear();
    file.seekg(0, ios::beg);


    char c;
    int countOp = 0;
    ArvHuffman* k0 = new ArvHuffman("");
    string textoCod;

    string charPrev;

    auto inicio = chrono::high_resolution_clock::now();

    while (file.get(c)) {


        //Contexto K = -1:
        if(countOp == 0) {
            textoCod += eqv->buscaCod(c); //codifica de acordo com a lista equivalente

            eqv->remove(c);               //remove da lista após a codificação
            eqv->recalculaCodificacoes(); //lista atualiza codificações de acordo com o tam atual   
            k0->update(c);                //Contexto K = 0 atualizado com novo caractere

            
            charPrev = c;                 //Armazenamento do caractere atual para a prox iteração
        }
        
        //Contexto K = 0:
        if(countOp >= 1) {
            //Contexto K = 0
            if(k0->contains(c)) {             //Se K = 0 tem o símbolo, codifica e atualiza
                textoCod += k0->getCode(c); 

                k0->update(c);
            }
            else {                            //Se não, pega do K = -1, e atualiza os dois
                textoCod+= k0->getNYTCode();

                textoCod += eqv->buscaCod(c);

                eqv->remove(c);
                eqv->recalculaCodificacoes();
                k0->update(c);
            }                       
            charPrev = c;
        }

        countOp++;
    }
    file.close();

    auto fim = chrono::high_resolution_clock::now();
    chrono::duration<double> duracao = fim - inicio;
    
    cout << "Tempo de execução para K = 0: " << duracao.count() << " segundos" << endl;

    float nSimb = TextProcessing::countTotalChar();
    float bitspSimb = textoCod.size()/nSimb;

    cout << "Comprimido em K = 0 com " << bitspSimb << " bits/símbolo." << endl;


    for(int i =0; i < textoCod.size(); i += 8) {
        string byteStr = textoCod.substr(i, 8);

        while(byteStr.size()<8) {
            byteStr += '0';
        }

        char byte = static_cast<char>(bitset<8>(byteStr).to_ulong());
        fileOut.write(&byte, sizeof(char));
    }

    fileOut.close();

    delete eqv;
    delete k0;

}
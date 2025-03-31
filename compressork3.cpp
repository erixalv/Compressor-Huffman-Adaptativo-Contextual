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


using namespace std;


int main() {

    //Processamento do texto e retorno do número de caracteres para a criação da lista de K = -1
    TextProcessing::preprocessText();
    int numChar = TextProcessing::countDistinctCharacters();

    string fileName = "input.txt";

    ifstream file(fileName);
    ofstream fileOut("codificadok3.bin", ios::binary);

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
    vector<unique_ptr<ArvHuffman>> k1, k2, k3, k4, k5;
    string textoCod;

    string charPrev, charPrev2, charPrev3, charPrev4, charPrev5;

    auto inicio = chrono::high_resolution_clock::now();

    while (file.get(c)) {

        //Contexto K = -1
        if(countOp == 0) {
            textoCod += eqv->buscaCod(c); //codifica de acordo com a lista equivalente
            eqv->remove(c);               //remove da lista após a codificação
            eqv->recalculaCodificacoes(); //lista atualiza codificações de acordo com o tam atual   
            k0->update(c);                //Contexto K = 0 atualizado com novo caractere

            
            charPrev = c;                 //Armazenamento do caractere atual para a prox iteração
            charPrev2 = c;
            charPrev3 = c;
            charPrev4 = c;
            charPrev5 = c;
        }
        if(countOp == 1) {
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
            k1.push_back(make_unique<ArvHuffman>(charPrev)); // K = 1 é uma lista de árvores, uma para cada
            //letra de contexto anterior;
            //mySymbol é a letra de cada uma
            k1[0]->update(c);                        //atualiza o K = 1 com a letra anterior e a atual como nó
            charPrev = c;                            //atualiza a letra anterior
            charPrev2 += c;                          //atualiza as 2 letras anteriores
            charPrev3 += c;
            charPrev4 += c;
            charPrev5 += c;
        }

        //Contexto K = 1 
        if(countOp == 2) {

            bool found = 0;

            for (int i = 0; i < k1.size(); i++) { //Procura se existe no K = 1 a letra anterior

                if (k1[i]->getMySymbol() == charPrev) { //Se sim:
                    found = 1;

                    if(k1[i]->contains(c)) {            //Codifica e atualiza a árvore
                        textoCod += k1[i]->getCode(c);
                        k1[i]->update(c);
                    }
                    else {                              //Se não:
                        textoCod += k1[i]->getNYTCode();//Codifica Rou
                        if(k0->contains(c)) {           //Busca no K = 0 e se conter, codifica
                            textoCod += k0->getCode(c);
                            k0->update(c);
                        }
                        else {                          //Se não tem, codifica no K = -1
                            textoCod += eqv->buscaCod(c);
                            eqv->remove(c);
                            eqv->recalculaCodificacoes();
                            k0->update(c);
                        }
                        k1[i]->update(c);                //Atualiza a árvore K = 1[i] com o símbolo atual
                    }
                    break;
                }   
            }

            if(found == 0) {                         //Se K = 1 não tem a letra anterior como contexto:
                if(k0->contains(c)) {           //Busca no K = 0 e codifica
                    textoCod += k0->getCode(c);
                    k0->update(c);
                }
                else {                          //Se não tem no K = 0, codifica como K = -1
                    textoCod += eqv->buscaCod(c);
                    eqv->remove(c);
                    eqv->recalculaCodificacoes();
                    k0->update(c);
                }

                k1.push_back(make_unique<ArvHuffman>(charPrev)); //Atualiza a lista de árvore de K = 1
                k1.back()->update(c);                            //com a nova árvore para o novo contexto
            }

            k2.push_back(make_unique<ArvHuffman>(charPrev2));
            k2[0]->update(c);

            charPrev2 = charPrev + c;
            charPrev = c;
            charPrev3 += c;
            charPrev4 += c;
            charPrev5 += c;
        }

        //Contexto K = 2:
        if(countOp == 3) {
            bool found2 = 0;

            for (int i = 0; i < k2.size(); i++) { //Procura se existe no K = 2 as 2 letrasa anteriores
                if(k2[i]->getMySymbol() == charPrev2) {
                    found2 = 1;

                    if(k2[i]->contains(c)) {      //Se a árvore de contexto 2 contém a letra atual codifica:
                        textoCod += k2[i]->getCode(c);
                        k2[i]->update(c);
                    }
                    else {                        //Se não, codifica NYT e executa a busca anterior de K = 1
                        textoCod += k2[i]->getNYTCode();

                        bool found = 0;
                        for (int j = 0; j < k1.size(); j++) { //Procura se existe no K = 1 a letra anterior

                            if (k1[j]->getMySymbol() == charPrev) { //Se sim:
                                found = 1;
            
                                if(k1[j]->contains(c)) {            //Codifica e atualiza a árvore
                                    textoCod += k1[i]->getCode(c);
                                    k1[j]->update(c);
                                }
                                else {                              //Se não:
                                    textoCod += k1[j]->getNYTCode();//Codifica Rou
                                    if(k0->contains(c)) {           //Busca no K = 0 e se conter, codifica
                                        textoCod += k0->getCode(c);
                                        k0->update(c);
                                    }
                                    else {                          //Se não tem, codifica no K = -1
                                        textoCod += eqv->buscaCod(c);
                                        eqv->remove(c);
                                        eqv->recalculaCodificacoes();
                                        k0->update(c);
                                    }
                                    k1[j]->update(c);                //Atualiza a árvore K = 1[i] com o símbolo atual
                                }
                                break;
                            }   
                        }
            
                        if(found == 0) {                         //Se K = 1 não tem a letra anterior como contexto:
                            if(k0->contains(c)) {           //Busca no K = 0 e codifica
                                textoCod += k0->getCode(c);
                                k0->update(c);
                            }
                            else {                          //Se não tem no K = 0, codifica como K = -1
                                textoCod += eqv->buscaCod(c);
                                eqv->remove(c);
                                eqv->recalculaCodificacoes();
                                k0->update(c);
                            }
            
                            k1.push_back(make_unique<ArvHuffman>(charPrev)); //Atualiza a lista de árvore de K = 1
                            k1.back()->update(c);                            //com a nova árvore para o novo contexto
                        }
                        k2[i]->update(c);
                    }
                }
            }

            if(found2 == 0) {                             //Se K = 2 nao tem as 2 letras anteriores, executa
                                                          //a busca de K = 1
                bool found = 0;
                    for (int i = 0; i < k1.size(); i++) { //Procura se existe no K = 1 a letra anterior

                        if (k1[i]->getMySymbol() == charPrev) { //Se sim:
                            found = 1;
        
                            if(k1[i]->contains(c)) {            //Codifica e atualiza a árvore
                                textoCod += k1[i]->getCode(c);
                                k1[i]->update(c);
                            }
                            else {                              //Se não:
                                textoCod += k1[i]->getNYTCode();//Codifica Rou
                                if(k0->contains(c)) {           //Busca no K = 0 e se conter, codifica
                                    textoCod += k0->getCode(c);
                                    k0->update(c);
                                }
                                else {                          //Se não tem, codifica no K = -1
                                    textoCod += eqv->buscaCod(c);
                                    eqv->remove(c);
                                    eqv->recalculaCodificacoes();
                                    k0->update(c);
                                }
                                k1[i]->update(c);                //Atualiza a árvore K = 1[i] com o símbolo atual
                            }
                            break;
                        }   
                    }
        
                    if(found == 0) {                         //Se K = 1 não tem a letra anterior como contexto:
                        if(k0->contains(c)) {           //Busca no K = 0 e codifica
                            textoCod += k0->getCode(c);
                            k0->update(c);
                        }
                        else {                          //Se não tem no K = 0, codifica como K = -1
                            textoCod += eqv->buscaCod(c);
                            eqv->remove(c);
                            eqv->recalculaCodificacoes();
                            k0->update(c);
                        }
        
                        k1.push_back(make_unique<ArvHuffman>(charPrev)); //Atualiza a lista de árvore de K = 1
                        k1.back()->update(c);                            //com a nova árvore para o novo     
                                                                         //contexto
                    }
                k2.push_back(make_unique<ArvHuffman>(charPrev2)); //Atualiza a árvore de K = 2 com 
                k2.back()->update(c);                             //as 2 letras anteriores de contexto
            }

            k3.push_back(make_unique<ArvHuffman>(charPrev3));
            k3.back()->update(c);

            charPrev3 = charPrev2 + c;
            charPrev2 = charPrev + c;
            charPrev = c; 
            charPrev4 += c;
            charPrev5 += c;
        }

        //Contexto K = 3:
        if (countOp >= 4) {
            bool found3 = 0;

            for (int i = 0; i < k3.size(); i++) { //Procura em K=3 se as 3 letras anteriores existem
                if(k3[i]->getMySymbol() == charPrev3) { //Se existe: busca se a letra atual existe como nó
                    found3 = 1;

                    if(k3[i]->contains(c)) {            //Se sim, codifica
                        textoCod += k3[i]->getCode(c);
                        k3[i]->update(c);
                    }
                    else {                              //Se não, busca em contextos anteriores até achar
                        textoCod += k3[i]->getNYTCode();

                        bool found2 = 0;

                        for (int j = 0; j < k2.size(); j++) { //Procura se existe no K = 2 as 2 letrasa anteriores
                            if(k2[j]->getMySymbol() == charPrev2) {
                                found2 = 1;

                                if(k2[j]->contains(c)) {      //Se a árvore de contexto 2 contém a letra atual codifica:
                                    textoCod += k2[i]->getCode(c);
                                    k2[j]->update(c);
                                }
                                else {                        //Se não, codifica NYT e executa a busca anterior de K = 1
                                    textoCod += k2[j]->getNYTCode();

                                    bool found = 0;
                                    for (int k = 0; k < k1.size(); k++) { //Procura se existe no K = 1 a letra anterior

                                        if (k1[k]->getMySymbol() == charPrev) { //Se sim:
                                            found = 1;
                        
                                            if(k1[k]->contains(c)) {            //Codifica e atualiza a árvore
                                                textoCod += k1[k]->getCode(c);
                                                k1[k]->update(c);
                                            }
                                            else {                              //Se não:
                                                textoCod += k1[k]->getNYTCode();//Codifica Rou
                                                if(k0->contains(c)) {           //Busca no K = 0 e se conter, codifica
                                                    textoCod += k0->getCode(c);
                                                    k0->update(c);
                                                }
                                                else {                          //Se não tem, codifica no K = -1
                                                    textoCod += eqv->buscaCod(c);
                                                    eqv->remove(c);
                                                    eqv->recalculaCodificacoes();
                                                    k0->update(c);
                                                }
                                                k1[k]->update(c);                //Atualiza a árvore K = 1[i] com o símbolo atual
                                            }
                                            break;
                                        }   
                                    }
                        
                                    if(found == 0) {                         //Se K = 1 não tem a letra anterior como contexto:
                                        if(k0->contains(c)) {           //Busca no K = 0 e codifica
                                            textoCod += k0->getCode(c);
                                            k0->update(c);
                                        }
                                        else {                          //Se não tem no K = 0, codifica como K = -1
                                            textoCod += eqv->buscaCod(c);
                                            eqv->remove(c);
                                            eqv->recalculaCodificacoes();
                                            k0->update(c);
                                        }
                        
                                        k1.push_back(make_unique<ArvHuffman>(charPrev)); //Atualiza a lista de árvore de K = 1
                                        k1.back()->update(c);                            //com a nova árvore para o novo contexto
                                    }
                                    k2[j]->update(c);
                                }
                            }
                        }

                        if(found2 == 0) {                             //Se K = 2 nao tem as 2 letras anteriores, executa
                                                                    //a busca de K = 1
                            bool found = 0;
                                for (int i = 0; i < k1.size(); i++) { //Procura se existe no K = 1 a letra anterior

                                    if (k1[i]->getMySymbol() == charPrev) { //Se sim:
                                        found = 1;
                    
                                        if(k1[i]->contains(c)) {            //Codifica e atualiza a árvore
                                            textoCod += k1[i]->getCode(c);
                                            k1[i]->update(c);
                                        }
                                        else {                              //Se não:
                                            textoCod += k1[i]->getNYTCode();//Codifica Rou
                                            if(k0->contains(c)) {           //Busca no K = 0 e se conter, codifica
                                                textoCod += k0->getCode(c);
                                                k0->update(c);
                                            }
                                            else {                          //Se não tem, codifica no K = -1
                                                textoCod += eqv->buscaCod(c);
                                                eqv->remove(c);
                                                eqv->recalculaCodificacoes();
                                                k0->update(c);
                                            }
                                            k1[i]->update(c);                //Atualiza a árvore K = 1[i] com o símbolo atual
                                        }
                                        break;
                                    }   
                                }
                    
                                if(found == 0) {                         //Se K = 1 não tem a letra anterior como contexto:
                                    if(k0->contains(c)) {           //Busca no K = 0 e codifica
                                        textoCod += k0->getCode(c);
                                        k0->update(c);
                                    }
                                    else {                          //Se não tem no K = 0, codifica como K = -1
                                        textoCod += eqv->buscaCod(c);
                                        eqv->remove(c);
                                        eqv->recalculaCodificacoes();
                                        k0->update(c);
                                    }
                    
                                    k1.push_back(make_unique<ArvHuffman>(charPrev)); //Atualiza a lista de árvore de K = 1
                                    k1.back()->update(c);      //com a nova árvore para o novo contexto     
                                }
                            k2.push_back(make_unique<ArvHuffman>(charPrev2)); //Atualiza a árvore de K = 2 com 
                            k2.back()->update(c);                             //as 2 letras anteriores de ctx
                        }
                    }
                    k3[i]->update(c);
                }
            }
            
            if(found3 == 0) {
                bool found2 = 0;

                for (int i = 0; i < k2.size(); i++) { //Procura se existe no K = 2 as 2 letras anteriores
                    if(k2[i]->getMySymbol() == charPrev2) {
                        found2 = 1;

                        if(k2[i]->contains(c)) {  //Se a árvore de contexto 2 contém a letra atual codifica:
                            textoCod += k2[i]->getCode(c);
                            k2[i]->update(c);
                        }
                        else {                      //Se não, codifica NYT e executa a busca anterior de K = 1
                            textoCod += k2[i]->getNYTCode();

                            bool found = 0;
                            for (int j = 0; j < k1.size(); j++) { //Procura se existe no K = 1 a letra anterior

                                if (k1[i]->getMySymbol() == charPrev) { //Se sim:
                                    found = 1;
                
                                    if(k1[j]->contains(c)) {            //Codifica e atualiza a árvore
                                        textoCod += k1[j]->getCode(c);
                                        k1[j]->update(c);
                                    }
                                    else {                              //Se não:
                                        textoCod += k1[j]->getNYTCode();//Codifica Rou
                                        if(k0->contains(c)) {           //Busca no K = 0 e se conter, codifica
                                            textoCod += k0->getCode(c);
                                            k0->update(c);
                                        }
                                        else {                          //Se não tem, codifica no K = -1
                                            textoCod += eqv->buscaCod(c);
                                            eqv->remove(c);
                                            eqv->recalculaCodificacoes();
                                            k0->update(c);
                                        }
                                        k1[j]->update(c);                //Atualiza a árvore K = 1[i] com o símbolo atual
                                    }
                                    break;
                                }   
                            }
                
                            if(found == 0) {                         //Se K = 1 não tem a letra anterior como contexto:
                                if(k0->contains(c)) {           //Busca no K = 0 e codifica
                                    textoCod += k0->getCode(c);
                                    k0->update(c);
                                }
                                else {                          //Se não tem no K = 0, codifica como K = -1
                                    textoCod += eqv->buscaCod(c);
                                    eqv->remove(c);
                                    eqv->recalculaCodificacoes();
                                    k0->update(c);
                                }
                
                                k1.push_back(make_unique<ArvHuffman>(charPrev)); //Atualiza a lista de árvore de K = 1
                                k1.back()->update(c);                            //com a nova árvore para o novo contexto
                            }
                            k2[i]->update(c);
                        }
                    }
                }

                if(found2 == 0) {                             //Se K = 2 nao tem as 2 letras anteriores, executa
                                                            //a busca de K = 1
                    bool found = 0;
                        for (int i = 0; i < k1.size(); i++) { //Procura se existe no K = 1 a letra anterior

                            if (k1[i]->getMySymbol() == charPrev) { //Se sim:
                                found = 1;
            
                                if(k1[i]->contains(c)) {            //Codifica e atualiza a árvore
                                    textoCod += k1[i]->getCode(c);
                                    k1[i]->update(c);
                                }
                                else {                              //Se não:
                                    textoCod += k1[i]->getNYTCode();//Codifica Rou
                                    if(k0->contains(c)) {           //Busca no K = 0 e se conter, codifica
                                        textoCod += k0->getCode(c);
                                        k0->update(c);
                                    }
                                    else {                          //Se não tem, codifica no K = -1
                                        textoCod += eqv->buscaCod(c);
                                        eqv->remove(c);
                                        eqv->recalculaCodificacoes();
                                        k0->update(c);
                                    }
                                    k1[i]->update(c);                //Atualiza a árvore K = 1[i] com o símbolo atual
                                }
                                break;
                            }   
                        }
            
                        if(found == 0) {                         //Se K = 1 não tem a letra anterior como contexto:
                            if(k0->contains(c)) {           //Busca no K = 0 e codifica
                                textoCod += k0->getCode(c);
                                k0->update(c);
                            }
                            else {                          //Se não tem no K = 0, codifica como K = -1
                                textoCod += eqv->buscaCod(c);
                                eqv->remove(c);
                                eqv->recalculaCodificacoes();
                                k0->update(c);
                            }
            
                            k1.push_back(make_unique<ArvHuffman>(charPrev)); //Atualiza a lista de árvore de K = 1
                            k1.back()->update(c);      //com a nova árvore para o novo contexto     
                        }
                    k2.push_back(make_unique<ArvHuffman>(charPrev2)); //Atualiza a árvore de K = 2 com 
                    k2.back()->update(c);                             //as 2 letras anteriores de ctx
                }

                k3.push_back(make_unique<ArvHuffman>(charPrev3));
                k3.back()->update(c);
            }

            k4.push_back(make_unique<ArvHuffman>(charPrev4));
            k4.back()->update(c);
            
            charPrev4 = charPrev3 + c;
            charPrev3 = charPrev2 + c;
            charPrev2 = charPrev + c;
            charPrev = c;
            charPrev5 += c;
        }
        countOp++;
    }    
    auto fim = chrono::high_resolution_clock::now();
    chrono::duration<double> duracao = fim - inicio;
    
    cout << "Tempo de execução para K = 3: " << duracao.count() << " segundos" << endl;

    float nSimb = TextProcessing::countTotalChar();
    float bitspSimb = textoCod.size()/nSimb;

    cout << "Comprimido em K = 3 com " << bitspSimb << " bits/símbolo." << endl;


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

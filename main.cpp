#include <iostream>
#include <fstream>
#include <unordered_set>
#include <string>
#include "LinkedList.h"
#include "ArvHuffman.h"
#include "ContextModel.h"
#include "text_processing.h"
using namespace std;

int main() {
    // Pré-processamento do texto
    TextProcessing::preprocessText();
    int numChar = TextProcessing::countDistinctCharacters();

    // Arquivos de entrada e saída
    string fileName = "input.txt";
    string fileOutName = "codificado.bin";

    ifstream file(fileName);
    ofstream fileOut(fileOutName);
    
    if (!file) {
        cerr << "Erro ao abrir o arquivo: " << fileName << endl;
        return 1;
    }

    if (!fileOut) {
        cerr << "Erro ao abrir o arquivo: " << fileOutName << endl;
        return 1;
    }

    // Leitura dos caracteres únicos
    unordered_set<char> caracteresUnicos;
    char caractere;
    while (file.get(caractere)) {
        caracteresUnicos.insert(caractere);
    }
    file.clear();
    file.seekg(0); // Volta ao início do arquivo para nova leitura

    // Criação da lista de equivalência
    // Criação da lista de equivalência
    string caracteresArmazenados;
    for (char c : caracteresUnicos) {
        caracteresArmazenados += c;
    }

    LinkedList *eqv = new LinkedList();
    for (int i = 0; i < numChar; i++) {
        eqv->insereOrdenado(caracteresArmazenados[i], 1);
    }
    eqv->recalculaCodificacoes(); // Garante que as codificações estejam atualizadas
    
    // Inicialização do modelo de contexto
    ContextModel *context = new ContextModel(5, shared_ptr<LinkedList>(eqv));
    
    string textoCod;
    string contexto; // Mantém os últimos caracteres (até 5)
    
    // Processamento do arquivo
    char c;
    while (file.get(c)) {
        // Pega os últimos 5 caracteres (ou menos, se não houver)
        string ctxAtual = contexto.length() >= 5 ? contexto.substr(contexto.length() - 5) : contexto;
        
        cout << "--------------------------------------------------" << endl;
        cout << "Símbolo atual: '" << c << "'" << endl;
        cout << "Contexto atual: \"" << ctxAtual << "\"" << endl;
        
        auto resultado = context->processarSimbolo(ctxAtual, c);
        textoCod += resultado.second;
        
        cout << "Codificação: " << resultado.second << endl;
        cout << "Fallback usado? " << (resultado.first ? "Sim (k = -1)" : "Não (contexto encontrado)") << endl;
        
        // Atualiza o contexto (adiciona o novo caractere e mantém no máximo 5)
        contexto += c;
        if (contexto.length() > 5) {
            contexto = contexto.substr(contexto.length() - 5);
        }
        
        cout << "Contexto atualizado: \"" << contexto << "\"" << endl;
    }

    cout << "Feito";

    // Escreve o resultado codificado
    fileOut << textoCod;

    // Limpeza
    file.close();
    fileOut.close();
    delete eqv;
    delete context;

    return 0;
}
#ifndef CONTEXTMODEL_H
#define CONTEXTMODEL_H


#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <bitset>
#include "ArvHuffman.h"
#include "LinkedList.h"

using namespace std;


#pragma once

class ContextModel {
    private:
        vector<unordered_map<string, shared_ptr<ArvHuffman>>> contextos; // Para ordens 0-5
        shared_ptr<LinkedList> listaEquivalencia; // Lista de fallback
        int ordemMaxima;
        
    public:
        ContextModel(int ordem, shared_ptr<LinkedList> eqvList) : 
            ordemMaxima(ordem), listaEquivalencia(eqvList) {
            contextos.resize(ordem + 1); // Cria estruturas para ordens 0 até ordemMaxima
            // Inicializa o contexto de ordem 0
            contextos[0][""] = make_shared<ArvHuffman>();
        }
        
        pair<bool, string> processarSimbolo(const string& contextoAnterior, char simboloAtual) {
            // Tenta encontrar o símbolo nos contextos de ordem 5 até 0
            for (int ordem = min((int)contextoAnterior.length(), ordemMaxima); ordem >= 0; ordem--) {
                string ctx = contextoAnterior.substr(contextoAnterior.length() - ordem);
                
                if (!contextos[ordem][ctx]) {
                    contextos[ordem][ctx] = make_shared<ArvHuffman>();
                }
        
                auto& arvore = contextos[ordem][ctx];
                
                // Verifica se o símbolo existe ANTES de atualizar
                if (arvore->simboloExiste(simboloAtual)) {
                    string codigo = arvore->obterCodigo(simboloAtual);
                    arvore->atualizarArvore(simboloAtual); // Atualiza apenas se o símbolo existir
                    return make_pair(false, codigo); // Codificação contextual
                }
            }
            
            // Fallback para k = -1 (lista de equivalentes)
            if (listaEquivalencia) {
                string codigo = listaEquivalencia->buscaCodificacaoPorSimbolo(simboloAtual);
                if (!codigo.empty()) {
                    listaEquivalencia->remove(simboloAtual);
                    return make_pair(true, codigo); // Codificação por lista de equivalentes
                }
            }
            
            // Fallback final: codificação bruta (8 bits ASCII)
            return make_pair(true, bitset<8>(simboloAtual).to_string());
        }
};


#endif
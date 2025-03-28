#ifndef ARVHUFFMAN
#define ARVHUFFMAN

#pragma once

#include "NoHuffman.h"
#include <bitset>

class ArvHuffman {
    private:
        shared_ptr<NoHuffman> raiz;
        unordered_map<char, shared_ptr<NoHuffman>> nos_folha;
        vector<shared_ptr<NoHuffman>> nos_internos;
        int contador_ordem;
        const char SIMBOLO_EOF = '\0';
        
        shared_ptr<NoHuffman> encontrarNYT() {
            for (auto& par : nos_folha) {
                if (par.second->peso == 0) {
                    return par.second;
                }
            }
            return nullptr;
        }
        
        void incrementarPesos(shared_ptr<NoHuffman> no) {
            while (no != nullptr) {
                no->peso++;
                no = no->pai;
            }
        }
        
        void rebalancear(shared_ptr<NoHuffman> no) {
            while (no != nullptr) {
                auto atual = no;
                
                // Encontra o nó com maior peso na mesma camada
                for (auto& interno : nos_internos) {
                    if (interno->peso > atual->peso && interno->ordem > atual->ordem) {
                        swap(atual, interno);
                    }
                }
                
                // Troca os nós se necessário
                if (atual != no) {
                    swap(no->ordem, atual->ordem);
                    swap(no->peso, atual->peso);
                    
                    if (no->pai->esquerda == no) {
                        no->pai->esquerda = atual;
                    } else {
                        no->pai->direita = atual;
                    }
                    
                    if (atual->pai->esquerda == atual) {
                        atual->pai->esquerda = no;
                    } else {
                        atual->pai->direita = no;
                    }
                    
                    swap(no->pai, atual->pai);
                }
                
                no = no->pai;
            }
        }
        
    public:
        ArvHuffman() : contador_ordem(0) {
            inicializar();
        }
        
        void inicializar() {
            auto noEOF = make_shared<NoHuffman>(SIMBOLO_EOF, 0);
            raiz = noEOF;
            nos_folha[SIMBOLO_EOF] = noEOF;
            noEOF->ordem = contador_ordem++;
        }
        
        void atualizarArvore(char simbolo) {
            auto no = nos_folha[simbolo];
            if (!no) {
                // Cria novo nó para símbolo não existente
                auto novoNo = make_shared<NoHuffman>(simbolo, 1);
                auto nyt = encontrarNYT();
                
                // Cria novo nó interno
                auto noInterno = make_shared<NoHuffman>('\0', 0);
                noInterno->esquerda = nyt;
                noInterno->direita = novoNo;
                
                // Atualiza pais
                nyt->pai = noInterno;
                novoNo->pai = noInterno;
                
                // Atualiza estrutura
                if (nyt == raiz) {
                    raiz = noInterno;
                } else {
                    if (nyt->pai->esquerda == nyt) {
                        nyt->pai->esquerda = noInterno;
                    } else {
                        nyt->pai->direita = noInterno;
                    }
                }
                
                nos_folha[simbolo] = novoNo;
                nos_internos.push_back(noInterno);
                no = noInterno;
            }
            
            // Atualiza pesos até a raiz
            while (no != nullptr) {
                no->peso++;
                no = no->pai;
            }
            
            // Rebalanceia a árvore
            rebalancear(nos_folha[simbolo]);
        }
        
        string obterCodigo(char simbolo) {
            string codigo;
            shared_ptr<NoHuffman> no;
            
            if (simbolo == SIMBOLO_EOF) {
                no = nos_folha[SIMBOLO_EOF];
            } else {
                if (nos_folha.find(simbolo) == nos_folha.end()) {
                    return "";
                }
                no = nos_folha[simbolo];
            }
            
            while (no->pai != nullptr) {
                if (no->pai->esquerda == no) {
                    codigo = "0" + codigo;
                } else {
                    codigo = "1" + codigo;
                }
                no = no->pai;
            }
            
            return codigo;
        }
        
        string codificar(char simbolo) {
            // Primeiro verifica se o símbolo existe na árvore
            if (simboloExiste(simbolo)) {
                return obterCodigo(simbolo);  // Retorna código Huffman se existir
            } else {
                string codigoNYT = obterCodigo('\0');
                string codigoSimbolo = bitset<8>(simbolo).to_string();
                return codigoNYT + codigoSimbolo;  // NYT + símbolo bruto
            }
        }
        
        char decodificar(const string& bits, size_t& pos) {
            shared_ptr<NoHuffman> atual = raiz;
            
            while (atual->esquerda != nullptr || atual->direita != nullptr) {
                if (pos >= bits.size()) {
                    return '\0'; // Erro ou fim inesperado
                }
                
                if (bits[pos++] == '0') {
                    atual = atual->esquerda;
                } else {
                    atual = atual->direita;
                }
            }
            
            if (atual->simbolo == '\0') {
                // NYT - ler próximo byte
                char simbolo = 0;
                for (int i = 0; i < 8; i++) {
                    if (pos >= bits.size()) {
                        return '\0'; // Erro
                    }
                    simbolo = (simbolo << 1) | (bits[pos++] == '1' ? 1 : 0);
                }
                atualizarArvore(simbolo);
                return simbolo;
            } else {
                atualizarArvore(atual->simbolo);
                return atual->simbolo;
            }
        }
        
        void imprimirArvore(shared_ptr<NoHuffman> no, int nivel = 0) {
            if (no == nullptr) return;
            
            imprimirArvore(no->direita, nivel + 1);
            
            for (int i = 0; i < nivel; i++) {
                cout << "    ";
            }
            
            if (no->simbolo == '\0') {
                cout << "[" << no->peso << "]" << endl;
            } else {
                cout << no->simbolo << " (" << no->peso << ")" << endl;
            }
            
            imprimirArvore(no->esquerda, nivel + 1);
        }
        
        void imprimir() {
            cout << "Árvore de Huffman:" << endl;
            imprimirArvore(raiz);
        }
        bool simboloExiste(char simbolo) const {
            return nos_folha.find(simbolo) != nos_folha.end() && nos_folha.at(simbolo)->peso > 0;
        }
    };

#endif
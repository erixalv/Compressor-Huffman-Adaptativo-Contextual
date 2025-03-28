#ifndef NOHUFFMAN
#define NOHUFFMAN

#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <queue>
#include <memory>

using namespace std;

// Estrutura do nó da árvore
class NoHuffman {
public:
    char simbolo;
    int peso;
    int ordem;
    shared_ptr<NoHuffman> esquerda;
    shared_ptr<NoHuffman> direita;
    shared_ptr<NoHuffman> pai;
    
    NoHuffman(char s = '\0', int p = 0) : simbolo(s), peso(p), ordem(0), esquerda(nullptr), direita(nullptr), pai (nullptr) {}
};

#endif
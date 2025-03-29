#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#pragma once

#include "No.h"

class LinkedList
{
public:
    LinkedList();
    bool isEmpty();
    int getSize();
    No* getHead();
    void insereOrdenado(char l, int f);
    void remove(char l);
    void recalculaCodificacoes();
    void printList();
    string buscaCodificacaoPorSimbolo(char l);
    char buscaSimboloPorCodificacao(const std::string& cod);

private:
    No* head;
    int size;
};

#endif
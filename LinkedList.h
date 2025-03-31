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
    void insereOrdenado(char l, int f);
    void remove(char l);
    void recalculaCodificacoes();
    void printList();
    string buscaCod(char c);

private:
    No* head;
    int size;
};

#endif
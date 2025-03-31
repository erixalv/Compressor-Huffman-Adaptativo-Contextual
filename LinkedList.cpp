#include "LinkedList.h"
#include <cmath>
#include <iostream>

using namespace std;

LinkedList::LinkedList()
{
    head = nullptr;
    size = 0;
}

bool LinkedList::isEmpty() {
    return (size == 0);
}

int LinkedList::getSize() {
    return size;
}

void LinkedList::insereOrdenado(char l, int f) {
    // Aloca o novo nó
    No* novoNo = new No();
    novoNo->setLetra(l);
    novoNo->setFreq(f);
    novoNo->setNext(nullptr); // Importante para evitar lixo de memória

    // Se a lista estiver vazia, o novo nó torna-se a cabeça
    if(getSize() == 0) {
        head = novoNo;
        size++;
        return;
    }

    // Caso o novo nó deva ser inserido no início.
    // Critério: letra menor que a do head ou mesma letra e frequência menor.
    if(l < head->getLetra() || (l == head->getLetra() && f < head->getFreq())) {
        novoNo->setNext(head);
        head = novoNo;
        size++;
        return;
    }

    // Percorre a lista para encontrar o local de inserção adequado
    No* atual = head;
    while(atual->getNext() != nullptr) {
        // Verifica se devemos inserir antes do próximo nó
        // Critério: se a letra do próximo nó for maior que 'l'
        // ou se a letra for a mesma mas a frequência do próximo nó for maior que 'f'
        if(atual->getNext()->getLetra() > l ||
           (atual->getNext()->getLetra() == l && atual->getNext()->getFreq() > f)) {
            break;
        }
        atual = atual->getNext();
    }

    // Insere o novo nó entre 'atual' e 'atual->getNext()'
    novoNo->setNext(atual->getNext());
    atual->setNext(novoNo);
    size++;
}

void LinkedList::remove(char l) {
    // Se a lista estiver vazia, nada a remover.
    if (head == nullptr)
        return;

    // Se o nó a ser removido é o primeiro (head)
    if (head->getLetra() == l) {
        No* temp = head;
        head = head->getNext();
        delete temp;  // Libera a memória alocada para o nó removido
        size--;
        return;
    }

    // Procura o nó que precede o nó a ser removido
    No* atual = head;
    while (atual->getNext() != nullptr) {
        if (atual->getNext()->getLetra() == l) {
            No* temp = atual->getNext();
            // Atualiza o ponteiro para "pular" o nó removido
            atual->setNext(temp->getNext());
            delete temp;
            size--;
            return;
        }
        atual = atual->getNext();
    }
}

void LinkedList::recalculaCodificacoes() {
    
    // 'size' representa o número atual de nós na lista
    int x = size;
    
    // Calcula o número mínimo de bits (n) para representar um número menor que x
    // O loop aumenta n enquanto 2^n for menor ou igual a x.
    int n = 0;
    while ((1 << n) < x) { // (1 << n) é equivalente a 2^n
        n++;
    }
    
    // Percorre a lista atribuindo a codificação de cada nó de acordo com sua posição.
    No* atual = head;
    int pos = 0;
    while (atual != nullptr) {
        // Converte a posição para uma string binária com n dígitos (com zeros à esquerda)
        std::string codificacao = "";
        int num = pos;
        // Gera a codificação de trás para frente
        for (int i = 0; i < n; i++) {
            codificacao = ((num & 1) ? "1" : "0") + codificacao;
            num >>= 1;
        }
        atual->setCod(codificacao);
        pos++;
        atual = atual->getNext();
    }
}

void LinkedList::printList() {
    No* atual = head;
    while (atual != nullptr) {
        cout << "Caractere: " << atual->getLetra()
                  << ", Frequência: " << atual->getFreq();
        
        // Se o nó possuir uma codificação definida, exibe também.
        std::string cod = atual->getCod();
        if (!cod.empty()) {
            cout << ", Codificação: " << cod;
        }
        cout << endl;
        atual = atual->getNext();
    }
}

string LinkedList::buscaCod(char c) {
    No* aux = head;
    while(aux->getLetra() != c) {
        aux = aux->getNext();
    }
    return aux->getCod();
}
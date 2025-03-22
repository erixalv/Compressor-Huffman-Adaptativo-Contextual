#include "ArvHuffman.h"

ArvHuffman::ArvHuffman()
{
    root = nullptr;
}

NoArv* ArvHuffman::buscaLetraOrigin(NoArv* no, char c) {
    if (no == nullptr) {
        return nullptr;
    }
    if(no->getL() == c) {
        return no;
    }

    NoArv* aux;
    aux = no;

    while (aux->getL() != c) {
        if (aux->getL() > c) {
            ArvHuffman::buscaLetraOrigin(no->getLeft(), c);
        }
        if (aux->getL() < c) {
            ArvHuffman::buscaLetraOrigin(no->getRight(), c);
        }
    }
}

NoArv* ArvHuffman::buscaLetra(char c) {
    return ArvHuffman::buscaLetraOrigin(root, c);
}

void ArvHuffman::insereLetra(char c) {
    NoArv* aux;
    aux->setL(c);

    if(root == nullptr) {
        NoArv* rou;
        rou->setFreq(1);
        root->setL('R');
        aux->setFreq(1);
        root->setFreq(2);
        root->setLeft(rou);
        root->setRight(aux);
        return;
    }
    if(buscaLetra(c) != nullptr) {
        NoArv* aux2;
        aux2 = buscaLetra(c);
        aux2->setFreq(1);
        return;
    }
    if(buscaLetra(c) == nullptr) {
                
    }
}
#include "No.h"

No::No()
{
    letra = '\0';
    freq = 0;
    next = nullptr;
}

int No::getFreq() {
    return freq;
}

void No::setFreq(int f) {
    freq = f;
}

char No::getLetra() {
    return letra;
}

void No::setLetra(char l) {
    letra = l;
}

No* No::getNext() {
    return next;
}

void No::setNext(No* n) {
    next = n;
}

string No::getCod() {
    return cod;
}

void No::setCod(string c) {
    cod = c;
}
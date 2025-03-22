#include "NoArv.h"

NoArv::NoArv()
{
    l = '\0';
    freq = 0;
    cod = "";
    right = nullptr;
    left = nullptr;
}

char NoArv::getL() {
    return l;
}

void NoArv::setL(char c) {
    l = c;
}

int NoArv::getFreq() {
    return freq;
}

void NoArv::setFreq(int f) {
    freq += f;
}

string NoArv::getCod() {
    return cod;
}

void NoArv::setCod(string s) {
    cod += s;
}

NoArv* NoArv::getLeft() {
    return left;
}

void NoArv::setLeft(NoArv* l) {
    left = l;
}

NoArv* NoArv::getRight() {
    return right;
}

void NoArv::setRight(NoArv* r) {
    right = r;
}
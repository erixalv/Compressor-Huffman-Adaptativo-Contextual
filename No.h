#ifndef NO_H
#define NO_H

#pragma once

#include <string>

using namespace std;

class No
{
public:
    No();
    int getFreq();
    void setFreq(int f);
    char getLetra();
    void setLetra(char l);
    No* getNext();
    void setNext(No* n);
    string getCod();
    void setCod(string c);

private:
    int freq;
    char letra;
    string cod;
    No* next;
};

#endif
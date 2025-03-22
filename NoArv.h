#ifndef NOARV_H
#define NOARV_H

#pragma once

#include <iostream>
#include <string>

using namespace std;

class NoArv
{
public:
    NoArv();
    char getL();
    void setL(char c);
    int getFreq();
    void setFreq(int f);
    string getCod();
    void setCod(string s);
    NoArv* getLeft();
    void setLeft(NoArv* l);
    NoArv* getRight();
    void setRight(NoArv* r);

private:
    char l;
    int freq;
    string cod;
    NoArv* right;
    NoArv* left;
};

#endif
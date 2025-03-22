#ifndef ARVHUFFMAN_H
#define ARVHUFFMAN_H

#pragma once

#include <iostream>
#include <string>
#include "NoArv.h"

class ArvHuffman
{
public:
    ArvHuffman();
    void insereLetra(char c);
    NoArv* buscaLetra(char c);

private:
    NoArv* root;
    NoArv* buscaLetraOrigin(NoArv* no, char c);

};

#endif
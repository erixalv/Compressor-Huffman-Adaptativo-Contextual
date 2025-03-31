#ifndef ARVHUFFMAN_H
#define ARVHUFFMAN_H

#include <unordered_map>
#include <string>

class ArvHuffman {
public:
    ArvHuffman(std::string s);
    ~ArvHuffman();

    // Atualiza a árvore com o símbolo recebido
    void update(char symbol);

    // Retorna o código binário do símbolo (ou o código do nó NYT se o símbolo ainda não foi inserido)
    std::string getCode(char symbol);

    // Retorna o código binário do nó NYT (Not Yet Transmitted)
    std::string getNYTCode();

    bool contains(char symbol);

    std::string getMySymbol();
    void setMySymbol(std::string s);

private:
    struct Node {
        char symbol;    // Símbolo armazenado (se folha); para nós internos pode ser irrelevante
        int weight;     // Peso do nó
        int order;      // Número de ordem usado pelo algoritmo FGK
        Node *parent;
        Node *left;
        Node *right;
        bool isNYT;     // Verdadeiro se o nó for o nó NYT ("Not Yet Transmitted")

        Node(char sym, int ord, bool nyt);
    };

    Node* root;
    Node* NYT; // Ponteiro para o nó NYT atual
    int maxOrder; // Contador para atribuição de ordem (iniciado com um valor alto)
    std::string mySymbol;
    std::unordered_map<char, Node*> symbolNodes; // Mapeia cada símbolo para seu nó folha

    // Libera memória da árvore recursivamente
    void deleteTree(Node* node);

    // Atualiza a árvore a partir do nó fornecido (algoritmo FGK)
    void updateTree(Node* node);

    // Troca dois nós na árvore (ajustando ponteiros de pai e atualizando a ordem)
    void swapNodes(Node* node1, Node* node2);

    // Encontra um candidato para troca (nó com mesmo peso, ordem maior e que não seja ancestral do nó)
    Node* findSwapCandidate(Node* node);

    // Gera o código binário (caminho da raiz até o nó); '0' para ramo esquerdo e '1' para direito
    std::string generateCode(Node* node);

    // Verifica se 'ancestor' é ancestral de 'node'
    bool isAncestor(Node* ancestor, Node* node);

};

#endif // ADAPTIVE_HUFFMAN_H

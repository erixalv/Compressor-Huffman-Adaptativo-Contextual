#include "ArvHuffman.h"
#include <iostream>
#include <functional>

// Construtor do nó
ArvHuffman::Node::Node(char sym, int ord, bool nyt)
    : symbol(sym), weight(0), order(ord), parent(nullptr), left(nullptr), right(nullptr), isNYT(nyt) {}

// Construtor da árvore
ArvHuffman::ArvHuffman(std::string s) {
    maxOrder = 512; // Valor inicial para atribuição de ordem (pode ser ajustado conforme necessário)
    root = new Node(0, maxOrder, true); // Inicialmente, a árvore contém apenas o nó NYT
    NYT = root;
    mySymbol = s;
}

// Destrutor: libera a memória alocada para a árvore
ArvHuffman::~ArvHuffman() {
    deleteTree(root);
}

// Função recursiva para liberar a árvore
void ArvHuffman::deleteTree(Node* node) {
    if (node) {
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
}

// Verifica se 'ancestor' é ancestral de 'node'
bool ArvHuffman::isAncestor(Node* ancestor, Node* node) {
    Node* current = node->parent;
    while (current) {
        if (current == ancestor)
            return true;
        current = current->parent;
    }
    return false;
}

// Gera o código binário para um nó, percorrendo do nó até a raiz
std::string ArvHuffman::generateCode(Node* node) {
    std::string code;
    Node* current = node;
    while (current != root) {
        if (current->parent->left == current)
            code.insert(code.begin(), '0');
        else
            code.insert(code.begin(), '1');
        current = current->parent;
    }
    return code;
}

// Retorna o código para um símbolo; se o símbolo não estiver na árvore, retorna o código do nó NYT
std::string ArvHuffman::getCode(char symbol) {
    if (symbolNodes.find(symbol) != symbolNodes.end()) {
        return generateCode(symbolNodes[symbol]);
    } else {
        return getNYTCode();
    }
}

// Retorna o código do nó NYT
std::string ArvHuffman::getNYTCode() {
    return generateCode(NYT);
}

// Troca dois nós na árvore, ajustando seus ponteiros de pai e seus números de ordem
void ArvHuffman::swapNodes(Node* node1, Node* node2) {
    if (!node1 || !node2 || node1 == node2 || node1->parent == nullptr || node2->parent == nullptr)
        return;

    Node* parent1 = node1->parent;
    Node* parent2 = node2->parent;

    // Atualiza o filho do pai para apontar para o nó trocado
    if (parent1->left == node1)
        parent1->left = node2;
    else
        parent1->right = node2;

    if (parent2->left == node2)
        parent2->left = node1;
    else
        parent2->right = node1;

    std::swap(node1->parent, node2->parent);
    std::swap(node1->order, node2->order);
}

// Percorre a árvore para encontrar o candidato para troca, conforme o algoritmo FGK
ArvHuffman::Node* ArvHuffman::findSwapCandidate(Node* node) {
    Node* candidate = nullptr;
    // Função lambda para percorrer a árvore
    std::function<void(Node*)> traverse = [&](Node* current) {
        if (!current)
            return;
        if (current->weight == node->weight && current != node && current->order > node->order && !isAncestor(current, node)) {
            if (!candidate || current->order > candidate->order)
                candidate = current;
        }
        traverse(current->left);
        traverse(current->right);
    };
    traverse(root);
    return candidate;
}

// Atualiza a árvore a partir do nó fornecido, seguindo o procedimento do algoritmo FGK
void ArvHuffman::updateTree(Node* node) {
    while (node) {
        Node* candidate = findSwapCandidate(node);
        if (candidate) {
            swapNodes(node, candidate);
        }
        node->weight++;
        node = node->parent;
    }
}

// Atualiza a árvore com o símbolo recebido
void ArvHuffman::update(char symbol) {
    // Se o símbolo ainda não existe na árvore, é necessário expandir o nó NYT
    if (symbolNodes.find(symbol) == symbolNodes.end()) {
        Node* oldNYT = NYT;
        oldNYT->isNYT = false; // Este nó deixa de ser NYT e se torna um nó interno

        // Cria dois novos nós: um nó NYT (à esquerda) e um nó folha para o novo símbolo (à direita)
        int newOrderRight = oldNYT->order - 1;
        int newOrderLeft = oldNYT->order - 2;
        oldNYT->left = new Node(0, newOrderLeft, true);
        oldNYT->right = new Node(symbol, newOrderRight, false);
        oldNYT->left->parent = oldNYT;
        oldNYT->right->parent = oldNYT;

        // Atualiza o mapeamento e o ponteiro para o nó NYT
        symbolNodes[symbol] = oldNYT->right;
        NYT = oldNYT->left;

        // Atualiza a árvore a partir do nó antigo (que agora é interno)
        updateTree(oldNYT);
    } else {
        // Se o símbolo já existe, atualiza a árvore a partir do nó folha correspondente
        Node* node = symbolNodes[symbol];
        updateTree(node);
    }
}

bool ArvHuffman::contains(char symbol) {
    return symbolNodes.find(symbol) != symbolNodes.end();
}

std::string ArvHuffman::getMySymbol() {
    return mySymbol;
}

void ArvHuffman::setMySymbol(std::string s) {
    mySymbol = s;
}
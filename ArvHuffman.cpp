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
        NYT->weight++;  // Incrementa o peso do NYT toda vez que ele é referenciado
        return generateCode(NYT);
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
    if (symbolNodes.find(symbol) == symbolNodes.end()) {
        Node* oldNYT = NYT;
        
        // >>>>> REMOVER oldNYT->weight++ (não incrementamos o NYT antigo) <<<<<
        oldNYT->isNYT = false;  // Agora vira um nó interno

        // Cria os novos nós (novo NYT e folha para o novo símbolo)
        int newOrderRight = oldNYT->order - 1;
        int newOrderLeft = oldNYT->order - 2;
        oldNYT->left = new Node(0, newOrderLeft, true);
        oldNYT->right = new Node(symbol, newOrderRight, false);
        oldNYT->left->parent = oldNYT;
        oldNYT->right->parent = oldNYT;

        symbolNodes[symbol] = oldNYT->right;
        NYT = oldNYT->left;

        // >>>>> INCREMENTA O PESO DO NOVO NYT (opcional, se quiser que comece com 1) <<<<<
        NYT->weight = 1;  // Começa com peso 1, pois foi referenciado

        updateTree(oldNYT);  // Atualiza a árvore a partir do nó interno
    } else {
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

int ArvHuffman::getWeight(char sym) {
    if (findNode(sym) != nullptr) {
        ArvHuffman::Node* aux = findNode(sym);
        return aux->weight;
    }
    return NYT->weight;
}

int ArvHuffman::getTotalWeight() {
    return root ? root->weight : 0;
}

ArvHuffman::Node* ArvHuffman::findNode(char symbol) {
    if (symbolNodes.find(symbol) != symbolNodes.end()) {
        return symbolNodes[symbol];
    }
    return nullptr;
}

char ArvHuffman::decodeSymbol(const std::string& encoded, size_t& index) {
    Node* current = root;
    
    while (current && !current->isNYT && (current->left || current->right)) {
        if (index >= encoded.size()) {
            throw std::runtime_error("Erro na decodificação: índice fora do intervalo.");
        }
        
        if (encoded[index] == '0') {
            current = current->left;
        } else {
            current = current->right;
        }
        index++;
    }
    
    if (current && !current->isNYT) {
        return current->symbol;
    } else {
        throw std::runtime_error("Erro na decodificação: símbolo inválido encontrado.");
    }
}


bool ArvHuffman::containsPrefix(const std::string& prefix) {
    Node* current = root;
    
    for (char bit : prefix) {
        if (!current) return false;
        
        if (bit == '0') {
            current = current->left;
        } else if (bit == '1') {
            current = current->right;
        } else {
            return false; // Entrada inválida
        }
    }
    
    return current != nullptr;
}

char ArvHuffman::getSymbolByCode(const std::string& code) const {
    Node* current = root;
    
    for (char bit : code) {
        if (!current) {
            throw std::runtime_error("Código inválido: caminho interrompido antes do fim da codificação");
        }
        
        if (bit == '0') {
            current = current->left;
        } else if (bit == '1') {
            current = current->right;
        } else {
            throw std::runtime_error("Código inválido: contém caractere que não é 0 ou 1");
        }
    }
    
    if (current && !current->isNYT && !current->left && !current->right) {
        return current->symbol;  // Retorna o símbolo do nó folha
    } else if (current && current->isNYT) {
        return '\0';  // Retorna nulo para indicar NYT
    } else {
        throw std::runtime_error("Código inválido: não leva a um nó folha");
    }
}

bool ArvHuffman::containsCode(const std::string& code) const {
    Node* current = root;
    
    for (char bit : code) {
        if (!current) {
            return false;  // Caminho interrompido antes do fim do código
        }
        
        if (bit == '0') {
            current = current->left;
        } else if (bit == '1') {
            current = current->right;
        } else {
            return false;  // Código contém caractere inválido (não é 0 ou 1)
        }
    }
    
    // Retorna true se chegou a um nó válido (folha ou NYT)
    return current != nullptr;
}
#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <fstream>
#include <bitset>
#include <queue>
#include <algorithm>
#include "LinkedList.h"
#include <unordered_set>

class Node {
public:
    char symbol;
    int weight;
    std::shared_ptr<Node> left, right, parent;
    bool is_nyt;
    int order;  // Ordem para o algoritmo adaptativo
    
    Node(char symbol, int weight, int order, bool is_nyt = false) 
        : symbol(symbol), weight(weight), order(order), is_nyt(is_nyt), 
          left(nullptr), right(nullptr), parent(nullptr) {}
};

class AdaptiveHuffmanTree {
private:
    std::shared_ptr<Node> root;
    std::unordered_map<char, std::shared_ptr<Node>> symbol_table;
    std::shared_ptr<Node> nyt;
    int current_order;
    
    // Encontra o nó para troca durante a atualização
    std::shared_ptr<Node> find_swap_node(std::shared_ptr<Node> node) {
        auto current = root;
        std::queue<std::shared_ptr<Node>> q;
        q.push(current);
        
        std::shared_ptr<Node> swap_node = nullptr;
        
        while (!q.empty()) {
            auto n = q.front();
            q.pop();
            
            if (n != node->parent && n->weight == node->weight && n->order > node->order) {
                swap_node = n;
            }
            
            if (n->left) q.push(n->left);
            if (n->right) q.push(n->right);
        }
        
        return swap_node;
    }
    
    // Troca dois nós na árvore
    void swap_nodes(std::shared_ptr<Node> a, std::shared_ptr<Node> b) {
        if (!a || !b || a == b) return;
        
        // Troca as ordens
        std::swap(a->order, b->order);
        
        // Troca as posições na árvore
        if (a->parent->left == a && b->parent->left == b) {
            std::swap(a->parent->left, b->parent->left);
        } else if (a->parent->left == a && b->parent->right == b) {
            std::swap(a->parent->left, b->parent->right);
        } else if (a->parent->right == a && b->parent->left == b) {
            std::swap(a->parent->right, b->parent->left);
        } else {
            std::swap(a->parent->right, b->parent->right);
        }
        
        std::swap(a->parent, b->parent);
    }
    
    // Atualiza a árvore após inserção
    void update_tree(std::shared_ptr<Node> node) {
        while (node) {
            // Encontra nó para troca (se necessário)
            auto swap_node = find_swap_node(node);
            if (swap_node) {
                swap_nodes(node, swap_node);
            }
            
            node->weight++;
            node = node->parent;
        }
    }
    
    // Obtém o código Huffman para um nó
    std::string get_code(std::shared_ptr<Node> node) {
        std::string code;
        auto current = node;
        
        while (current->parent) {
            if (current->parent->left == current) {
                code += '0';
            } else {
                code += '1';
            }
            current = current->parent;
        }
        
        std::reverse(code.begin(), code.end());
        return code;
    }

public:
    AdaptiveHuffmanTree() : current_order(1000) {
        root = std::make_shared<Node>('\0', 0, current_order--, true);
        nyt = root;
    }
    
    // Insere um símbolo na árvore
    std::string insert(char symbol) {
        if (symbol_table.find(symbol) != symbol_table.end()) {
            auto node = symbol_table[symbol];
            std::string code = get_code(node);
            update_tree(node);
            return code;
        } else {
            // Cria novo nó interno
            auto new_internal = std::make_shared<Node>('\0', 1, current_order--);
            // Cria nova folha para o símbolo
            auto new_leaf = std::make_shared<Node>(symbol, 1, current_order--);
            
            // Configura a estrutura da árvore
            new_internal->left = nyt;
            new_internal->right = new_leaf;
            new_internal->parent = nyt->parent;
            
            nyt->parent = new_internal;
            new_leaf->parent = new_internal;
            
            // Atualiza a raiz se necessário
            if (!new_internal->parent) {
                root = new_internal;
            } else {
                if (new_internal->parent->left == nyt)
                    new_internal->parent->left = new_internal;
                else
                    new_internal->parent->right = new_internal;
            }
            
            // Atualiza as tabelas
            symbol_table[symbol] = new_leaf;
            
            // Cria novo NYT
            nyt = std::make_shared<Node>('\0', 0, current_order--, true);
            new_internal->left = nyt;
            nyt->parent = new_internal;
            
            // Obtém o código para o NYT + símbolo
            std::string nyt_code = get_code(new_internal);
            std::string symbol_code = std::bitset<8>(symbol).to_string();
            
            update_tree(new_internal);
            
            return nyt_code + symbol_code;
        }
    }
    
    bool has_symbol(char symbol) {
        return symbol_table.find(symbol) != symbol_table.end();
    }
};

class ContextManager {
private:
    std::unordered_map<std::string, AdaptiveHuffmanTree> contexts;
    LinkedList fallback_list;  // Lista para fallback
    int max_context_size;
    
    std::string get_fallback_context(const std::string& context) {
        return context.size() > 1 ? context.substr(1) : "";
    }
    
public:
    ContextManager(int max_ctx_size = 5) : max_context_size(max_ctx_size) {
        // Inicializa a lista com alguns símbolos comuns
        const std::string common_chars = " etaoinshrdlcumwfgypbvkjxqz ";
        for (char c : common_chars) {
            fallback_list.insereOrdenado(c, 1);  // Frequência inicial 1
        }
        fallback_list.recalculaCodificacoes();
    }
    
    std::string encode_symbol(std::string context, char symbol) {
        std::string original_context = context;
        
        while (!context.empty()) {
            if (contexts.find(context) != contexts.end()) {
                if (contexts[context].has_symbol(symbol)) {
                    std::string code = contexts[context].insert(symbol);
                    std::cout << "Símbolo: '" << symbol << "' | Contexto: '" << original_context 
                              << "' | Codificação: " << code << std::endl;
                    return code;
                }
            }
            context = get_fallback_context(context);
        }
        
        // Fallback: usa a lista encadeada para contexto vazio
        std::string code = fallback_list.buscaCodificacaoPorSimbolo(symbol);
        if (!code.empty()) {
            // Símbolo encontrado na lista - remove e recodifica
            fallback_list.remove(symbol);
            fallback_list.recalculaCodificacoes();
            std::cout << "Símbolo: '" << symbol << "' | Contexto: '" << original_context 
                      << "' (fallback list) | Codificação: " << code << std::endl;
            return code;
        } else {
            // Símbolo novo - adiciona à lista e usa codificação da árvore vazia
            fallback_list.insereOrdenado(symbol, 1);  // Frequência inicial 1
            fallback_list.recalculaCodificacoes();
            std::string new_code = fallback_list.buscaCodificacaoPorSimbolo(symbol);
            std::string nyt_code = contexts[""].insert(symbol);  // Ainda insere na árvore vazia
            std::cout << "Símbolo: '" << symbol << "' | Contexto: '" << original_context 
                      << "' (new symbol) | Codificação: " << new_code << std::endl;
            return new_code;
        }
    }

    void update_context(std::string& context, char symbol) {
        context += symbol;
        if (context.size() > max_context_size) {
            context.erase(0, 1);
        }
    }
};

class BitStreamWriter {
private:
    std::ofstream output;
    char buffer;
    int bit_count;
    
public:
    BitStreamWriter(const std::string& filename) : output(filename, std::ios::binary), buffer(0), bit_count(0) {}
    
    ~BitStreamWriter() {
        flush();
        output.close();
    }
    
    void write_bit(bool bit) {
        if (bit) {
            buffer |= (1 << (7 - bit_count));
        }
        bit_count++;
        
        if (bit_count == 8) {
            flush();
        }
    }
    
    void write_bits(const std::string& bits) {
        for (char bit : bits) {
            write_bit(bit == '1');
        }
    }
    
    void flush() {
        if (bit_count > 0) {
            output.put(buffer);
            buffer = 0;
            bit_count = 0;
        }
    }
};

int main() {
    ContextManager cm;
    std::ifstream input("input.txt", std::ios::binary);
    BitStreamWriter output("codificado.bin");
    
    if (!input) {
        std::cerr << "Erro ao abrir arquivo de entrada!\n";
        return 1;
    }
    
    std::string context = "";
    char ch;
    while (input.get(ch)) {
        std::string code = cm.encode_symbol(context, ch);
        output.write_bits(code);
        cm.update_context(context, ch);
    }
    
    input.close();
    std::cout << "Arquivo codificado com sucesso!\n";
    return 0;
}
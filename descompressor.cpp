#include <iostream>
#include <fstream>
#include <memory>
#include <unordered_map>
#include <queue>
#include <bitset>
#include <stdexcept>

using namespace std;

class BitStreamReader {
private:
    ifstream input;
    unsigned char buffer;
    int bit_pos;

public:
    BitStreamReader(const string& filename) : input(filename, ios::binary), buffer(0), bit_pos(0) {
        if (!input) throw runtime_error("Erro ao abrir arquivo");
    }

    bool read_bit() {
        if (bit_pos == 0) {
            if (!input.get((char&)buffer)) return false;
            bit_pos = 8;
        }
        return (buffer >> (--bit_pos)) & 1;
    }

    bool eof() const { return input.eof() && bit_pos == 0; }
};

class HuffmanDecoder {
    struct Node {
        char symbol;
        int weight;
        int order;
        shared_ptr<Node> left, right, parent;
        bool is_nyt;

        Node(char s, int w, int o, bool nyt = false)
            : symbol(s), weight(w), order(o), is_nyt(nyt),
              left(nullptr), right(nullptr), parent(nullptr) {}
    };

    shared_ptr<Node> root;
    shared_ptr<Node> nyt;
    unordered_map<char, shared_ptr<Node>> symbol_map;
    int next_order;

    shared_ptr<Node> find_swap_node(shared_ptr<Node> node) {
        queue<shared_ptr<Node>> q;
        q.push(root);
        shared_ptr<Node> best = nullptr;

        while (!q.empty()) {
            auto current = q.front();
            q.pop();

            if (current != node && current != node->parent && 
                current->weight == node->weight && 
                current->order > node->order) {
                if (!best || current->order > best->order) {
                    best = current;
                }
            }

            if (current->left) q.push(current->left);
            if (current->right) q.push(current->right);
        }
        return best;
    }

    void swap_nodes(shared_ptr<Node> a, shared_ptr<Node> b) {
        if (!a || !b || a == b) return;

        swap(a->order, b->order);

        if (a->parent->left == a) a->parent->left = b;
        else a->parent->right = b;

        if (b->parent->left == b) b->parent->left = a;
        else b->parent->right = a;

        swap(a->parent, b->parent);
    }

    void update_tree(shared_ptr<Node> node) {
        while (node) {
            auto swap_node = find_swap_node(node);
            if (swap_node) {
                swap_nodes(node, swap_node);
            }
            node->weight++;
            node = node->parent;
        }
    }

public:
    HuffmanDecoder() : next_order(512) {
        root = make_shared<Node>('\0', 0, next_order--, true);
        nyt = root;
    }

    bool decode(BitStreamReader& reader, char& symbol) {
        auto current = root;

        while (current->left || current->right) {
            bool bit = reader.read_bit();
            current = bit ? current->right : current->left;
            if (!current) return false;
        }

        if (current->is_nyt) {
            symbol = 0;
            for (int i = 0; i < 8; i++) {
                bool bit = reader.read_bit();
                symbol = (symbol << 1) | (bit ? 1 : 0);
            }

            auto internal = make_shared<Node>('\0', 1, next_order--);
            auto leaf = make_shared<Node>(symbol, 1, next_order--);

            internal->left = nyt;
            internal->right = leaf;
            internal->parent = nyt->parent;

            if (!internal->parent) {
                root = internal;
            } else {
                if (internal->parent->left == nyt) {
                    internal->parent->left = internal;
                } else {
                    internal->parent->right = internal;
                }
            }

            nyt->parent = internal;
            leaf->parent = internal;

            nyt = make_shared<Node>('\0', 0, next_order--, true);
            internal->left = nyt;
            nyt->parent = internal;

            symbol_map[symbol] = leaf;
            update_tree(internal);
        } else {
            symbol = current->symbol;
            update_tree(current);
        }
        return true;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <entrada.bin> <saida.txt>\n";
        return 1;
    }

    try {
        BitStreamReader reader(argv[1]);
        ofstream output(argv[2]);

        if (!output) throw runtime_error("Erro ao criar arquivo de saida");

        HuffmanDecoder decoder;
        char symbol;
        int count = 0;

        while (!reader.eof()) {
            if (decoder.decode(reader, symbol)) {
                output.put(symbol);
                count++;
            }
        }

        cout << "Descompressao concluida. " << count << " caracteres escritos.\n";
    } catch (const exception& e) {
        cerr << "Erro: " << e.what() << endl;
        return 1;
    }

    return 0;
}
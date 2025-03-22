#ifndef TEXT_PROCESSING_H
#define TEXT_PROCESSING_H

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <set>
#include <cctype>

namespace TextProcessing {

    // Função para pré-processar o texto: converte letras acentuadas e maiúsculas para minúsculas sem acento,
    // removendo vírgulas, pontos e mantendo apenas letras minúsculas e espaços.
    inline void preprocessText(const std::string& inputFileName = "./MemoriasPostumas.txt",
                               const std::string& outputFileName = "./input.txt") {
        std::ifstream inFile(inputFileName);
        std::ofstream outFile(outputFileName);

        if (!inFile || !outFile) {
            std::cerr << "Erro ao abrir os arquivos!" << std::endl;
            return;
        }

        // Mapa para converter letras acentuadas para suas versões sem acento
        std::unordered_map<char, char> mapaAcentos = {
            {'á', 'a'}, {'à', 'a'}, {'ã', 'a'}, {'â', 'a'},
            {'é', 'e'}, {'è', 'e'}, {'ê', 'e'},
            {'í', 'i'}, {'ì', 'i'}, {'î', 'i'},
            {'ó', 'o'}, {'ò', 'o'}, {'ô', 'o'}, {'õ', 'o'},
            {'ú', 'u'}, {'ù', 'u'}, {'û', 'u'},
            {'ç', 'c'},
            {'Á', 'a'}, {'À', 'a'}, {'Ã', 'a'}, {'Â', 'a'},
            {'É', 'e'}, {'È', 'e'}, {'Ê', 'e'},
            {'Í', 'i'}, {'Ì', 'i'}, {'Î', 'i'},
            {'Ó', 'o'}, {'Ò', 'o'}, {'Ô', 'o'}, {'Õ', 'o'},
            {'Ú', 'u'}, {'Ù', 'u'}, {'Û', 'u'},
            {'Ç', 'c'}
        };

        std::string linha;
        while (std::getline(inFile, linha)) {
            std::string processado;
            for (char ch : linha) {
                // Se o caractere for acentuado, substitui pela versão sem acento
                if (mapaAcentos.find(ch) != mapaAcentos.end()) {
                    processado.push_back(mapaAcentos[ch]);
                } else {
                    // Converte para minúsculo
                    char chLower = std::tolower(static_cast<unsigned char>(ch));
                    // Mantém apenas letras de 'a' a 'z' e o espaço
                    if ((chLower >= 'a' && chLower <= 'z') || chLower == ' ') {
                        processado.push_back(chLower);
                    }
                }
            }
            outFile << processado << " ";
        }

        inFile.close();
        outFile.close();
    }

    // Função para contar quantos caracteres diferentes (incluindo espaço) há no texto.
    // Por padrão, a contagem é feita no arquivo de entrada processado ("input.txt").
    inline size_t countDistinctCharacters(const std::string& inputFileName = "./input.txt") {
        std::ifstream inFile(inputFileName);
        if (!inFile) {
            std::cerr << "Erro ao abrir o arquivo!" << std::endl;
            return 0;
        }

        std::set<char> caracteresUnicos;
        std::string linha;
        while (std::getline(inFile, linha)) {
            for (char ch : linha) {
                caracteresUnicos.insert(ch);
            }
        }
        inFile.close();
        return caracteresUnicos.size();
    }

} // namespace TextProcessing

#endif // TEXT_PROCESSING_H

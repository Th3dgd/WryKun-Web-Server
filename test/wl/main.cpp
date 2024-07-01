#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <cstring>

enum class TokenType {
    Write, StringLiteral, Semicolon, End, Invalid
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
public:
    explicit Lexer(const std::string &src) : src(src), pos(0) {}
    Token getNextToken() {
        while (pos < src.length() && isspace(src[pos])) {
            ++pos;
        }

        if (pos >= src.length()) {
            return {TokenType::End, ""};
        }

        char currentChar = src[pos];

        if (isalpha(currentChar) || currentChar == ':') {
            std::string identifier;
            while (pos < src.length() && (isalnum(src[pos]) || src[pos] == ':')) {
                identifier += src[pos++];
            }
            if (identifier == "wl::write") { 
                return {TokenType::Write, identifier};
            } else {
                return {TokenType::StringLiteral, identifier};
            }
        } 

        if (currentChar == '"') {
            std::string str;
            while (++pos < src.length() && src[pos] != '"') {
                str += src[pos];
            }
            if (pos == src.length()) {
                throw std::runtime_error("[WL] - Unterminated string literal");
            }
            ++pos;
            return {TokenType::StringLiteral, str};
        }

        if (currentChar == ';') {
            ++pos;
            return {TokenType::Semicolon, ";"};
        }

        throw std::runtime_error("[WL] - invalid character: '" + std::string(1, currentChar) + "'"); 
    }

private:
    std::string src;
    size_t pos;
};

struct Elf64Header {
    uint8_t e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
};

struct Elf64ProgramHeader {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
};

void writeElfHeader(std::ofstream& outputFile) {
    Elf64Header header{};
    std::memcpy(header.e_ident, "\x7f""ELF\x02\x01\x01", 7);
    header.e_type = 2;
    header.e_machine = 0x3E;
    header.e_version = 1;
    header.e_entry = 0x400078;
    header.e_phoff = sizeof(Elf64Header);
    header.e_ehsize = sizeof(Elf64Header);
    header.e_phentsize = sizeof(Elf64ProgramHeader);
    header.e_phnum = 1;

    outputFile.write(reinterpret_cast<const char*>(&header), sizeof(header));
}

void writeProgramHeader(std::ofstream& outputFile, size_t fileSize) {
    Elf64ProgramHeader progHeader{};
    progHeader.p_type = 1;
    progHeader.p_flags = 5;
    progHeader.p_offset = 0;
    progHeader.p_vaddr = 0x400000;
    progHeader.p_paddr = 0x400000;
    progHeader.p_filesz = fileSize;
    progHeader.p_memsz = fileSize;
    progHeader.p_align = 0x200000;

    outputFile.write(reinterpret_cast<const char*>(&progHeader), sizeof(progHeader));
}

void writeInstructions(std::ofstream& outputFile, const std::vector<std::string>& instructions) {
    size_t totalTextLength = 0;
    for (const auto& instruction : instructions) {
        totalTextLength += instruction.size() + 4;
    }

    uint8_t textStart[] = {
        0xB8, 0x01, 0x00, 0x00, 0x00,
        0xBF, 0x01, 0x00, 0x00, 0x00,
        0x48, 0x8D, 0x35, 0x10, 0x00, 0x00, 0x00,
        0xBA, static_cast<uint8_t>(totalTextLength & 0xFF), static_cast<uint8_t>((totalTextLength >> 8) & 0xFF), static_cast<uint8_t>((totalTextLength >> 16) & 0xFF), static_cast<uint8_t>((totalTextLength >> 24) & 0xFF),
        0x0F, 0x05,
        0xB8, 0x3C, 0x00, 0x00, 0x00,
        0x31, 0xFF,
        0x0F, 0x05
    };

    outputFile.write(reinterpret_cast<const char*>(textStart), sizeof(textStart));

    for (const auto& instruction : instructions) {
        uint32_t strLength = instruction.size();
        outputFile.write(reinterpret_cast<const char*>(&strLength), sizeof(uint32_t));
        outputFile.write(instruction.c_str(), instruction.size());
        outputFile.put('\0');
    }
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "[WL] - Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "[WL] - Failed to open input file." << std::endl;
        return 1;
    }

    std::string sourceCode; 
    std::string dataFile;
    while (getline(inputFile, dataFile)) {
        sourceCode += dataFile + "\n";
    }
    inputFile.close();

    Lexer lexer(sourceCode);
    std::vector<std::string> instructions;

    while (true) {
        Token token = lexer.getNextToken();
        if (token.type == TokenType::End) break;
        if (token.type == TokenType::Write) {
            token = lexer.getNextToken();
            if (token.type == TokenType::StringLiteral) {
                instructions.push_back(token.value);
                token = lexer.getNextToken();
                if (token.type != TokenType::Semicolon) {
                    throw std::runtime_error("[WL] - Expected ';' after string literal");
                }
            } else {
                throw std::runtime_error("[WL] - Expected string literal after 'wl::write'");
            }
        }
    }

    std::ofstream outputFile(argv[2], std::ios::binary);
    if (!outputFile.is_open()) {
        throw std::runtime_error("[WL] - Failed to create output file.");
    }

    writeElfHeader(outputFile);

    std::streampos phOffset = outputFile.tellp();
    writeProgramHeader(outputFile, 0);

    writeInstructions(outputFile, instructions);

    std::streampos fileSize = outputFile.tellp();
    outputFile.seekp(phOffset);
    writeProgramHeader(outputFile, fileSize);

    outputFile.close();

    return 0;
}

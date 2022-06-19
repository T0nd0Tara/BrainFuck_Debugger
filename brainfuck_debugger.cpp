#include <stack>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <cassert>
#include <vector>
#include <regex>
#include <stdlib.h>

#define MEM_SIZE 30000

size_t DEBUG_CELL_RANGE = 9U;
size_t DEBUG_INST_RANGE = 15U;

namespace Color {
    enum Code {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_YELLOW   = 33,
        FG_DEFAULT  = 39,
    };
    class Modifier {
        Code code;
    public:
        Modifier(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Modifier& mod) {
            return os << "\033[" << mod.code << "m";
        }
    };
}

Color::Modifier def(Color::FG_DEFAULT);
Color::Modifier yellow(Color::FG_YELLOW);
Color::Modifier green(Color::FG_GREEN);
Color::Modifier red(Color::FG_RED);

enum OP_NAME: uint8_t{
    PLUS, MINUS,
    SHL, SHR,
    DOT, COMMA,
    BRACK_OP, BRACK_CL,
};

struct OP{
    OP_NAME op: 3;
    size_t operand: 21;
};

std::vector<OP> lexer(std::string prog){
    std::vector<OP> out;
    std::stack<size_t> brackets;
    for (size_t i=0; i < prog.size(); i++){
        switch(prog[i]){
        case '+':
            out.push_back({PLUS, 0});
            break;
        case '-':
            out.push_back({MINUS, 0});
            break;
        case '<':
            out.push_back({SHL, 0});
            break;
        case '>':
            out.push_back({SHR, 0});
            break;
        case '.':
            out.push_back({DOT, 0});
            break;
        case ',':
            out.push_back({COMMA, 0});
            break;
        case '[':
            brackets.push(out.size());
            out.push_back({BRACK_OP, 0});
            break;
        case ']':
            size_t op_ip = brackets.top();
            size_t cl_ip = out.size();
            brackets.pop();
            assert(out[op_ip].op == BRACK_OP && "Bug in the lexer");
            out[op_ip].operand = cl_ip;
            out.push_back({BRACK_CL, op_ip - 1});            
            break;         
        }
    }
    if(brackets.size() != 0){
        std::cerr << red << "ERROR: uneven brackets in the program\n" << def;
        exit(1);
    }
    return out;
}


void interpert(std::vector<OP>& prog){
    uint8_t mem[MEM_SIZE] = { 0 };
    uint8_t *ptr = mem;

    for (size_t i=0; i<prog.size(); i++){
        size_t operand = prog[i].operand;
        switch(prog[i].op){
        case PLUS:
            ++*ptr;
            break;
        case MINUS:
            --*ptr;
            break;
        case SHL:
            --ptr;
            break;
        case SHR:
            ++ptr;
            break;
        case DOT:
            putchar(*ptr);
            break;
        case COMMA:
            char c;
            do{ c = getchar();
            }while(isspace(c));

            *ptr = c;
            break;
        case BRACK_OP:
            if (*ptr == 0) i = operand;
            break;
        case BRACK_CL:
            i = operand;
            break;
        }
    }
}

void debug(std::vector<OP>& prog){
    assert(prog.size() > 0);
    uint8_t mem[MEM_SIZE] = { 0 };
    uint8_t *ptr = mem;

    std::string sOutput;
    auto evalStep = [&](size_t& i){
        size_t operand = prog[i].operand;
        switch(prog[i].op){
        case PLUS:
            ++*ptr;
            break;
        case MINUS:
            --*ptr;
            break;
        case SHL:
            --ptr;
            break;
        case SHR:
            ++ptr;
            break;
        case DOT:
            sOutput += std::string(1, *ptr);
            break;
        case COMMA:
            char c;
            do{ c = getchar();
            }while(isspace(c));
            *ptr = c;

            break;
        case BRACK_OP:
            if (*ptr == 0) i = operand;
            break;
        case BRACK_CL:
            i = operand;
            break;
        }
    };
    auto printCells = [&](){
        // 0     1      
        // +-----+-----+
        // | 255 |  0  |
        // +-----+-----+
        size_t cell_ind = ptr - mem;
        size_t firstCell = (cell_ind < DEBUG_CELL_RANGE)? 0 : cell_ind - DEBUG_CELL_RANGE;
        for (size_t scanCell = firstCell; scanCell < firstCell + 2U*DEBUG_CELL_RANGE; scanCell++){
            if (scanCell == cell_ind) std::cout << green;
            std::cout << scanCell;
            if (scanCell == cell_ind) std::cout << def;
            std::cout << std::string(6-std::to_string(scanCell).size(), ' ');
        }
        std::cout << "\n";
        for (size_t scanCell = firstCell; scanCell < firstCell + 2U*DEBUG_CELL_RANGE; scanCell++){
            if (scanCell == cell_ind) std::cout << green;
            if (cell_ind + 1 != scanCell) std::cout << "+";
            std::cout << "-----";
            if (scanCell + 1 == firstCell + 2U*DEBUG_CELL_RANGE) std::cout << "+";
            if (scanCell == cell_ind) std::cout << "+" << def;
        }
        std::cout << "\n";
        for (size_t scanCell = firstCell; scanCell < firstCell + 2U*DEBUG_CELL_RANGE; scanCell++){
            uint8_t val = mem[scanCell];
            if (scanCell == cell_ind) std::cout << green;
            if (cell_ind + 1 != scanCell) std::cout << "|";
            std::cout << std::string(4 - std::to_string(val).size(), ' ') << (uint16_t)val << " ";
            if (scanCell + 1 == firstCell + 2U*DEBUG_CELL_RANGE) std::cout << "|";
            if (scanCell == cell_ind) std::cout << "|" << def;
        }
        std::cout << "\n";
        for (size_t scanCell = firstCell; scanCell < firstCell + 2U*DEBUG_CELL_RANGE; scanCell++){
            if (scanCell == cell_ind) std::cout << green;
            if (cell_ind + 1 != scanCell) std::cout << "+";
            std::cout << "-----";
            if (scanCell + 1 == firstCell + 2U*DEBUG_CELL_RANGE) std::cout << "+";
            if (scanCell == cell_ind) std::cout << "+" << def;
        }
        std::cout << '\n';
    };
    auto printInstrucs = [&](size_t curr_ip){
        size_t firstInst = (curr_ip < DEBUG_INST_RANGE)? 0 : curr_ip - DEBUG_INST_RANGE;
        
        for (size_t ip = firstInst; ip < firstInst + 2U * DEBUG_INST_RANGE; ip++){
            if (ip >= prog.size()) break;
            if (ip == curr_ip) std::cout << green;
            switch(prog[ip].op){
            case PLUS:     std::cout << "+"; break;
            case MINUS:    std::cout << "-"; break;
            case SHL:      std::cout << "<"; break;
            case SHR:      std::cout << ">"; break;
            case DOT:      std::cout << "."; break;
            case COMMA:    std::cout << ","; break;
            case BRACK_OP: std::cout << "["; break;
            case BRACK_CL: std::cout << "]"; break;
            }
            if (ip == curr_ip) std::cout << def;
        }
    };
    size_t ip = 0;
    std::string sInput;
    while (true){
        system("clear");
        std::cout << yellow << "\t-- DEBUGGER --\n\n" << def;
        printCells();

        std::cout << "\n" << yellow << "\tIP: " << ip << "\nOPS: " << def;
        printInstrucs(ip);
        std::cout << "\n\n" << yellow << "OUTPUT:\n" << def;
        std::cout << sOutput << "\n\n";
        if (ip >= prog.size()) break;

        bool bContinue = false;
        size_t nJmp = 1;
        while(true){
        
            std::cout << "> ";
            std::getline(std::cin, sInput);
            if (sInput == ""){
                bContinue = true;
                break;
            }
            if (std::regex_match(sInput, std::regex("^c:([0-9]+)$"))){
                size_t cellNum = std::stoull(sInput.substr(2));
                if (cellNum == ptr - mem) std::cout << green;
                std::cout << "+-----+\n";
                std::cout << "|" << std::string(4 - std::to_string(mem[cellNum]).size(), ' ') << (uint16_t)mem[cellNum] << " |\n";
                std::cout << "+-----+\n";
                if (cellNum == ptr - mem) std::cout << def;
                continue;
            }
            if (std::regex_match(sInput, std::regex("^jmp ([0-9]+)$"))){
                bContinue = true;
                nJmp = std::stoull(sInput.substr(4));
                break;
            }
            
            if (std::regex_match(sInput, std::regex("^goto ([0-9]+)$"))){
                nJmp = 0;
                bContinue = true;

                size_t addr = std::stoull(sInput.substr(5));
                if (addr < ip) std::cout << yellow << "WARNING: address '" << addr << "' is before current instruction pointer. so we might not reach it...\n" << def;
                if (addr >= prog.size()){
                    std::cerr << red << "ERROR: address '" << addr << "' is out of program range. (program length: " << prog.size() << ")\n" << def;
                    continue;
                }
                while (ip != addr){
                    evalStep(ip);
                    ip++;
                    if (ip >= prog.size()) return;
                }
                break;
            }
            if (sInput == "i"){
                std::cout << ip << "\n";
                continue;
            }
            if (sInput == "q")
                break;
            
            std::cout << red << "Unknown input '" << sInput << "'\n" << def;
        }
        if (!bContinue) break;

        for (size_t i=0; i< nJmp; i++){
            evalStep(ip);
            ip++;
        }
        
    }   
}

void printUsage(std::string sCompiler){
    std::cout << yellow;
    std::cout << "\n";
    std::cout << "USAGE:\n";
    std::cout << "\t" << sCompiler << " <args> " << " <path_to_brainfuck_file>\n";
    std::cout << "\n";
    std::cout << "ARGS:\n";
    std::cout << "\t`-h`         : prints this :D\n";
    std::cout << "\t`-d`         : put you in debugger mode\n";
    std::cout << "\t`-cr <uint>` : sets the 'Cell Range' for debug mode\n";
    std::cout << "\t`-ir <uint>` : sets the 'Instruction Range' for debug mode\n";
    std::cout << "\n";
    std::cout << "DEBUG INSTRUCTIONS:\n";
    std::cout << "\t`<enter>`     : advances the instruction pointer\n";
    std::cout << "\t`i`           : prints the instruction pointer\n";
    std::cout << "\t`c:<uint>`    : prints the value inside cell <uint>\n";
    std::cout << "\t`goto <uint>` : advances the instruction pointer until it's <uint> or until end of program\n";
    std::cout << "\t`jmp <uint>`  : advances the instruction pointer  <uint> times\n";
    std::cout << "\t`q`           : quits the program\n";
    std::cout << "\n";
    std::cout << def;
}
int main(int args, char* argv[]){
    if (args < 2){
        std::cerr << red << "ERROR: incorrect use of the program. Try '" << argv[0] << " -h' for help.\n" << def;
        return 1;
    }

    for (int nArg = 1; nArg < args; nArg++){
        std::string sArg(argv[nArg]);
        if (sArg == "-h"){
            printUsage(argv[0]);
            return 0;
        }
    }
    std::string sCompiler = argv[0];
    std::string sFilePath = argv[args-1];

    std::string sProg, line;
    std::ifstream file(sFilePath);
    if (!file.is_open()){
        std::cerr << red << "ERROR: couldn't open file '" << sFilePath << "'\n" << def;
        return 1;
    }
    while(std::getline(file, line)){
        sProg += line;
    }
    file.close();

    std::vector<OP> prog = lexer(sProg);

    if (prog.size() == 0){
        std::cout << yellow << "WARNING: empty program, exiting now.\n" << def;
        return 0;
    }

    bool bDebug = false;
    for (int arg = 1; arg < args-1; arg++){
        std::string sFlag(argv[arg]);
        if (sFlag == "-d"){
            bDebug = true;
            std::cout << green << "Running debugger\n" << def;
        }
        else if (sFlag == "-cr"){ // cell range
            if (arg + 1 == args - 1){
                std::cerr << red << "ERROR: not enough arguements for -cr flag: use: `-cr <uint>`\n" << def;
                return 1;
            }
            DEBUG_CELL_RANGE = std::stoull(std::string(argv[++arg]));
        }
        else if (sFlag == "-ir"){ // instruction range
            if (arg + 1 == args - 1){
                std::cerr << red << "ERROR: not enough arguements for -ir flag: use: `-ir <uint>`\n" << def;
                return 1;
            }            
            DEBUG_INST_RANGE = std::stoull(std::string(argv[++arg]));
        }
        else
            std::cout << yellow << "WARNING: Unknown option `" << argv[arg] << "`\n" << def;
        
    }

    if (bDebug) debug(prog);
    else        interpert(prog);

    return 0;
}
#include <stdio.h>

#define QUOTE_LITERAL(x) #x
#define QUOTE(x) QUOTE_LITERAL(x)

static constexpr char code[] =
#if defined(SCRIPT_FILE)
#   include QUOTE(SCRIPT_FILE)
#elif defined(SCRIPT)
    SCRIPT
#else
    ""
#endif
;

enum class opcode {
    ptr_inc,
    ptr_dec,
    data_inc,
    data_dec,
    write,
    read,
    jmp_ifz,
    jmp
};

template<size_t IC>
struct Program {
    size_t icount;
    opcode instructions[IC];
    size_t jmp_table[IC];
};

template<size_t N>
static constexpr auto parse(const char (&str)[N]) {
    Program<N> program{};

    size_t jump_stack[N] = {};
    size_t jump_stack_top = 0;

    for (auto ptr = str; *ptr; ++ptr) {
        switch (*ptr) {
            case '>':
                program.instructions[program.icount++] = opcode::ptr_inc;
                break;
            case '<':
                program.instructions[program.icount++] = opcode::ptr_dec;
                break;
            case '+':
                program.instructions[program.icount++] = opcode::data_inc;
                break;
            case '-':
                program.instructions[program.icount++] = opcode::data_dec;
                break;
            case '.':
                program.instructions[program.icount++] = opcode::write;
                break;
            case ',':
                program.instructions[program.icount++] = opcode::read;
                break;
            case '[':
                jump_stack[jump_stack_top++] = program.icount;
                program.instructions[program.icount++] = opcode::jmp_ifz;
                break;
            case ']': {
                auto open = jump_stack[--jump_stack_top];
                auto close = program.icount++;

                program.instructions[close] = opcode::jmp;
                program.jmp_table[close] = open;

                program.jmp_table[open] = close + 1;
                break;
            }
        }
    }

    return program;
}

template<const auto& P, size_t IP = 0>
static constexpr void execute(unsigned char* tape) {
    if constexpr (IP >= P.icount) {
        return;
    } else if constexpr (P.instructions[IP] == opcode::ptr_inc) {
        ++tape;
        return execute<P, IP + 1>(tape);
    } else if constexpr (P.instructions[IP] == opcode::ptr_dec) {
        --tape;
        return execute<P, IP + 1>(tape);
    } else if constexpr (P.instructions[IP] == opcode::data_inc) {
        ++*tape;
        return execute<P, IP + 1>(tape);
    } else if constexpr (P.instructions[IP] == opcode::data_dec) {
        --*tape;
        return execute<P, IP + 1>(tape);
    } else if constexpr (P.instructions[IP] == opcode::write) {
        putchar(*tape);
        return execute<P, IP + 1>(tape);
    } else if constexpr (P.instructions[IP] == opcode::read) {
        *tape = getchar();
        return execute<P, IP + 1>(tape);
    } else if constexpr (P.instructions[IP] == opcode::jmp_ifz) {
        if (*tape == 0) {
            return execute<P, P.jmp_table[IP]>(tape);
        } else {
            return execute<P, IP + 1>(tape);
        }
    } else if constexpr (P.instructions[IP] == opcode::jmp) {
        return execute<P, P.jmp_table[IP]>(tape);
    }
}

static unsigned char memory[1024];

int main() {
    static constexpr auto program = parse(code);
    execute<program>(memory);
}

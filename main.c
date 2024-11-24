#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_TOKEN_LENGTH 100
#define NUM_KEYWORDS 20

// Tipos de Tokens
typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_KEYWORD,
    TOKEN_STRING,
    TOKEN_UNKNOWN,
    TOKEN_EOF
} TokenType;

// Estrutura de Token
typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LENGTH];
    int line;
} Token;

// Lista de Palavras Reservadas de PERL
const char *keywords[NUM_KEYWORDS] = {
    "my", "sub", "use", "strict", "if", "elsif", "else",
    "for", "foreach", "while", "do", "next", "last",
    "redo", "print", "return", "package", "require", "bless", "eval"
};

// Variável Global para Controlar a Linha Atual
int current_line = 1;

// Função para verificar se uma palavra é reservada
int is_keyword(const char *word) {
    for (int i = 0; i < NUM_KEYWORDS; i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Função para Obter Próximo Token
Token get_next_token(FILE *file) {
    Token token;
    int ch;

    // Ignorar espaços em branco e contagem de linhas
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') current_line++;
        if (!isspace(ch)) break;
    }

    // Verificar Fim do Arquivo
    if (ch == EOF) {
        token.type = TOKEN_EOF;
        strcpy(token.value, "EOF");
        token.line = current_line;
        return token;
    }

    // Identificar Números
    if (isdigit(ch)) {
        int i = 0;
        token.value[i++] = ch;
        while ((ch = fgetc(file)) != EOF && isdigit(ch)) {
            token.value[i++] = ch;
        }
        token.value[i] = '\0';
        if (ch != EOF) ungetc(ch, file);
        token.type = TOKEN_NUMBER;
        token.line = current_line;
        return token;
    }

    // Identificar Identificadores e Palavras Reservadas
    if (isalpha(ch) || ch == '_') {
        int i = 0;
        token.value[i++] = ch;
        while ((ch = fgetc(file)) != EOF && (isalnum(ch) || ch == '_')) {
            token.value[i++] = ch;
        }
        token.value[i] = '\0';
        if (ch != EOF) ungetc(ch, file);

        if (is_keyword(token.value)) {
            token.type = TOKEN_KEYWORD;
        } else {
            token.type = TOKEN_IDENTIFIER;
        }
        token.line = current_line;
        return token;
    }

    // Identificar Strings
    if (ch == '"') {
        int i = 0;
        while ((ch = fgetc(file)) != EOF && ch != '"') {
            token.value[i++] = ch;
        }
        token.value[i] = '\0';
        if (ch == EOF) {
            token.type = TOKEN_UNKNOWN;
        } else {
            token.type = TOKEN_STRING;
        }
        token.line = current_line;
        return token;
    }

    // Identificar Operadores Simples
    if (strchr("+-*/=;(){}", ch)) {
        token.type = TOKEN_OPERATOR;
        token.value[0] = ch;
        token.value[1] = '\0';
        token.line = current_line;
        return token;
    }

    // Qualquer outra coisa é um token desconhecido
    token.type = TOKEN_UNKNOWN;
    token.value[0] = ch;
    token.value[1] = '\0';
    token.line = current_line;
    return token;
}

// Analisador Léxico
void lexical_analysis(FILE *file) {
    Token token;
    printf("Análise Léxica:\n");
    do {
        token = get_next_token(file);
        printf("Linha: %d | Tipo: %d | Valor: %s\n", token.line, token.type, token.value);
    } while (token.type != TOKEN_EOF);
}

// Função Principal
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo_perl>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    lexical_analysis(file);

    fclose(file);
    return 0;
}

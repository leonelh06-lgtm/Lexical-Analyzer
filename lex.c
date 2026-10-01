/*
Homework:
lex - HW2 PL/0 lexical analyzer
Author(s): <Leonel Hernandez>
Language: C only
To Compile:
gcc -Wall -Wextra -std=c11 -O2 lex.c -o lex
To Execute (on Eustis):
./lex <input_file>
where:
<input_file> is the path to a text file holding a PL/0 source program
Notes:
- Implements the lexical analyzer described in the homework
instructions.- Prints four sections to standard output: Source Program, Lexeme
Table, Name Table and Token List.
- Writes two files into the working directory: tokens.txt and
nametable.txt.
- Stops at the first lexical error, prints everything scanned before
it, reports the error with its line and column, and exits with a
non-zero status.
- Exits with status 0 when the whole program scans without an error.
- Tested on Eustis.
Class: COP 3402 - Systems Software
Instructor: Jie Lin, Ph.D.
Due Date: 10/2/2026
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define MAX_LEXEME_LEN 16

typedef struct{
char lexeme[MAX_LEXEME_LEN];
int tokenCode;
int line;
int col;
int nameIdx;
}Token;

void addToken(Token* tokens, int* tCount, char* lexeme, int tokenCode, int line, int col);
int main(int argc, char* argv[]){
//Check if command line promt is incorrect
if(argc != 2){
    printf("Usage: ./lex <input file>");
    return 1;
}
FILE* ifp = fopen(argv[1], "r");

//Checks if file is empty
if(ifp == NULL){
    printf("Error: unable to open input file '%s'", argv[1]);
    return 1;
}

//Creates token and Nameable output files
FILE* tokenFile = fopen("tokens.txt", "w");
FILE* namableFile = fopen("nametable.txt", "w");

int charCount = 0;
int ch;

//Should have done parallel array for symbols too but I came up with the idea after :(
const char* reservedWords[] = {"begin", "end", "if", "fi", "then", "while", "elihw", "do", "od", "odd", "call", "const", "var", "procedure", "write", "read", "else"};
const int reserverdCodes[] = {20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36};
const int wordsReserved = 17;


//Gets length of file to dyanmically allocate charStream
while((ch = fgetc(ifp)) != EOF){
    charCount++;
}
rewind(ifp);

//Allocates space for tokenStream & names Array of Tokens
Token* tokens = malloc(sizeof(Token) * charCount);
int tCount = 0;
Token* names = malloc(sizeof(Token) * charCount);
int nameCount = 0;

//Header for Source Program section
printf("Source Program:\n\n");
//Stores the character stream and prints out source program
char* charStream = malloc(sizeof(char) * (charCount + 1));
for(int i = 0; i< charCount; i++){
    fscanf(ifp, "%c", &charStream[i]);
    printf("%c", charStream[i]);
}
printf("\n");

//Variables used to store current location
int i = 0;
int col = 1;
int line = 1;
int errCode = 0;
int errLine;
int errCol;
char errMsg[200];


while(i<charCount){
    char ch = charStream[i];

    //Condtional Logic if ch is one of the four white space chars
    if(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r'){
        //Check for newline
        if(ch == '\n'){
            line++;
            col = 1;
        }
        //Check for Carriage Return
        else if(ch == '\r'){
            //Carraige return does nothing since it just follows \n
        }
        else{
            col++;
        }
        i++;
        continue;
    }

    int startLine = line;
    int startCol = col;

    //Handles identifiers and reserved words
    if(isalpha((unsigned char) ch)){
        //Creates string to store lexeme
        char lexeme[MAX_LEXEME_LEN];
        int lexIdx = 0;

        //Used to track length for errror 2
        int start = i;

        //Stores uninterrupted letters and numbers in lexeme
        while(i<charCount && isalnum((unsigned char) charStream[i])){

            if(lexIdx < MAX_LEXEME_LEN - 1){
                lexeme[lexIdx++] = charStream[i];
            }
            i++;
            col++;
        }
        lexeme[lexIdx] = '\0';

        int len = i - start;

        //Check length of lexeme
        if(len > 12){
            errCode = 2;
            errLine = startLine;
            errCol = startCol;
            //Gets adress to start reading at then prints full too long identifier
            snprintf(errMsg, sizeof(errMsg), "identifier too long '%.*s'", len, charStream + start);
            break;
        }
        
        //Flag to check if reserved word
        int reserved = 0;

        //Checks for reserved words
        for(int f = 0; f<wordsReserved; f++){
            //If reserved word logic
            if(strcmp(lexeme, reservedWords[f]) == 0){
                reserved = 1;
                addToken(tokens, &tCount, lexeme, reserverdCodes[f], startLine, startCol);
                break;
            }
        }

        //If identifier logic
        if(!reserved){
            int n;
            for(n = 0; n<nameCount; n++){
                //Only adds to names first time an identifier is
                if(strcmp(names[n].lexeme, lexeme) == 0){
                    break;
                }
            }
            //Unique identifier
            if(n == nameCount){
                strcpy(names[nameCount].lexeme, lexeme);
                names[nameCount].line = startLine;
                names[nameCount].col = startCol;
                nameCount++;
            }

            //Wether or not added to names still is added to tokenStream
            addToken(tokens, &tCount, lexeme, 1, startLine, startCol);
            tokens[tCount - 1].nameIdx = n;
        }
        //Starts next loop for next lexeme
        continue;
        
    }

    //Handles numbers
    else if(isdigit((unsigned char) ch)){
        char lexeme[MAX_LEXEME_LEN];
        int lexIdx = 0;

        int start = i;

        while(i<charCount && isalnum( (unsigned char) charStream[i])){
            if(lexIdx < MAX_LEXEME_LEN - 1){
                lexeme[lexIdx++] = charStream[i];
            }
            i++;
            col++;
        }
        lexeme[lexIdx] = '\0';
        int len = i - start;

        //Checks for error 6
        int hasLetter = 0;
        for(int j = 0; lexeme[j] != '\0'; j++){
            if(isalpha((unsigned char)lexeme[j])){
                hasLetter = 1;
                break;
            }
        }
        if(hasLetter){
            errCode = 6;
            errLine = startLine;
            errCol = startCol;
            snprintf(errMsg, sizeof(errMsg), "number followed by a letter '%.*s'", len, charStream + start);
            break;
        }
        if(len > 6){
            errCode = 3;
            errLine = startLine;
            errCol = startCol;
            snprintf(errMsg, sizeof(errMsg), "number too long '%.*s'", len, charStream + start);
            break;
        }
        addToken(tokens, &tCount, lexeme, 2, startLine, startCol);
        //Starts next loop for next lexeme
        continue;
    }

 //Handles Comments

 //Comments
 //Checks for "/*"
    if(ch == '/' && i + 1 < charCount && charStream[i + 1] == '*'){
        int commentStartLine = line;
        int commentStartCol = col;
        i += 2;
        col += 2;

        //Initialize comment close flag
        int closed = 0;

        //Loops through comment
        while(i<charCount){
            //Checks if there is a comment while inside the comment
            if(charStream[i] == '/' && i + 1 < charCount && charStream[i + 1] == '*'){
                errCode = 9;
                errLine = line;
                errCol = col;
                snprintf(errMsg, sizeof(errMsg), "'/*' inside a comment");
                break;
            }

            //checks for closing comment "*/"
            if(charStream[i] == '*' && i + 1<charCount && charStream[i + 1] == '/'){
                i+= 2;
                col += 2;
                closed = 1;
                break;
            }

            //Brings current position to first column of the nextline
            if(charStream[i] == '\n'){
                line++;
                col = 1;
            }

            //Any character other than carriage return will advance column of current position
            else if(charStream[i] != '\r'){
                col++;
            }
            //Advances charStream element while inside comment
            i++;


        }

        //errCode condition prevents collision with error 9
        if(!closed && errCode == 0){
            errCode = 7;
            errLine = commentStartLine;
            errCol = commentStartCol;
            snprintf(errMsg, sizeof(errMsg), "comment is not closed before end of file");
            break;
        }

        //If theres error 7 or 9 then get out of the loop and print results
        if(errCode != 0)
            break;
        
        continue;
        }


    //Handles Special Symbols

    /*Creates token and adds into token stream 
    then increment i and col by 1 if its a single
    char symbol 2 if its a double char symbol*/
    switch(ch)
    {
        case '+': 
            addToken(tokens, &tCount, "+", 3, line, col);
            i++;
            col++;
            break;

        case '-': 
            addToken(tokens, &tCount, "-", 4, line, col);
            i++;
            col++;
            break;

        case '*': 
            if(i+ 1 < charCount && charStream[i + 1] == '/'){
                errCode = 8;
                errLine = line;
                errCol = col;
                snprintf(errMsg, sizeof(errMsg), "'*/' without a matching '/*'");
            }
            else{
                addToken(tokens, &tCount, "*", 5, line, col);
            i++;
            col++;
            }
            break;

        case '/': 
            addToken(tokens, &tCount, "/", 6, line, col);
            i++;
            col++;
            break;

        case '=' : 
            if(i + 1< charCount && charStream[i + 1] == '='){
                addToken(tokens, &tCount, "==", 7, line, col);
                i+= 2;
                col+= 2;
                break;
            }
            else{
                addToken(tokens, &tCount, "=", 18, line, col);
                i++;
                col++;
                break;
            }

        case '!':
            if(i + 1 < charCount && charStream[i + 1] == '='){
                addToken(tokens, &tCount, "!=", 8, line, col);
                i+= 2;
                col+= 2;
                break;
            }
            else{
                errCode = 5;
                errLine = line;
                errCol = col;
                snprintf(errMsg, sizeof(errMsg), "'!' must be followed by '='");
                break;
            }

        case '<':
            if(i + 1 <  charCount && charStream[i + 1] == '='){
                addToken(tokens, &tCount, "<=", 10, line, col);
                i += 2;
                col+= 2;
                break;
            }
            else{
                addToken(tokens, &tCount, "<", 9, line, col);
                i++;
                col++;
                break;
            }

        case '>':
            if(i + 1 < charCount && charStream[i + 1] == '='){
                addToken(tokens, &tCount, ">=", 12, line, col);
                i+= 2;
                col+= 2;
                break;
            }
            else{
                addToken(tokens, &tCount, ">", 11, line, col);
                i++;
                col++;
                break;
            }

        case '(':
            addToken(tokens, &tCount, "(", 13, line, col);
            i++;
            col++;
            break;

        case ')': 
        addToken(tokens, &tCount, ")", 14, line, col);
        i++;
        col++;
        break;

        case ',': 
        addToken(tokens, &tCount, ",", 15, line, col);
        i++;
        col++;
        break;

        case ';': 
        addToken(tokens, &tCount, ";", 16, line, col);
        i++;
        col++;
        break;

        case '.': 
        addToken(tokens, &tCount, ".", 17, line, col);
        i++;
        col++;
        break;

        case ':':
            if(i + 1 < charCount && charStream[i + 1] == '='){
                addToken(tokens, &tCount, ":=", 19, line, col);
                i+= 2;
                col+= 2;
                break;
            }
            else{
                errCode = 4;
                errLine = line;
                errCol = col;
                snprintf(errMsg, sizeof(errMsg), "':' must be followed by '='");
                break;
            }
            
        default:
            if(isprint((unsigned char) ch)){
                errCode = 1;
                errLine = line;
                errCol = col;
                snprintf(errMsg, sizeof(errMsg), "invalid character '%c'", ch);
                break;
            }
            else{
                errCode = 10;
                errLine = line;
                errCol = col;
                snprintf(errMsg, sizeof(errMsg), "byte 0x%02X is not part of this language", (unsigned char)ch);
                break;
            }
    }

    if(errCode != 0)
        break;
}

//Checks if any tokens
if(tCount == 0 && errCode == 0){
    errCode = 11;
    errLine = 1;
    errCol = 1;
    snprintf(errMsg, sizeof(errMsg), "no tokens in the source program");
}

//Handles output in main .txt file

//Lexeme Table
printf("\nLexeme Table:\n");
printf("\nlexeme\ttoken\n");
for(int i = 0; i<tCount; i++){
    printf("%s\t\t\t%d\n", tokens[i].lexeme, tokens[i].tokenCode);
}
//Name Table
printf("\nName Table:\n\n");
printf("index\tname\t\tline\tcolumn\n");
for(int i = 0; i<nameCount; i++){
    printf("%d\t\t\t%s\t\t\t%d\t\t\t%d\n", i, names[i].lexeme, names[i].line, names[i].col);
}

//Token List
printf("\nToken List:\n\n");
for(int i = 0; i<tCount; i++){
    if(tokens[i].tokenCode == 1){
        printf("%d %d ", tokens[i].tokenCode, tokens[i].nameIdx);
    }
    else if(tokens[i].tokenCode == 2){
        printf("%d %s ", tokens[i].tokenCode, tokens[i].lexeme);
    }
    else{
    printf("%d ", tokens[i].tokenCode);
    }
}
printf("\n");

// Writes results into token.txt and nameable.txt
for(int i = 0; i < tCount; i++){
    if(tokens[i].tokenCode == 1){
        fprintf(tokenFile, "%d %d\n", tokens[i].tokenCode, tokens[i].nameIdx);
    }
    else if(tokens[i].tokenCode == 2){
       fprintf(tokenFile, "%d %s\n", tokens[i].tokenCode, tokens[i].lexeme);
    }
    else{
        fprintf(tokenFile, "%d\n", tokens[i].tokenCode);
    }
}

for(int i = 0; i < nameCount; i++){
    fprintf(namableFile, "%d %s %d %d\n", i, names[i].lexeme, names[i].line, names[i].col);
}

fclose(tokenFile);
fclose(namableFile);

//Prints error codes
if(errCode != 0){
    printf("Error %d at line %d, column %d: %s\n", errCode, errLine, errCol, errMsg);
}

//Frees allocated memory
free(tokens);
free(names);
free(charStream);

return errCode != 0;
}
void addToken(Token* tokens, int* tCount, char* lexeme, int tokenCode, int line, int col){
    Token t;
    strcpy(t.lexeme, lexeme);
    t.tokenCode = tokenCode;
    t.line = line;
    t.col = col;
    tokens[*tCount] = t;
    (*tCount)++;
}
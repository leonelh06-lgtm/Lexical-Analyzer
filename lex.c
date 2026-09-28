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
#define MAX_LEXEME_LEN 1000

typedef struct{
char lexeme[MAX_LEXEME_LEN];
int tokenType;
int line;
int col;
}Token;

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
Token* tokens = malloc(sizeof(Token) * charCount);

//Gets length of file to dyanmically allocate charStream
while((ch = fgetc(ifp)) != EOF){
    charCount++;
}
rewind(ifp);

printf("Source Program:\n\n");

//Stores the character stream and prints out source program
char* charStream = malloc(sizeof(char) * (charCount + 1));
for(int i = 0; i< charCount; i++){
    fscanf(ifp, "%c", &charStream[i]);
    printf("%c", charStream[i]);
}

//Variables used to store current location
int i = 0;
int col = 1;
int line = 0;

while(i<charCount){
    char ch = charStream[i];

    //Condtional Logic if ch is one of the four space characters
    if(isspace(ch)){
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
    if(isalpha(ch)){
        //Creates string to store lexeme
        char lexeme[MAX_LEXEME_LEN];
        int lexIdx = 0;

        //Stores uninterrupted letters and numbers in lexeme
        while(i<charCount && isalnum(charStream[i])){
            if(lexIdx < MAX_LEXEME_LEN - 1){
                lexeme[lexIdx++] = charStream[i];
            }
            i++;
            col++;
        }
        lexeme[lexIdx] = '\0';

        //Check length of lexeme
        if(strlen(lexeme) > 12){
            printf("Error 2 at line %d, column %d: identifier too long '%s'\n", startLine, startCol, lexeme);
            return 1;
        }
    }
    //Handles numbers
    else if(isdigit(ch)){
        char lexeme[MAX_LEXEME_LEN];
        int lexIdx = 0;

        while(i<charCount && isalnum(charStream[i])){
            if(lexIdx < MAX_LEXEME_LEN - 1){
                lexeme[lexIdx++] = charStream[i];
            }
            i++;
            col++;
        }
        lexeme[lexIdx] = '\0';

        int hasLetter = 0;
        for(int j = 0; lexeme[j] != '\0'; j++){
            if(isalpha(lexeme[j])){
                hasLetter = 1;
                break;
            }
        }
        if(hasLetter){
            printf("Error 6 at line %d, column %d: number followed by a letter %s.\n", startLine, startCol, lexeme);
            return 1;
        }
        if(strlen(lexeme) > 6){
            printf("Error 3 at line %d, column %d: number too long %s.\n", startLine, startCol, lexeme);
            return 1;
        }
    }

 //Handles Opperators & Comments

 //Comments
 //Checks for "/*"
    if(ch == '/' && i + 1 < charCount && charStream[i + 1] == '*'){
        int commentStartLine = line;
        int commentStartCol = col;
        i += 2;
        col += 2;

        //Flag for if the comment closes
        int closed = 0;
        while(i<charCount){
            //Checks if there is a comment while inside the comment
            if(charStream[i] == '/' && i + 1 < charCount && charStream[i + 1] == '*'){
                printf("Error 9 at line %d, column %d: ’/*’ inside a comment.\n", line, col);
                return 1;
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

        if(!closed){
            printf("Error 7 at line %d, column %d: comment is not closed before end of file.\n", commentStartLine, commentStartCol);
            return 1;
        }
        continue;

        switch(ch)
        {
            case ':':
                if(i + 1 < charCount && charStream[i + 1] == '='){
                    //Token 19 (:=)

                }
                else{
                    printf("Error 4 at line %d, column %d: ’:’ must be followed by ’=’. A colon that is not part of :=.", line, col);
                }
        }
    }



}

printf("\nLexeme Table:\n\n");
printf("lexeme\ttoken\n");
free(charStream);
}

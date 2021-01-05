#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include "neillsdl2.h"

#define strsame(A, B) (strcmp(A, B) == 0)
#define NINE 57
#define ZERO 48
#define MINUS 45
#define POINT 46
#define PLUS 43
#define ASTERISK 42
#define SLASH 47
#define STRING p->wds[p->cw]
#define NEXTSTRING p->cw = p->cw + 1;

struct prog {
    char wds[1000][1000];
    /* Current wd*/
    int cw;
};

typedef struct prog Program;

enum bool {false, true};
typedef enum bool bool;

void test(void);

bool Prog(Program *p);
bool Instructlst(Program *p);
bool isnumber(Program *p);
bool Instruction(Program *p);
bool FD(Program *p);
bool LT(Program *p);
bool RT(Program *p);
bool DO(Program *p);
bool isvar(Program *p);
bool varnum(Program *p);
bool OP(Program *p);
bool Polish(Program *p);
bool Set(Program *p);
bool Do(Program *p); 
void Draw(Program *p);

int main(int argc, char** argv) {

    int i;
    FILE *fp;
    Program prog;
    
    prog.cw = 0;

    test();

    if (argc != 2) {
        fprintf(stderr, "You have not included the right arguments\n");
        exit(2);
    }

    for (i = 0; i < 1000; i++) {
        prog.wds[i][0] = '\0';
    }
    
    if (!(fp = fopen(argv[1], "r"))) {
        fprintf(stderr, "Cannot open %s\n", argv[1]);
        exit(2);
    }

    i = 0;

    while (fscanf(fp, "%s", prog.wds[i++]) == 1 && i < 1000);

    Prog(&prog);

    printf("Parsed OK\n");

    return 0;

}

void Draw(Program *p) {

    SDL_RenderDrawLine(p->renderer, STRING, )
}

bool isnumber(Program *p) {

    int i = 0, len = strlen(STRING);

    /*Check ASCII Codes*/
    while(i < len) {
        if ((STRING[i] > NINE || STRING[i] < ZERO) && \
        (STRING[i] != POINT && STRING[i] != MINUS )) {
            return false;
        }
        i++;
    }
    return true;
}

bool FD(Program *p) {

    if (strsame(STRING, "FD")) {
        NEXTSTRING
        if (!isnumber(p)) {
            return false;
        }
        NEXTSTRING
        return true;
    }
    return false;
}

bool LT(Program *p) {

    if (strsame(STRING, "LT")) {
        NEXTSTRING
        if (!isnumber(p)) {
            return false;
        }
        NEXTSTRING
        return true;
    }
    return false;
}
    
bool RT(Program *p) {

    if (strsame(STRING, "RT")) {
        NEXTSTRING
        if (!isnumber(p)) {
            return false;
        }
        NEXTSTRING
        return true;
    }
    return false;
}

bool isvar(Program *p) {
   
   /*Cast to int pointer and dereference*/
   if (!isalpha(*(int*)STRING)) {
        return false;
    }
    return true;
}

bool varnum(Program *p) {

    if (isnumber(p) || isvar(p)) {
        NEXTSTRING
        return true;
    }
    return false;
}

bool OP(Program *p) {
    
    /*Cast to int pointer and dereference*/
    switch(*(int*)STRING){
        case MINUS:
        case PLUS:
        case ASTERISK:
        case SLASH:
            NEXTSTRING
            return true;
        default:
            return false; 
    }
}

bool Polish(Program *p) {

    if (strsame(STRING, ";")){
        NEXTSTRING
        return true;
    }
    if (OP(p)) {
        Polish(p);
        return true;
    }
    if (varnum(p)){
        Polish(p);
        return true;
    }
    else {
        fprintf(stderr, "Failed Polish\n");
        exit(2);
    }
    return false;
}

bool Set(Program *p) {

    if (!strsame(STRING, "SET")){
        return false;
    }
    NEXTSTRING
    if(!isvar(p)) {
        return false;
    }
    NEXTSTRING
    if (!strsame(STRING, ":=")) {
        return false;
    }
    NEXTSTRING
    if (!Polish(p)) {
        return false;
    }
    return true;
}

bool Do(Program *p){
    
    if (!strsame(STRING, "DO")) {
        return false;
    }
    NEXTSTRING
    if (!isvar(p)) {
        return false;
    }
    NEXTSTRING
    if (!strsame(STRING, "FROM")) {
        return false;
    }
    NEXTSTRING
    if (!varnum(p)){
        return false;
    }
    if (!strsame(STRING, "TO")) {
        return false;
    }
    NEXTSTRING
    if (!varnum(p)){
        return false;
    }
    if (!strsame(STRING, "{")) {
        return false;
    }
    NEXTSTRING
    Instructlst(p);
    return true;
}

bool Instruction(Program *p) {

    if (FD(p) || LT(p) || RT(p) || Do(p) || Set(p)) {
        return true;
    }
    return false;
}

bool Instructlst(Program *p) {

    if (strsame(STRING, "}")) {
        NEXTSTRING
        return true;
    }
    if (!Instruction(p)) {
        return false;
    }
    Instructlst(p);
    return true;
}

bool Prog(Program *p) {

    /*Test for NULL at the beginning */
    if (p == NULL) {
        return false;
    }
    
    if (!strsame(STRING, "{")){
        fprintf(stderr, "Error: No start of program\n");
        exit(2);
    }
    NEXTSTRING
    Instructlst(p);
    return true;
}



void test(void){

    Program prog;
    char str[1000];
    int i, j;

    /*Test Prog function (maybe last function to be tested)*/
    /* Test isnumber first*/
    strcpy(str, "27383");
    sscanf(str, "%s", prog.wds[0]);
    assert(isnumber(&prog));
    strcpy(str, "2892");
    sscanf(str, "%s", prog.wds[0]);
    assert(isnumber(&prog));
    strcpy(str, "3182");
    sscanf(str, "%s", prog.wds[0]);
    assert(isnumber(&prog));
    strcpy(str, "3");
    sscanf(str, "%s", prog.wds[0]);
    assert(isnumber(&prog));
    /*Works for negative numbers*/
    strcpy(str, "-3182");
    sscanf(str, "%s", prog.wds[0]);
    assert(isnumber(&prog));
    /*Does not work for strings*/
    strcpy(str, "LT");
    sscanf(str, "%s", prog.wds[0]);
    assert(!isnumber(&prog));
    strcpy(str, "Hello");
    sscanf(str, "%s", prog.wds[0]);
    assert(!isnumber(&prog));
    /*Works for decimals*/
    strcpy(str, "772.2823");
    sscanf(str, "%s", prog.wds[0]);
    assert(isnumber(&prog));

    
    /*Clean prog*/
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            prog.wds[i][j] = 0;
        }
    }
    prog.cw = 0;

    /*Test FD function*/
    strcpy(str, "FD");
    sscanf(str, "%s", prog.wds[0]);
    strcpy(str, "2892");
    sscanf(str, "%s", prog.wds[1]);
    assert(FD(&prog));
    strcpy(str, "FD");
    sscanf(str, "%s", prog.wds[2]);
    strcpy(str, "25");
    sscanf(str, "%s", prog.wds[3]);
    assert(FD(&prog));
    /*Check false for other instructions*/
    strcpy(str, "LT");
    sscanf(str, "%s", prog.wds[4]);
    strcpy(str, "25");
    sscanf(str, "%s", prog.wds[5]);
    assert(!FD(&prog));
    strcpy(str, "RT");
    sscanf(str, "%s", prog.wds[4]);
    strcpy(str, "25");
    sscanf(str, "%s", prog.wds[5]);
    assert(!FD(&prog));
    strcpy(str, "DO");
    sscanf(str, "%s", prog.wds[4]);
    strcpy(str, "25");
    sscanf(str, "%s", prog.wds[5]);
    assert(!FD(&prog));

    /*Clean prog*/
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            prog.wds[i][j] = 0;
        }
    }
    prog.cw = 0;

    /*Test LT function*/
    strcpy(str, "LT");
    sscanf(str, "%s", prog.wds[0]);
    strcpy(str, "39");
    sscanf(str, "%s", prog.wds[1]);
    assert(LT(&prog));
    strcpy(str, "LT");
    sscanf(str, "%s", prog.wds[2]);
    strcpy(str, "90");
    sscanf(str, "%s", prog.wds[3]);
    assert(LT(&prog));
    /*Check false for other instructions*/
    strcpy(str, "FD");
    sscanf(str, "%s", prog.wds[4]);
    strcpy(str, "25");
    sscanf(str, "%s", prog.wds[5]);
    assert(!LT(&prog));
    strcpy(str, "RT");
    sscanf(str, "%s", prog.wds[4]);
    strcpy(str, "25");
    sscanf(str, "%s", prog.wds[5]);
    assert(!LT(&prog));
    strcpy(str, "DO");
    sscanf(str, "%s", prog.wds[4]);
    strcpy(str, "25");
    sscanf(str, "%s", prog.wds[5]);
    assert(!LT(&prog));

    /*Clean prog*/
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            prog.wds[i][j] = 0;
        }
    }
    prog.cw = 0;

    /*Test RT function*/
    strcpy(str, "RT");
    sscanf(str, "%s", prog.wds[0]);
    strcpy(str, "5");
    sscanf(str, "%s", prog.wds[1]);
    assert(RT(&prog));
    strcpy(str, "RT");
    sscanf(str, "%s", prog.wds[2]);
    strcpy(str, "12");
    sscanf(str, "%s", prog.wds[3]);
    assert(RT(&prog));
    /*Check false for other instructions*/
    strcpy(str, "FD");
    sscanf(str, "%s", prog.wds[4]);
    strcpy(str, "49");
    sscanf(str, "%s", prog.wds[5]);
    assert(!RT(&prog));
    strcpy(str, "LT");
    sscanf(str, "%s", prog.wds[4]);
    strcpy(str, "1");
    sscanf(str, "%s", prog.wds[5]);
    assert(!RT(&prog));
    strcpy(str, "DO");
    sscanf(str, "%s", prog.wds[4]);
    strcpy(str, "29");
    sscanf(str, "%s", prog.wds[5]);
    assert(!RT(&prog));

    /*Clean prog*/
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            prog.wds[i][j] = 0;
        }
    }
    prog.cw = 0;

    /*Test isvar function*/
    strcpy(str, "a");
    sscanf(str, "%s", prog.wds[0]);
    assert(isvar(&prog));
    strcpy(str, "b");
    sscanf(str, "%s", prog.wds[0]);
    assert(isvar(&prog));
    strcpy(str, "z");
    sscanf(str, "%s", prog.wds[0]);
    assert(isvar(&prog));
    strcpy(str, "L");
    sscanf(str, "%s", prog.wds[0]);
    assert(isvar(&prog));

    /*Clean prog*/
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            prog.wds[i][j] = 0;
        }
    }
    prog.cw = 0;

    /*Test varnum*/
    strcpy(str, "a");
    sscanf(str, "%s", prog.wds[0]);
    assert(varnum(&prog));
    strcpy(str, "23");
    sscanf(str, "%s", prog.wds[1]);
    assert(varnum(&prog));
    strcpy(str, "P");
    sscanf(str, "%s", prog.wds[2]);
    assert(varnum(&prog));
    strcpy(str, "-23.7");
    sscanf(str, "%s", prog.wds[3]);
    assert(varnum(&prog));
    /*Check for false cases*/
    strcpy(str, "P4");
    sscanf(str, "%s", prog.wds[4]);
    assert(!varnum(&prog));
    strcpy(str, "D9");
    sscanf(str, "%s", prog.wds[4]);
    assert(!varnum(&prog));

    /*Clean prog*/
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            prog.wds[i][j] = 0;
        }
    }
    prog.cw = 0;


    /*Test OP*/
    strcpy(str, "*");
    sscanf(str, "%s", prog.wds[0]);
    assert(OP(&prog));
    strcpy(str, "+");
    sscanf(str, "%s", prog.wds[1]);
    assert(OP(&prog));
    strcpy(str, "-");
    sscanf(str, "%s", prog.wds[2]);
    assert(OP(&prog));
    strcpy(str, "/");
    sscanf(str, "%s", prog.wds[3]);
    assert(OP(&prog));
    /*Check for false cases*/
    strcpy(str, "12");
    sscanf(str, "%s", prog.wds[4]);
    assert(!OP(&prog));
    strcpy(str, "H");
    sscanf(str, "%s", prog.wds[4]);
    assert(!OP(&prog));

    /*Clean prog*/
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            prog.wds[i][j] = 0;
        }
    }
    prog.cw = 0;

    /*Test Polish*/
    strcpy(str, "*");
    sscanf(str, "%s", prog.wds[0]);
    strcpy(str, "45");
    sscanf(str, "%s", prog.wds[1]);
    strcpy(str, ";");
    sscanf(str, "%s", prog.wds[2]);
    assert(Polish(&prog));
    strcpy(str, "/");
    sscanf(str, "%s", prog.wds[3]);
    strcpy(str, "A");
    sscanf(str, "%s", prog.wds[4]);
    strcpy(str, ";");
    sscanf(str, "%s", prog.wds[5]);
    assert(Polish(&prog));
    strcpy(str, "-");
    sscanf(str, "%s", prog.wds[6]);
    strcpy(str, "9");
    sscanf(str, "%s", prog.wds[7]);
    strcpy(str, "A");
    sscanf(str, "%s", prog.wds[8]);
    strcpy(str, "-23.2");
    sscanf(str, "%s", prog.wds[9]);
    strcpy(str, "15");
    sscanf(str, "%s", prog.wds[10]);
    strcpy(str, ";");
    sscanf(str, "%s", prog.wds[11]);
    assert(Polish(&prog));

        /*Clean prog*/
    for (i = 0; i < 11; i++) {
        for (j = 0; j < 5; j++) {
            prog.wds[i][j] = 0;
        }
    }
    prog.cw = 0;
    
    /*Test Set*/
    strcpy(str, "SET");
    sscanf(str, "%s", prog.wds[0]);
    strcpy(str, "L");
    sscanf(str, "%s", prog.wds[1]);
    strcpy(str, ":=");
    sscanf(str, "%s", prog.wds[2]);
    strcpy(str, ";");
    sscanf(str, "%s", prog.wds[3]);
    assert(Set(&prog));
    strcpy(str, "SET");
    sscanf(str, "%s", prog.wds[4]);
    strcpy(str, "a");
    sscanf(str, "%s", prog.wds[5]);
    strcpy(str, ":=");
    sscanf(str, "%s", prog.wds[6]);
    strcpy(str, "+");
    sscanf(str, "%s", prog.wds[7]);
    strcpy(str, "-");
    sscanf(str, "%s", prog.wds[8]);
    strcpy(str, "/");
    sscanf(str, "%s", prog.wds[9]);
    strcpy(str, "*");
    sscanf(str, "%s", prog.wds[10]);
    strcpy(str, "15");
    sscanf(str, "%s", prog.wds[11]);
    strcpy(str, ";");
    sscanf(str, "%s", prog.wds[12]);
    assert(Set(&prog));
    /*Test false*/
    strcpy(str, "S");
    sscanf(str, "%s", prog.wds[13]);
    strcpy(str, "L");
    sscanf(str, "%s", prog.wds[14]);
    strcpy(str, ":=");
    sscanf(str, "%s", prog.wds[15]);
    strcpy(str, ";");
    sscanf(str, "%s", prog.wds[16]);
    assert(!Set(&prog));

    /*Clean prog*/
    for (i = 0; i < 17; i++) {
        for (j = 0; j < 5; j++) {
            prog.wds[i][j] = 0;
        }
    }
    prog.cw = 0;

    /*Have to write Instrctlst, Instruction, Do*/
    /*Test Do*/
    strcpy(str, "DO");
    sscanf(str, "%s", prog.wds[0]);
    strcpy(str, "A");
    sscanf(str, "%s", prog.wds[1]);
    strcpy(str, "FROM");
    sscanf(str, "%s", prog.wds[2]);
    strcpy(str, "14");
    sscanf(str, "%s", prog.wds[3]);
    strcpy(str, "TO");
    sscanf(str, "%s", prog.wds[4]);
    strcpy(str, "1222");
    sscanf(str, "%s", prog.wds[5]);
    strcpy(str, "{");
    sscanf(str, "%s", prog.wds[6]);
    strcpy(str, "}");
    sscanf(str, "%s", prog.wds[7]);
    assert(Do(&prog));
    strcpy(str, "DO");
    sscanf(str, "%s", prog.wds[8]);
    strcpy(str, "b");
    sscanf(str, "%s", prog.wds[9]);
    strcpy(str, "FROM");
    sscanf(str, "%s", prog.wds[10]);
    strcpy(str, "-3");
    sscanf(str, "%s", prog.wds[11]);
    strcpy(str, "TO");
    sscanf(str, "%s", prog.wds[12]);
    strcpy(str, "1252");
    sscanf(str, "%s", prog.wds[13]);
    strcpy(str, "{");
    sscanf(str, "%s", prog.wds[14]);
    strcpy(str, "RT");
    sscanf(str, "%s", prog.wds[15]);
    strcpy(str, "10");
    sscanf(str, "%s", prog.wds[16]);
    strcpy(str, "}");
    assert(Do(&prog));

    /*Clean prog*/
    for (i = 0; i < 17; i++) {
        for (j = 0; j < 5; j++) {
            prog.wds[i][j] = 0;
        }
    }
    prog.cw = 0;
    
    /*Test Instruction*/
    strcpy(str, "DO");
    sscanf(str, "%s", prog.wds[0]);
    strcpy(str, "A");
    sscanf(str, "%s", prog.wds[1]);
    strcpy(str, "FROM");
    sscanf(str, "%s", prog.wds[2]);
    strcpy(str, "14");
    sscanf(str, "%s", prog.wds[3]);
    strcpy(str, "TO");
    sscanf(str, "%s", prog.wds[4]);
    strcpy(str, "1222");
    sscanf(str, "%s", prog.wds[5]);
    strcpy(str, "{");
    sscanf(str, "%s", prog.wds[6]);
    strcpy(str, "}");
    sscanf(str, "%s", prog.wds[7]);
    assert(Instruction(&prog));
    strcpy(str, "LT");
    sscanf(str, "%s", prog.wds[8]);
    strcpy(str, "16");
    sscanf(str, "%s", prog.wds[9]);
    assert(Instruction(&prog));
    strcpy(str, "LT");
    sscanf(str, "%s", prog.wds[10]);
    strcpy(str, "16");
    sscanf(str, "%s", prog.wds[11]);
    assert(Instruction(&prog));
    strcpy(str, "SET");
    sscanf(str, "%s", prog.wds[12]);
    strcpy(str, "m");
    sscanf(str, "%s", prog.wds[13]);
    strcpy(str, ":=");
    sscanf(str, "%s", prog.wds[14]);
    strcpy(str, ";");
    sscanf(str, "%s", prog.wds[15]);
    assert(Instruction(&prog));

    /*Clean prog*/
    for (i = 0; i < 17; i++) {
        for (j = 0; j < 5; j++) {
            prog.wds[i][j] = 0;
        }
    }
    prog.cw = 0;

    /*Test Instructlst*/
    strcpy(str, "}");
    sscanf(str, "%s", prog.wds[0]);
    assert(Instructlst(&prog));   
    strcpy(str, "LT");
    sscanf(str, "%s", prog.wds[1]);
    strcpy(str, "16");
    sscanf(str, "%s", prog.wds[2]);
    strcpy(str, "}");
    sscanf(str, "%s", prog.wds[3]);
    assert(Instructlst(&prog));   
    strcpy(str, "SET");
    sscanf(str, "%s", prog.wds[4]);
    strcpy(str, "L");
    sscanf(str, "%s", prog.wds[5]);
    strcpy(str, ":=");
    sscanf(str, "%s", prog.wds[6]);
    strcpy(str, ";");
    sscanf(str, "%s", prog.wds[7]);
    strcpy(str, "}");
    sscanf(str, "%s", prog.wds[8]);
    assert(Instructlst(&prog));


}
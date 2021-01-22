#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "neillsdl2.h"
#include "stack.h"
#include "Linked/specific.h"
#include "specific.h"

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
#define FULLCIRCLE 360
#define TORADIANS *(M_PI/180) 
#define ANGLE p->turtle.angle
#define X p->turtle.x
#define Y p->turtle.y

void test(void);

bool Prog(Program *p);
bool Instructlst(Program *p);
bool isnumber(Program *p);
bool Instruction(Program *p);
bool FD(Program *p);
bool LT(Program *p);
bool RT(Program *p);
bool isvar(Program *p);
bool varnum(Program *p);
bool OP(Program *p);
bool Polish(Program *p);
bool Set(Program *p);
bool Do(Program *p); 
void Draw(Program *p);
void rotate_left(Program *p);
void rotate_right(Program *p);
void addvar(Program *p);
int findval(Program *p, char *c);
int getval(Program *p);
int getstruct(Program *p, char *c);

int main(int argc, char** argv) {

    int i;
    FILE *fp;
    Program p;

    test();

    p.s = stack_init();
    /*To move into SDL screen*/
    p.turtle.x = 400;
    p.turtle.y = 300;

    if (argc != 2) {
        fprintf(stderr, "Wrong number of arguments\n");
        exit(2);
    }

    for (i = 0; i < 1000; i++) {
        p.wds[i][0] = '\0';
    }
    
    if (!(fp = fopen(argv[1], "r"))) {
        fprintf(stderr, "Cannot open %s\n", argv[1]);
        exit(2);
    }

    i = 0;
    while (fscanf(fp, "%s", p.wds[i++]) == 1 && i < 1000);

    /*Run Prog & SDL*/
    Neill_SDL_Init(&p.sw);
    Neill_SDL_SetDrawColour(&p.sw, 255, 255, 255);
    Prog(&p);
    Neill_SDL_UpdateScreen(&p.sw);

    while(!p.sw.finished) {
        Neill_SDL_Events(&p.sw);
    }

    /* Clear up graphics subsystems*/ 
    SDL_Quit();
    atexit(SDL_Quit);

    stack_free(p.s);

    return 0;
}

/*Start Program*/
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
    if (!Instructlst(p)) {
        return false;
    }
    return true;
}

/*Parse Instructionlst */
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

/*Check whether string is a number*/
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

/*Check for valid instruction */
bool Instruction(Program *p) {

    if (FD(p) || LT(p) || RT(p) || Do(p) || Set(p)) {
        return true;
    }
    return false;
}

/*Parse and move turtle forward if true*/
bool FD(Program *p) {

    if (strsame(STRING, "FD")) {
        NEXTSTRING
        if (!varnum(p)) {
            return false;
        }
        Draw(p);
        NEXTSTRING
        return true;
    }
    return false;
}

/*Parse and update turtle angle if true*/
bool LT(Program *p) {
    
    if (strsame(STRING, "LT")) {
        NEXTSTRING
        if (!varnum(p)) {
            return false;
        }
        rotate_left(p);
        NEXTSTRING
        return true;
    }
    return false;
}

/*Parse and update turtle angle if true*/
bool RT(Program *p) {

    if (strsame(STRING, "RT")) {
        NEXTSTRING
        if (!varnum(p)) {
            return false;
        }
        rotate_right(p);
        NEXTSTRING
        return true;
    }
    return false;
}

/*Chck whether string is valid variable*/
bool isvar(Program *p) {
   
   /*Cast to int pointer and dereference*/
   if (!isalpha(*(int*)STRING)) {
        return false;
    }
    addvar(p);
    return true;
}

/*Parse varnum*/
bool varnum(Program *p) {

    if (isnumber(p) || isvar(p)) {
        return true;
    }
    return false;
}

/*Pop values from stack, perform operation
and push value back onto stack*/
bool OP(Program *p) {

    stacktype d, g1, g2;

    stack_pop(p->s, &g2);
    stack_pop(p->s, &g1);

    /*Cast to int pointer and dereference*/
    switch(*(int*)STRING){
        case MINUS:
            d = g1 - g2;
            break;
        case PLUS:
            d = g1 + g2;
            break;
        case ASTERISK:
            d = g1 * g2;
            break;
        case SLASH:
            d = g1 / g2;
            break;
        default:
            return false; 
    }
    stack_push(p->s, d);
    return true;
}

bool Polish(Program *p) {

    if (strsame(STRING, ";")){
        NEXTSTRING
        return true;
    }
    /*Push number to stack*/
    if (varnum(p)){
        stack_push(p->s, getval(p));
        NEXTSTRING
        Polish(p);
        return true;
    }
    if (OP(p)) {
        NEXTSTRING
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

    stacktype d;
    int s;

    if (!strsame(STRING, "SET")){
        return false;
    }
    NEXTSTRING
    if(!isvar(p)) {
        return false;
    }
    s = getstruct(p, STRING);
    NEXTSTRING
    if (!strsame(STRING, ":=")) {
        return false;
    }
    NEXTSTRING
    if (!Polish(p)) {
        return false;
    }
    /*Pop off stack and store in var*/
    stack_pop(p->s, &d);
    p->v[s].val = d;

    return true;
}


bool Do(Program *p){

    int d, e, s;
    
    if (!strsame(STRING, "DO")) {
        return false;
    }
    NEXTSTRING
    if (!isvar(p)) {
        return false;
    }
    s = getstruct(p, STRING);
    NEXTSTRING
    if (!strsame(STRING, "FROM")) {
        return false;
    }
    NEXTSTRING
    if (!varnum(p)){
        return false;
    }
    /*Set loop start*/
    d = getval(p);
    p->v[s].val = d;
    NEXTSTRING
    if (!strsame(STRING, "TO")) {
        return false;
    }
    NEXTSTRING
    if (!varnum(p)){
        return false;
    }
    /*Set loop end*/
    e = getval(p);
    p->v[s].end = e;

    NEXTSTRING
    if (!strsame(STRING, "{")) {
        return false;
    }
    NEXTSTRING
    
    /*Loop back to cw until loop end*/
    p->v[s].word = p->cw;
    while(p->v[s].val != p->v[s].end) {
        Instructlst(p);
        p->cw = p->v[s].word;
        p->v[s].val = p->v[s].val + 1;
    }
    return true;
}


void Draw(Program *p) {

    double newx, newy;
    int distance = getval(p);

    /*Convert angle to radians to calculate new coords*/
    newx = X + (distance * cos((double)ANGLE TORADIANS));
    newy = Y + (distance * sin((double)ANGLE TORADIANS));

    /*Plot and draw*/
    SDL_RenderDrawLine(p->sw.renderer, X, Y, newx, newy);
    
    /*Update Coordinates*/
    X = newx;
    Y = newy;
}

/*Program to update the current angle*/
void rotate_left(Program *p) {

    int left_turn;

    /*Use to extract number from string*/
    left_turn = atoi(STRING);

    ANGLE = ANGLE + left_turn;

    if (ANGLE > FULLCIRCLE) {
        ANGLE = ANGLE - FULLCIRCLE;
    }
}

/*Program to update the current angle */
void rotate_right(Program *p) {

    int right_turn;

    /*Use to extract number from string*/
    right_turn = atoi(STRING);

    ANGLE = ANGLE - right_turn;

    if (ANGLE < 0) {
        ANGLE = ANGLE + FULLCIRCLE;
    }
}

/*Check struct for variable and if not there add*/
void addvar(Program *p) {

    int i = 0;

    /*Check whether var already in use*/
    while (p->v[i].o) {
        if (p->v[i].C == *(STRING)) {
            return;
        }
        i++;
    }
    p->v[i].C = *(STRING);
    p->v[i].o = true;
}

int findval(Program *p, char *c) {

    int i = 0;

    /*Find val attributed to var*/
    while (p->v[i].o) {
        if(p->v[i].C == *(c)) {
            return p->v[i].val;
        }
        i++;
    }
    /*Return 0 if false*/
    return 0;
}

/*Return string number or value attributed to string*/
int getval(Program *p) {

    int value = atoi(STRING);

    if (!value) {
        value = findval(p, STRING);
    }
    return value;
}


/*Set end of the loop*/
int getstruct(Program *p, char *c) {

    int i = 0;

    /*Search for variable*/
    while (p->v[i].o) {
        if (p->v[i].C == *(c)) {
            return i;
        }
        i++;
    }
    return 0;
}



void test(void){

    Program prog;
    char str[1000];
    int i, j;

    prog.s = stack_init();

    /*Separate tests designed for SDL file only*/

    /*Test rotate_right*/
    prog.turtle.angle = 90;
    strcpy(str, "24");
    sscanf(str, "%s", prog.wds[0]);
    rotate_right(&prog);
    assert(prog.turtle.angle == 66);
    strcpy(str, "89");
    sscanf(str, "%s", prog.wds[1]);
    prog.cw = prog.cw + 1;
    rotate_right(&prog);
    assert(prog.turtle.angle == 337);
    strcpy(str, "214");
    sscanf(str, "%s", prog.wds[2]);
    prog.cw = prog.cw + 1;
    rotate_right(&prog);
    assert(prog.turtle.angle == 123);
    strcpy(str, "21");
    sscanf(str, "%s", prog.wds[3]);
    prog.cw = prog.cw + 1;
    rotate_right(&prog);
    assert(prog.turtle.angle == 102);
    strcpy(str, "80");
    sscanf(str, "%s", prog.wds[4]);
    prog.cw = prog.cw + 1;
    rotate_right(&prog);
    assert(prog.turtle.angle == 22);

    /*Clean prog*/
    for (i = 0; i < 17; i++) {
        for (j = 0; j < 5; j++) {
            prog.wds[i][j] = 0;
        }
    }
    prog.cw = 0;

    /*Test rotate left*/
    prog.turtle.angle = 90;
    strcpy(str, "24");
    sscanf(str, "%s", prog.wds[0]);
    rotate_left(&prog);
    assert(prog.turtle.angle == 114);
    strcpy(str, "89");
    sscanf(str, "%s", prog.wds[1]);
    prog.cw = prog.cw + 1;
    rotate_left(&prog);
    assert(prog.turtle.angle == 203);
    strcpy(str, "214");
    sscanf(str, "%s", prog.wds[2]);
    prog.cw = prog.cw + 1;
    rotate_left(&prog);
    assert(prog.turtle.angle == 57);
    strcpy(str, "21");
    sscanf(str, "%s", prog.wds[3]);
    prog.cw = prog.cw + 1;
    rotate_left(&prog);
    assert(prog.turtle.angle == 78);
    strcpy(str, "80");
    sscanf(str, "%s", prog.wds[4]);
    prog.cw = prog.cw + 1;
    rotate_left(&prog);
    assert(prog.turtle.angle == 158);

    /*Clean prog*/
    for (i = 0; i < 17; i++) {
        for (j = 0; j < 5; j++) {
            prog.wds[i][j] = 0;
        }
    }
    prog.cw = 0;

    /*Test addvar*/
    strcpy(str, "A");
    sscanf(str, "%s", prog.wds[0]);
    addvar(&prog);
    assert(prog.v[0].C == 'A');
    strcpy(str, "B");
    sscanf(str, "%s", prog.wds[0]);
    addvar(&prog);
    assert(prog.v[1].C == 'B');
    strcpy(str, "C");
    sscanf(str, "%s", prog.wds[0]);
    addvar(&prog);
    assert(prog.v[2].C == 'C');
    strcpy(str, "D");
    sscanf(str, "%s", prog.wds[0]);
    addvar(&prog);
    assert(prog.v[3].C == 'D');
    strcpy(str, "E");
    sscanf(str, "%s", prog.wds[0]);
    addvar(&prog);
    assert(prog.v[4].C == 'E');
    /*Test stored registers*/
    strcpy(str, "A");
    sscanf(str, "%s", prog.wds[0]);
    addvar(&prog);
    assert(prog.v[5].C != 'A');
    strcpy(str, "B");
    sscanf(str, "%s", prog.wds[0]);
    addvar(&prog);
    assert(prog.v[5].C != 'B');

    /*Test findval & getstruct function*/
    prog.v[0].val = 36;
    strcpy(str, "A");
    sscanf(str, "%s", prog.wds[0]);
    assert(findval(&prog, prog.wds[prog.cw]) == 36);
    prog.v[1].val = 79;
    strcpy(str, "B");
    sscanf(str, "%s", prog.wds[0]);
    assert(findval(&prog, prog.wds[prog.cw]) == 79);
    prog.v[2].val = 102;
    strcpy(str, "C");
    sscanf(str, "%s", prog.wds[0]);
    assert(findval(&prog, prog.wds[prog.cw]) == 102);
    prog.v[3].val = 541;
    strcpy(str, "D");
    sscanf(str, "%s", prog.wds[0]);
    assert(findval(&prog, prog.wds[prog.cw]) == 541);
    /*Test false cases*/
    strcpy(str, "Z");
    sscanf(str, "%s", prog.wds[0]);
    assert(findval(&prog, prog.wds[prog.cw]) == 0);
    strcpy(str, "L");
    sscanf(str, "%s", prog.wds[0]);
    assert(findval(&prog, prog.wds[prog.cw]) == 0);

    /*Clean prog*/
    for (i = 0; i < 17; i++) {
        for (j = 0; j < 5; j++) {
            prog.wds[i][j] = 0;
            prog.v[i].val = 0;
            prog.v[i].o = false;
        }
    }
    prog.cw = 0;

    /*Test getval*/
    prog.v[0].val = 92;
    prog.v[0].C = 'L';
    prog.v[0].o = true;
    strcpy(str, "L");
    sscanf(str, "%s", prog.wds[0]);
    assert(getval(&prog) == 92);
    strcpy(str, "80");
    sscanf(str, "%s", prog.wds[0]);
    assert(getval(&prog) == 80);
    prog.v[1].val = 105;
    prog.v[1].C = 'G';
    prog.v[1].o = true;
    strcpy(str, "G");
    sscanf(str, "%s", prog.wds[0]);
    assert(getval(&prog) == 105);
    strcpy(str, "12");
    sscanf(str, "%s", prog.wds[0]);
    assert(getval(&prog) == 12);
    prog.v[2].val = 100;
    prog.v[2].C = 'A';
    prog.v[2].o = true;
    strcpy(str, "12");
    sscanf(str, "%s", prog.wds[0]);
    assert(getval(&prog) == 12);
    strcpy(str, "A");
    sscanf(str, "%s", prog.wds[0]);
    assert(getval(&prog) == 100);

    /*Clean prog*/
    for (i = 0; i < 17; i++) {
            prog.v[i].o = false;
        }
    prog.cw = 0;

    /*Test getstruct*/
    prog.v[0].o = true;
    prog.v[0].C = 'A';
    strcpy(str, "A");
    sscanf(str, "%s", prog.wds[0]);
    assert(getstruct(&prog, prog.wds[prog.cw]) == 0);
    prog.v[1].o = true;
    prog.v[1].C = 'L';
    strcpy(str, "L");
    sscanf(str, "%s", prog.wds[0]);
    assert(getstruct(&prog, prog.wds[prog.cw]) == 1);
    prog.v[2].o = true;
    prog.v[2].C = 'C';
    strcpy(str, "C");
    sscanf(str, "%s", prog.wds[0]);
    assert(getstruct(&prog, prog.wds[prog.cw]) == 2);
    strcpy(str, "A");
    sscanf(str, "%s", prog.wds[0]);
    assert(getstruct(&prog, prog.wds[prog.cw]) == 0);
    prog.v[3].o = true;
    prog.v[3].C = 'R';
    strcpy(str, "R");
    sscanf(str, "%s", prog.wds[0]);
    assert(getstruct(&prog, prog.wds[prog.cw]) == 3);
    strcpy(str, "C");
    sscanf(str, "%s", prog.wds[0]);
    assert(getstruct(&prog, prog.wds[prog.cw]) == 2);
    prog.v[4].o = true;
    prog.v[4].C = 'G';
    strcpy(str, "G");
    sscanf(str, "%s", prog.wds[0]);
    assert(getstruct(&prog, prog.wds[prog.cw]) == 4);


    stack_free(prog.s);

}



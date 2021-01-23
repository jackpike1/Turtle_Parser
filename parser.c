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

int main(int argc, char** argv) {

    int i;
    FILE *fp;
    Program p;
    
    p.cw = 0;

    test();

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

    Prog(&p);

    return 0;
}

/*Start program */
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

    if(!Instructlst(p)) {
        fprintf(stderr, "Error: Running program\n");
        exit(2);
    }
    
    return true;
}

/*Parse Instructlst*/
bool Instructlst(Program *p) {

    if (strsame(STRING, "}")) {
        NEXTSTRING
        return true;
    }
    if (!Instruction(p)) {
        return false;
    }
    if (!Instructlst(p)){
        return false;
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

/*Check whether string is a number, including
negative numbers and floating points*/
bool isnumber(Program *p) {

    int i = 0, len, count = 0;
    
    len = strlen(STRING);

    /*ASCII Codes '.' and '-' should parse*/
    while(i < len) {
        if (!isdigit(STRING[i]) && (STRING[i] != POINT\
         && STRING[i] != MINUS )) {
            return false;
        }
        /* '-' must be first character and
        '.' must not be first character*/
        if ((i > 0  && STRING[i] == MINUS) || \
         (i == 0 && STRING[i] == POINT)) {
            return false;
        }
        if (STRING[i] == POINT) {
            count += 1;
        }
        i++;
    }
    /*Floating point can only have one point*/
    if (count > 1) {
        return false;
    }
    return true;
}

/*Check whether string is a single uppercase alphabet*/
bool isvar(Program *p) {
   
   int len = strlen(STRING);

   if (len != 1) {
       return false;
   }

   /*Dereference pointer*/
   if (!isalpha(*STRING) || islower(*STRING)) {
        return false;
    }
    return true;
}

/*Parse varnum */
bool varnum(Program *p) {

    if (isnumber(p) || isvar(p)) {
        NEXTSTRING
        return true;
    }
    return false;
}

/*Parse FD*/
bool FD(Program *p) {

    if (strsame(STRING, "FD")) {
        NEXTSTRING
        if (!varnum(p)) {
            return false;
        }
        return true;
    }
    return false;
}

/*Parse LT*/
bool LT(Program *p) {

    if (strsame(STRING, "LT")) {
        NEXTSTRING
        if (!varnum(p)) {
            return false;
        }
        return true;
    }
    return false;
}

/*Parse RT*/
bool RT(Program *p) {

    if (strsame(STRING, "RT")) {
        NEXTSTRING
        if (!varnum(p)) {
            return false;
        }
        return true;
    }
    return false;
}

/*Parse OP*/
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

/*Parse Polish */
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

/*Parse Set*/
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

/*Parse Do*/
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



#include "specific.h"

int main(int argc, char** argv) {

    int i;
    FILE *fp;
    Program p;

    test();

    p.s = stack_init();
    /*To move into SDL screen*/
    p.turtle.x = WWIDTH/2;
    p.turtle.y = WHEIGHT/2;
    p.turtle.angle = 0;

    p.cw = 0;

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

    if (!Prog(&p)) {
        fprintf(stderr, "Error: invalid file %s \n", argv[1]);
        exit(2);
    }
    
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
        return false;
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
    if (!Instructlst(p)) {
        return false;
    }
    return true;
}

/*Check whether string is a positive number*/
bool isnumber(Program *p) {

    int i = 0, count = 0, len = strlen(STRING);

    while(i < len) {
        if (!isdigit(STRING[i]) && (STRING[i] != POINT)) {
            return false;
        }
        if (STRING[i] == POINT) {
            count += 1;
        }
        i++;
    }
    if (count > 1 || count == len) {
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

    /*Cast to double pointer and dereference*/
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

    int s;
    double d, e;
    
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
    if (!Doloop(p, s)){
        return false;
    }
    return true;
}

/*Execute do loop*/
bool Doloop(Program *p, int s) {

    /*Store cw and loop back until loop end*/
    p->v[s].word = p->cw;
    while((p->v[s].end + EPSILON) > p->v[s].val) {
        if (!Instructlst(p)) {
            return false;
        }
        p->v[s].val += 1;
        if ((p->v[s].end + EPSILON) > p->v[s].val) {
            p->cw = p->v[s].word;
        }
        else {
        }
    }
    return true;
}

/*Draw in SDL*/
void Draw(Program *p) {

    double newx, newy;
    double distance = getval(p);

    /*Convert angle to radians to calculate new coords*/
    newx = X + (distance * cos((double)ANGLE TORADIANS));
    newy = Y + ((int)distance * sin((double)ANGLE TORADIANS));

    SDL_RenderDrawLine(p->sw.renderer, X, Y, newx, newy);
    
    X = newx;
    Y = newy;
}

/*Update the current angle*/
void rotate_left(Program *p) {

    double left_turn;

    /*Use to extract float from string*/
    left_turn = atof(STRING);

    ANGLE = ANGLE + left_turn;

    while (ANGLE > FULLCIRCLE) {
        ANGLE = ANGLE - FULLCIRCLE;
    }
}

/*Update the current angle */
void rotate_right(Program *p) {

    double right_turn;

    /*Use to extract float from string*/
    right_turn = atof(STRING);

    ANGLE = ANGLE - right_turn;

    while (ANGLE < 0) {
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

double findval(Program *p, char *c) {

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
double getval(Program *p) {

    double value = atoi(STRING);

    if (value < EPSILON) {
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

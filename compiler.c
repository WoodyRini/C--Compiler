#include <stdio.h>
#include <ctype.h>
#include <string.h>
/* enumerate toktype */
typedef enum {FINISH,START,INID,INDIGIT,INEQ,INGT,INLT,COM3,COM2,COMDIV,INNEQ} StateType;

typedef enum {END,ELSE,IF,INT,RETURN,VOID,WHILE,PLUS, ERROR,MINUS, MULT, DIV, LT, LTE, GT, GTE, ASSIGN, EQ, NEQ, SEMI, COMMA, LP, RP, LB,RB, LC, RC,ID,DIGIT} TokType;

static struct {char* str; TokType tok; } reservedWords[256] =
{{"while",WHILE},{"else",ELSE},{"int",INT},{"if",IF},{"return",RETURN},{"void",VOID}};

#define TRUE 1
#define FALSE 0
int getToken(void);
static char toscan[256] = "";//this is buffer.
static int count = 0;// eq to linepos in their code
static int bufsize = 0;
static int lineno = 0;
static char tokenstring[40] = "";
char filename[50];
char listingname[50];
FILE *fp;
FILE *listing;
TokType token = GT;

static TokType reservedLookup(char * s){
	int i;
	for (i=0;i<6;i++)
		if (!strcmp(s,reservedWords[i].str))
			return reservedWords[i].tok;
	return ID;

}

static char getNextChar(void){
	if (!(count < bufsize))
	{ lineno++;
	if (fgets(toscan,255,fp))
	{ fprintf(listing,"%d: %s",lineno,toscan);
	bufsize = strlen(toscan);
	count = 0;
	return toscan[count++];}
	else return EOF;}
	else return toscan[count++];}

int main(void){
	printf("please enter a filename to be scanned\n");
	scanf("%s" , filename) ;
	printf("please enter your destination file\n");
	scanf("%s" , listingname) ;
	fp = fopen(filename,"r");
	listing = fopen(listingname,"w");
//	TokType token = GT;
	while (token != END){
//		printf("getting a token: count = %d\n",count);
		token = getToken();
		}
	fclose(fp);
	fclose(listing);
}

int getToken(void){

	int save;
//	printf("starting a getToken");
	int tokenstringindex = 0;// this is where we left off, now we need to figure how to work with an actual token string.
//	TokType token = GT;
	char current = toscan[count];
	StateType state = START;
	while (state != FINISH){
	char current = getNextChar();
//	printf("current is; %c\n tokindex is: %d\n",current, tokenstringindex);
	save = TRUE;
		switch(state){
			case START:
				if (isdigit(current)){
					state = INDIGIT;
					token = DIGIT;}
				else if (isalpha(current)){
					token = ID;
					state = INID;}
				else if ((current == ' ') || (current == '\t') || (current == '\n')){
					save = FALSE;
					}
				else if (current == '='){
					state = INEQ;}
				else if (current == '>'){
					state = INGT;}
				else if (current == '<'){
					state = INLT;}
				else if (current == '/'){
					state = COMDIV;
					save = FALSE;}
				else if (current == '!'){
					state = INNEQ;}
				else {
					state = FINISH;
				//	printf("here's a %c\n",current);
					switch(current){
					case '+':
						token = PLUS;
						break;	
					
					case '-':
						token = MINUS;
						break;	
					case '*':
						token = MULT;
						break;	
					case '[':
						token =	LB;
						break;	
					case ']':
						token = RB;
						break;	
					case '}':
						token = RC;
						break;	
					case '{':
						token = LC;
						break;
					case '(':
						token = LP;
						break;
					case ')':
						token = RP;
						break;
					case ';':
						token = SEMI;
						break;
					case ',':
						token = COMMA;
						break;
					case EOF:
						token = END;
						break;
					default:
						token = ERROR;
						break;
					}
				}
				break;
			case INID: //FIGURING this all out still
				if (!isalpha(current)){
					state = FINISH;
					count--;
					save = FALSE;
					}	
				break;
			case INDIGIT:
				if (!isdigit(current)){
					state = FINISH;
					count--;
					save = FALSE;
					}
				break;
			case INEQ:
				if (current != '='){
					
					token = ASSIGN;
					state = FINISH;
					count--;}
				else {
					state = FINISH;
					token = EQ;}
				break;
			case INGT:
				if (current != '='){
					
					token = GT;
					state = FINISH;
					count--;}
				else {
					state = FINISH;
					token = GTE;}
				break;
			case INLT:
				if (current != '='){
					
					token = LT;
					state = FINISH;
					count--;}
				else {
					state = FINISH;
					token = LTE;}
			
				break;
			case COMDIV:
				if (current != '*'){
					token = DIV;
					state = FINISH;
					count--;}
				else { 
					state = COM2; 
					save = FALSE;}
				break;
			case COM2:
				save = FALSE;
				if (current == '*'){
					state = COM3;}
				break;
			case COM3:
				save = FALSE;
				if (current == '/'){
					state = START;
					tokenstringindex = 0;}
				else if (current != '*')
					state = COM2;
				break;
			case INNEQ:
				if (current == '='){
					state = FINISH;
					token = NEQ;}
				break;

			
				
		}

		if (save) {
			 tokenstring[tokenstringindex++] = current;	
		}		
		if (state == FINISH) { 
			tokenstring[tokenstringindex] = '\0'; 
			if (token == ID)
				token = reservedLookup(tokenstring);
			}
//	++count;Don't think I need this because of getNextChar.
	}
	switch(token){
		case WHILE:
		case INT:
		case IF:
		case ELSE:
		case VOID:
		case RETURN:
			fprintf(listing,"\t%d:\t reserved word: %s\n",lineno,tokenstring);
			break;
		case ID:
			fprintf(listing,"\t%d:\t ID, name= %s\n",lineno,tokenstring);
			break;
		case DIGIT:
			fprintf(listing,"\t%d:\t Num, val= %s\n",lineno,tokenstring);
			break;
		case ASSIGN: 	
			fprintf(listing,"\t%d:\t =\n", lineno);
			break;
		case EQ:
			fprintf(listing,"\t%d:\t ==\n", lineno);
			break;
		case GT:
			fprintf(listing,"\t%d:\t >\n", lineno);
			break;
		case GTE:
			fprintf(listing,"\t%d:\t >=\n", lineno);
			break;	
		case LT:
			fprintf(listing,"\t%d:\t <\n", lineno);
			break;
		case LTE:
			fprintf(listing,"\t%d:\t <=\n", lineno);
			break;
		case NEQ:
			fprintf(listing,"\t%d:\t !=\n", lineno);
			break;	
		case RP:
			fprintf(listing,"\t%d:\t )\n", lineno);
			break;
		case LC: 
			fprintf(listing,"\t%d:\t {\n", lineno);
			break;
		case PLUS:
			fprintf(listing,"\t%d:\t +\n", lineno);
			break;
		case MINUS:
			fprintf(listing,"\t%d:\t -\n", lineno);
			break;	
		case MULT:
			fprintf(listing,"\t%d:\t *\n", lineno);
			break;
		case DIV:
			fprintf(listing,"\t%d:\t /\n", lineno);
			break;	
		case LB:
			fprintf(listing,"\t%d:\t [\n", lineno);
			break;	
		case RB:
			fprintf(listing,"\t%d:\t ]\n", lineno);
			break;	
		case RC:
			fprintf(listing,"\t%d:\t }\n", lineno);
			break;	
		case LP:
			fprintf(listing,"\t%d:\t (\n", lineno);
			break;
		case SEMI:
			fprintf(listing,"\t%d:\t ;\n", lineno);
			break;	
		case COMMA:
			fprintf(listing,"\t%d:\t ,\n", lineno);
			break;
		case ERROR:
			fprintf(listing,"\t%d:\t error: %s\n",lineno,tokenstring);
			break;
		case END: 
			fprintf(listing,"EOF");
		}

	return token; 

}

/*
	MATERIA : Compiladores y Lenguajes de Bajo Nivel
	Profesor: Julio Paciel
	TP      : Analizador Lexico (TP1)
	ALUMNOS : José de Jesús Cantero Maciel
*/

/*********** LIbrerias utilizadas **************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

/***************** MACROS **********************/

//Codigos
#define PROGRAM		256
#define TYPE		257
#define VAR			258
#define ARRAY		259
#define BEGIN		260
#define END			261
#define PR_DO		262
#define TO			263
#define DOWNTO		264
#define TERMINADOR_ENTER		265
#define OF			266
#define FUNCTION	267
#define PROCEDURE	268
#define PR_INTEGER	269
#define PR_REAL		270
#define PR_TRUE	    271
#define PR_FALSE		272
#define PR_FOR		273
#define PR_IF		274
#define PR_ELSE		275
#define PR_WHILE	276
#define REPEAT		277
#define UNTIL		278
#define PR_CASE		279
#define PR_ALERT		280
#define WRITELN		281
#define WRITE		282
#define DELIMITADOR_CODIGO		283
#define LITERAL_NUM			284
#define ID			285
#define BOOL		286
//#define TAB		286
#define CAR			287
#define LITERAL_CADENA		288
#define DOS_PUNTOS			289
#define COMA		290
#define L_LLAVE		291
#define R_LLAVE		292
//#define OP_ASIGNACION	293
#define L_CORCHETE	293
//#define USER_TYPE	294
#define R_CORCHETE	294
// Fin Codigos
#define TAMBUFF 	5
#define TAMLEX 		50
#define TAMHASH 	101

/************* Definiciones ********************/

typedef struct entrada{
	//definir los campos de 1 entrada de la tabla de simbolos
	int compLex;
	char lexema[TAMLEX];	
	struct entrada *tipoDato; // null puede representar variable no declarada	
	// aqui irian mas atributos para funciones y procedimientos...

} entrada;

typedef struct {
	char  compLex[TAMLEX];
	entrada *pe;
} token;

/************* Variables globales **************/

int consumir;			/* 1 indica al analizador lexico que debe devolver
						el sgte componente lexico, 0 debe devolver el actual */

char cad[5*TAMLEX];		// string utilizado para cargar mensajes de error
token t;				// token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico

FILE *archivo;			// Fuente pascal
FILE *pf; 
char buff[2*TAMBUFF];	// Buffer para lectura de archivo fuente
char id[TAMLEX];		// Utilizado por el analizador lexico
int delantero=-1;		// Utilizado por el analizador lexico
int fin=0;				// Utilizado por el analizador lexico
int numLinea=1;			// Numero de Linea

/************** Prototipos *********************/


void sigLex();		// Del analizador Lexico

/**************** Funciones **********************/

/*********************HASH************************/
entrada *tabla;				//declarar la tabla de simbolos
int tamTabla=TAMHASH;		//utilizado para cuando se debe hacer rehash
int elems=0;				//utilizado para cuando se debe hacer rehash

int h(const char* k, int m)
{
	unsigned h=0,g;
	int i;
	for (i=0;i<strlen(k);i++)
	{
		h=(h << 4) + k[i];
		if (g=h&0xf0000000){
			h=h^(g>>24);
			h=h^g;
		}
	}
	return h%m;
}
void insertar(entrada e);

void initTabla()
{	
	int i=0;

	tabla=(entrada*)malloc(tamTabla*sizeof(entrada));
	for(i=0;i<tamTabla;i++)
	{
		tabla[i].compLex=-1;
	}
}

int esprimo(int n)
{
	int i;
	for(i=3;i*i<=n;i+=2)
		if (n%i==0)
			return 0;
	return 1;
}

int siguiente_primo(int n)
{
	if (n%2==0)
		n++;
	for (;!esprimo(n);n+=2);

	return n;
}

//en caso de que la tabla llegue al limite, duplicar el tamaño
void rehash()
{
	entrada *vieja;
	int i;
	vieja=tabla;
	tamTabla=siguiente_primo(2*tamTabla);
	initTabla();
	for (i=0;i<tamTabla/2;i++)
	{
		if(vieja[i].compLex!=-1)
			insertar(vieja[i]);
	}		
	free(vieja);
}

//insertar una entrada en la tabla
void insertar(entrada e)
{
	int pos;
	if (++elems>=tamTabla/2)
		rehash();
	pos=h(e.lexema,tamTabla);
	while (tabla[pos].compLex!=-1)
	{
		pos++;
		if (pos==tamTabla)
			pos=0;
	}
	tabla[pos]=e;

}
//busca una clave en la tabla, si no existe devuelve NULL, posicion en caso contrario
entrada* buscar(const char *clave)
{
	int pos;
	entrada *e;
	pos=h(clave,tamTabla);
	while(tabla[pos].compLex!=-1 && strcmp(tabla[pos].lexema,clave)!=0 )
	{
		pos++;
		if (pos==tamTabla)
			pos=0;
	}
	return &tabla[pos];
}

void insertTablaSimbolos(const char *s, int n)
{
	entrada e;
	sprintf(e.lexema,s);
	e.compLex=n;
	insertar(e);
}

void initTablaSimbolos()
{
	int i;
	entrada pr,*e;
	const char *vector[]={
		"program",
		"type",
		"var",
		"array",
		"begin",
		"end",
		"do",
		"to",
		"downto",
		"then",
		"of",
		"function",
		"procedure", 
		"integer", 
		"real", 
		"boolean", 
		"char", 
		"for", 
		"if", 
		"else", 
		"while", 
		"repeat", 
		"until", 
		"case", 
		"record", 
		"writeln",
		"write",
		"const"
	};
 	for (i=0;i<28;i++)
	{
		insertTablaSimbolos(vector[i],i+256);
	}
	//insertTablaSimbolos(",",',');
	insertTablaSimbolos(".",'.');
	//insertTablaSimbolos(":",':');
	insertTablaSimbolos(";",';');
	insertTablaSimbolos("(",'(');
	insertTablaSimbolos(")",')');
	//insertTablaSimbolos("[",'[');
	//insertTablaSimbolos("]",']');
	insertTablaSimbolos("?",'?');
	insertTablaSimbolos("true",PR_TRUE);
	insertTablaSimbolos("false",PR_FALSE);
	///insertTablaSimbolos("not",NOT);
	insertTablaSimbolos(":",DOS_PUNTOS);
	insertTablaSimbolos(",",COMA);
	//insertTablaSimbolos("<",OP_RELACIONAL);
	//insertTablaSimbolos("<=",OP_RELACIONAL);
	//insertTablaSimbolos("<>",OP_RELACIONAL);
	//insertTablaSimbolos(">",OP_RELACIONAL);
	//insertTablaSimbolos(">=",OP_RELACIONAL);
	//insertTablaSimbolos("=",OP_ASIGNACION);
	//insertTablaSimbolos("+",OP_SUMA);
	insertTablaSimbolos("{",L_LLAVE);
	//insertTablaSimbolos("-",OP_SUMA);
	//insertTablaSimbolos("or",OP_SUMA);
	//insertTablaSimbolos("*",OP_MUL);
	insertTablaSimbolos("}",R_LLAVE);
	insertTablaSimbolos("[",L_CORCHETE);
	insertTablaSimbolos("]",R_CORCHETE);
	//insertTablaSimbolos("/",OP_MUL);
	//insertTablaSimbolos("div",OP_MUL);
	//insertTablaSimbolos("mod",OP_MUL);
	///insertTablaSimbolos(":=",OP_ASIGNACION);
}

// Rutinas del analizador lexico

void error(const char* mensaje)
{
	printf("Lin %d: Error Lexico. %s.\n",numLinea,mensaje);	
}

void sigLex()
{
	int i=0, longid=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];
	entrada e;

	while((c=fgetc(archivo))!=EOF)
	{

		if (c==' ' || c=='\t'){
			continue;	//eliminar espacios en blanco
			sprintf(t.compLex,"\t");
			//incrementar el numero de linea
			//numLinea++;
			break;
		}
		/*else if(c=='#'){
			if(c=fgetc(archivo) == '#'){
                 //numLinea++;
                 
            }
            sprintf(t.compLex,"COMMENT");
             while(c=fgetc(archivo) != '\n'){continue;}
             if (c!=EOF)
			    ungetc(c,archivo);
             break;
        }*/
		else if(c=='\n')
		{
             sprintf(t.compLex,"TERMINADOR_ENTER");
			//incrementar el numero de linea
			numLinea++;
			break;
		}
		else if (isalpha(c))
		{
			//es un identificador (o palabra reservada)
			i=0;
			do{
				id[i]=c;
				i++;
				c=fgetc(archivo);
				if (i>=TAMLEX)
					error("Longitud de Identificador excede tamaño de buffer");
			}while(isalpha(c) || isdigit(c));
			id[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			
					if(id[0]=='i' && id[1]=='f' && id[2]=='\0'){
						
						sprintf(t.compLex,"PR_IF");
					}
					else if(id[0]=='a' && id[1]=='l' && id[2]=='e' && id[3]=='r' && id[4]=='t' && id[5]=='\0'){
						sprintf(e.lexema,id);
						e.compLex=PR_ALERT;
						insertar(e);
						t.pe=buscar(id);
						sprintf(t.compLex,"PR_ALERT");
					}
					else{
						sprintf(e.lexema,id);
						e.compLex=ID;
						insertar(e);
						t.pe=buscar(id);
						sprintf(t.compLex,"ID");
					}
			//}
			break;
		}
		else if (isdigit(c))
		{
				//es un numero
				i=0;
				estado=0;
				acepto=0;
				id[i]=c;

				while(!acepto)
				{
					switch(estado){
					case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=0;
						}
						else if(c=='.'){
							id[++i]=c;
							estado=1;
						}
						else if(tolower(c)=='e'){
							id[++i]=c;
							estado=3;
						}
						else{
							estado=6;
						}
						break;

					case 1://un punto, debe seguir un digito (caso especial de array, puede venir otro punto)
						c=fgetc(archivo);						
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(c=='.')
						{
							i--;
							fseek(archivo,-1,SEEK_CUR);
							estado=6;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(tolower(c)=='e')
						{
							id[++i]=c;
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						c=fgetc(archivo);
						if (c=='+' || c=='-')
						{
							id[++i]=c;
							estado=4;
						}
						else if(isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=6;
						}break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
							ungetc(c,archivo);
						else
							c=0;
						id[++i]='\0';
						acepto=1;
						
						sprintf(t.compLex,"LITERAL_NUM");
						break;
					case -1:
						if (c==EOF)
							error("No se esperaba el fin de archivo");
						else
							error(msg);
						exit(1);
					}
				}
			break;
		}
		else if (c=='<') 
		{
			//es un operador relacional, averiguar cual
			c=fgetc(archivo);
			if (c=='>'){
				sprintf(t.compLex,"OP_RELACIONAL");
				t.pe=buscar("<>");
			}
			else if (c=='='){
				sprintf(t.compLex,"OP_RELACIONAL");
				t.pe=buscar("<=");
			}
			else{
				ungetc(c,archivo);
				sprintf(t.compLex,"OP_RELACIONAL");
				t.pe=buscar("<");
			}
			break;
		}
		else if (c=='>')
		{
			//es un operador relacional, averiguar cual
				c=fgetc(archivo);
			if (c=='='){
				sprintf(t.compLex,"OP_RELACIONAL");
				t.pe=buscar(">=");
			}
			else{
				ungetc(c,archivo);
				sprintf(t.compLex,"OP_RELACIONAL");
				t.pe=buscar(">");
			}
			break;
		}
		else if (c==':')
		{
			//puede ser un : o un operador de asignacion
			c=fgetc(archivo);
			if (c=='='){
				sprintf(t.compLex,"OP_ASIGNACION");
				t.pe=buscar(":=");
			}
			else{
				ungetc(c,archivo);
				sprintf(t.compLex,"DOS_PUNTOS");
				t.pe=buscar(":");
			}
			break;
		}
		else if (c=='?'){
			sprintf(t.compLex,"OP_CONDICION");
			t.pe=buscar("?");
			break;
		}
		else if (c=='+')
		{
			sprintf(t.compLex,"OP_SUMA");
			t.pe=buscar("+");
			break;
		}
		else if (c=='-')
		{
			c=fgetc(archivo);
			//printf("%c",c);
			if(c!='>'){
				
				sprintf(t.compLex,"OP_SUMA");
				t.pe=buscar("-");
				
				ungetc(c,archivo);
				break;
			}
			else{
				sprintf(e.lexema,"->");
				
				sprintf(t.compLex,"DELIMITADOR_CODIGO");
				break;
			}
			
			//break;
		}
		else if (c=='*')
		{
			sprintf(t.compLex,"OP_MUL");
			t.pe=buscar("*");
			break;
		}
		else if (c=='/')
		{
			sprintf(t.compLex,"OP_MUL");
			t.pe=buscar("/");
			break;
		}
		else if (c=='=')
		{
			sprintf(t.compLex,"OP_ASIGNACION");
			t.pe=buscar("=");
			break;
		}
		else if (c==',')
		{
			sprintf(t.compLex,"COMA");
			t.pe=buscar(",");
			break;
		}
		else if (c==';')
		{
			sprintf(t.compLex,"TERMINADOR_PUNTOCOMA");
			t.pe=buscar(";");
			break;
		}
		else if (c=='.')
		{
			sprintf(t.compLex,"PUNTO");
			t.pe=buscar(".");
			break;
		}
		else if (c=='(')
		{
			if ((c=fgetc(archivo))=='*')
			{//es un comentario
				while(c!=EOF)
				{
					c=fgetc(archivo);
					if (c=='*')
					{
						if ((c=fgetc(archivo))==')')
						{
							break;
						}
						ungetc(c,archivo);
					}
				}
				if (c==EOF)
					error("Se llego al fin de archivo sin finalizar un comentario");
				continue;
			}
			else
			{
				ungetc(c,archivo);
				sprintf(t.compLex,"L_PARENTESIS");
				t.pe=buscar("(");
			}
			break;
		}
		else if (c==')')
		{
			sprintf(t.compLex,"R_PARENTESIS");
			t.pe=buscar(")");
			break;
		}
		else if (c=='[')
		{
			sprintf(t.compLex,"L_CORCHETE");
			t.pe=buscar("[");
			break;
		}
		else if (c==']')
		{
			sprintf(t.compLex,"R_CORCHETE");
			t.pe=buscar("]");
			break;
		}
		else if (c=='\"')
		{//un caracter o una cadena de caracteres
			i=0;
			id[i]=c;
			i++;
			do{
				c=fgetc(archivo);
				if (c=='\"')
				{
					c=fgetc(archivo);
					if (c=='\"')
					{
						id[i]=c;
						i++;
						id[i]=c;
						i++;
					}
					else
					{
						id[i]='\"';
						i++;
						break;
					}
				}
				else if(c==EOF)
				{
					error("Se llego al fin de archivo sin finalizar un literal");
				}
				else{
					id[i]=c;
					i++;
				}
			}while(isascii(c));
			id[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			t.pe=buscar(id);
			//t.compLex=t.pe->compLex;
			if (t.pe->compLex==-1)
			{
				sprintf(e.lexema,id);
				if (strlen(id)==3 || strcmp(id,"''''")==0){
					e.compLex=CAR;
					sprintf(t.compLex,"CAR");}
				else{
					e.compLex=LITERAL_CADENA;
					sprintf(t.compLex,"LITERAL_CADENA");}
				insertar(e);
				t.pe=buscar(id);
				//t.compLex=e.compLex;
			}
			break;
		}
		else if (c=='#')
		{
			//elimina el comentario
			sprintf(t.compLex,"COMMENT");
			while(c!=EOF)
			{
				c=fgetc(archivo);
				if (c=='\n'){
					ungetc(c,archivo);
					break;
                }
					
			}
			if (c==EOF)
				error("Se llego al fin de archivo sin finalizar un comentario");
			break;
		}
		else if (c!=EOF)
		{
			sprintf(msg,"%c no esperado",c);
			error(msg);
		}
	}
	if (c==EOF)
	{
		sprintf(t.compLex,"EOF");
		sprintf(e.lexema,"EOF");
		t.pe=&e;
	}

}

int main(int argc,char* args[])
{
	// inicializar analizador lexico
	int complex=0;
    pf = fopen("output.txt","w"); 
    char guardar[100];
	initTabla();
	initTablaSimbolos();



	if(argc > 1)
	{
		
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo no encontrado.\n");
			exit(1);
		}
		int bnumlinea = 1;
		int endofF = 1;
		while (strcmp(t.compLex,"EOF")!=0 && endofF !=0){
         
			sigLex();
			if(strcmp(t.compLex,"TERMINADOR_ENTER")!=0 && bnumlinea == numLinea && strcmp(t.compLex,"EOF")!=0){
			   
			     strcat(guardar, t.compLex);
				 if(strcmp(t.compLex,"\t")!=0){
					strcat(guardar," ");
			     }

            }
            else{
                 //strcat(guardar, t.compLex);
                 printf("%s",guardar);
                 fprintf(pf,guardar);
                
                 bnumlinea = numLinea;
                 
                 
                 sprintf(guardar, "\n");
                 
            }
		}
		fclose(archivo);
		fclose(pf);
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}

	return 0;
	
	
}

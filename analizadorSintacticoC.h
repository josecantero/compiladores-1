/* analizador sintactico */

void comparar(int);
void listaDeSentencias();
void sentencia();
void sentenciaDeAsignacion();
void expresion();
void termino();
void expresionSimple();
void factor();
void parse();
void expresionLogica();
void esUnArray();
void sentencia_if ();
void sentencia_for();
void sentencia_declaracion();
void sentencia_write();



void sentencia_for(){
     comparar(PR_FOR);
     sentenciaDeAsignacion();
     comparar(TO);
     factor();     
     comparar(STEP);
     factor();
     comparar(PR_DO);
     listaDeSentencias();
     comparar(END);
     comparar(PR_FOR);
}


void comparar(int tokenEsperado){
    if (t.compLex==tokenEsperado){
        sigLex();   
    }
    else{
        error(" no se esperaba el token ");
    }
}

void sentencia_write(){
     comparar(t.compLex);
     comparar('(');
     if(t.compLex==LITERAL){
        comparar(LITERAL);                                           
     }
     else{
        factor();
     }
     comparar(')');
}

void termino(){
     factor();
     while(t.compLex==OP_MULT){
       comparar(t.compLex);
       factor();                  
     }
}

void sentencia_declaracion(){
     comparar(VAR);
     comparar(ID);
     esUnArray();         
     while(t.compLex==','){
        comparar(',');
        comparar(ID);
        esUnArray();                        
     }
} 

void expresionLogica(){
     expresion();
     if(t.compLex==OP_LOGIC){
       comparar(OP_LOGIC);
       expresion();        
     }                                            
}

void expresion(){
     expresionSimple();
    if (t.compLex==OP_REL){
       comparar(t.compLex);
       expresionSimple();       
    }   
}
 
void esUnArray(){
    if (t.compLex=='['){
       comparar('[');
       expresionSimple();
       comparar(']');            
    }    
}

void listaDeSentencias(){
    sentencia();
    while ((t.compLex!=EOF) && 
           (t.compLex!=END) && 
           (t.compLex!=PR_ELSE))
    {        
        comparar(';');
        if((t.compLex!=EOF) && 
           (t.compLex!=END) && 
           (t.compLex!=PR_ELSE)){
               sentencia();                   
        } 
    }
}

void sentencia(){
    switch(t.compLex){    
        case ID : sentenciaDeAsignacion();
           break;   
                    
        case VAR: sentencia_declaracion();
           break;
           
        case ST_WRITE: sentencia_write();
           break;
           
        case ST_WRITELN: sentencia_write();
           break;
           
        case PR_IF: sentencia_if();
           break;
           
        case PR_FOR: sentencia_for();
           break;
    
    default: error(" no se esperaba ");
      sigLex();    
      break;
    }    
}


void sentencia_if(){
     comparar(PR_IF);
     expresionLogica();
     comparar(THEN);
     listaDeSentencias();
     if(t.compLex==PR_ELSE){
        comparar(PR_ELSE);
        listaDeSentencias();           
     }
     comparar(END);
     comparar(PR_IF);
}



void factor(){
     switch(t.compLex){
        case NUM:
             comparar(NUM);
             break;
        case ID:
             comparar(ID);
             esUnArray();
             break;
        case '(':
             comparar('(');
             expresion();     
             comparar(')'); 
             break;
        default :
           error(" no se esperaba ");   
           sigLex();   
           break;                                        
      }     
 }

void expresionSimple(){
    termino();
    
    while(t.compLex==OP_SUMA){
        comparar(t.compLex);
        termino();          
    } 
}

void parse(){
      sigLex();        
      listaDeSentencias();
}


void sentenciaDeAsignacion(){
    if(t.compLex==ID){
        comparar(ID);
        esUnArray();
        comparar(OP_ASIGNA);
        expresion();        
    }
    
}

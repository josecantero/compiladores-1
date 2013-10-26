
#include "analizadorLexicoC.h"
#include "analizadorSintacticoC.h"


int main(int argc,char* args[])
{
	// inicializar analizador lexico
	int complex=0;

	initTabla();
	initTablaSimbolos();
	
	
  //CODIGO COMPILADO EN WINDOWS Y FUNCIONA ============================= 
   /*if (!(archivo=fopen("fuente.txt", "r")))
		 {
			  printf("Archivo no encontrado.");
			  //exit(1);
		 }//Funcion parse
		 else{
            parse();
            if(HayError==FALSE){
               printf("\nNo fue encontrado error alguno\n");                                  
            }
            fclose(archivo);
            system("pause");
           	return 0;
      }
    ///=================================================================*/

   if(argc > 1)
	 {
	    if (!(archivo=fopen(args[1],"rt")))
		    //if (!(archivo=fopen("fuente.txt", "r")))
		  {
			   printf("Archivo no encontrado.");
			   exit(1);
		  }            
		  //else{
      parse();
      if(HayError==FALSE){
         printf("\nNo fue encontrado error alguno\n");                                  
      }
      fclose(archivo);
      system("pause");
     	return 0;
      //}
    }else{
		  printf("Debe pasar como parametro el path al archivo fuente.");
		  exit(1);
	  }
	  system("pause");
}

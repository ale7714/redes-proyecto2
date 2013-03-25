/* 
 * Universidad Simón Bolivar
 * CI4835 - Redes de Computadoras
 * Proyecto 1 
 * 
 * Archivo:     encomun.c
 * Autor:       Alexandra Paredes 05-38680
 *              Javier Rodriguez  05-38838
 * Descripción: Funciones que usan en común el programa cliente y el servidor
 * 
 */

#include "encomun.h"

char *programname;

/*  Función: fatalerror
    Entrada:
          message: Apuntador al string en memoria que contiene el mensaje de 
                   error
        errortype: Entero que representa el tipo de error
    Salida:
        N/A 
    Función que muestra el mensaje de error, message, y aborta el programa con
    el error, errortype
*/
void fatalerror(char *message, int errortype) {
  fprintf(stderr, "%s: %s\n", programname, message);
  fflush(stdout);
  exit(errortype);
}


/*  Función: trim
    Entrada:
        cadena: Apuntador al string a modificar
    Salida:
        cadena: Apuntador al string modifcado
    Función que elimina, de existir, los espacios al inicio y fin del string
*/
char* trim(char *cadena){

    int i;

    // Estando en el inicio del string, mientras sea espacio se saltan
    while(isspace(*cadena)){
        cadena++;   
    }

    // Para eliminar los espacios al final
    for (i = strlen(cadena)- 1; (isspace(cadena[i])); i--) ; 

    cadena[i + 1] = '\0';

    return cadena;
    
}


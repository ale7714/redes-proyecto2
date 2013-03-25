/* 
 * Universidad Simón Bolivar
 * CI4835 - Redes de Computadoras
 * Proyecto 2
 * 
 * Archivo:     gestion_centro.h
 * Autor:       Alexandra Paredes 05-38680
 *              Javier Rodriguez  05-38838
 * Descripción: Variables globales para la gestion del centro
 * 
 */

#include "encomun.h"

char usoCentro[720]="\nUso: \t$./centro -n nombreCentro -cp capacidadMaxima \n\
    \t-i inventario -t tiempo -s suministro\n\
    Donde:\n\
    \t1) nombreCentro: es una cadena de caracteres sin espacio que \
    indica el nobre del centro de distribucion.\n\
    \t2) capacidadMaxima: entero entre 38.000 y 3.800.000 que indica la capacidad\
    maxima.\n\
    \t3) inventario: entero entre 0 y capacidadMaxima que indica el \
    inventario actual de la bomba.\n\
    \t4) tiempo: entero entre 0 y 180 que indica el tiempo de respuesta.\n\
    \t5) suministro: entero entre 0 y 10.000 que representa el suministro promedio en\
    litros por minuto.\n";


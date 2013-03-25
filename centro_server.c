/* 
 * Universidad Simón Bolivar
 * CI4835 - Redes de Computadoras
 * Proyecto 2
 * 
 * Archivo:     centro_server.c
 * Autor:       Alexandra Paredes 05-38680
 *              Javier Rodriguez  05-38838
 * Descripción: Programa que implemente las funciones del centro de distriucion (servidor)
 * 
 */

/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

//#include "gestion_centro.h"
#include "centro.h"
#include <time.h>

char entry[256];
pthread_mutex_t mute1 = PTHREAD_MUTEX_INITIALIZER;

///////////////////////////////////// INICIO FUNCIONES MD5 ////////////////////////////////////////////

// Constants are the integer part of the sines of integers (in radians) * 2^32.
const uint32_t k[64] = {
0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee ,
0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501 ,
0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be ,
0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 ,
0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa ,
0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8 ,
0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed ,
0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a ,
0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c ,
0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70 ,
0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05 ,
0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 ,
0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039 ,
0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1 ,
0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1 ,
0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

// leftrotate function definition
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

// These vars will contain the hash
uint32_t h0, h1, h2, h3;


void md5(uint8_t *initial_msg, size_t initial_len) {

    // Message (to prepare)
    uint8_t *msg = NULL;

    // Note: All variables are unsigned 32 bit and wrap modulo 2^32 when calculating

    // r specifies the per-round shift amounts
    const uint32_t r[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                          5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
                          4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                          6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

    // Initialize variables - simple count in nibbles:
    h0 = 0x67452301;
    h1 = 0xefcdab89;
    h2 = 0x98badcfe;
    h3 = 0x10325476;

    // Pre-processing: adding a single 1 bit
    //append "1" bit to message
    /* Notice: the input bytes are considered as bits strings,
       where the first bit is the most significant bit of the byte.[37] */

    // Pre-processing: padding with zeros
    //append "0" bit until message length in bit ≡ 448 (mod 512)
    //append length mod (2 pow 64) to message

    int new_len;
    for(new_len = initial_len*8 + 1; new_len%512!=448; new_len++);
    new_len /= 8;

    msg = calloc(new_len + 64, 1); // also appends "0" bits
                                   // (we alloc also 64 extra bytes...)
    memcpy(msg, initial_msg, initial_len);
    msg[initial_len] = 128; // write the "1" bit

    uint32_t bits_len = 8*initial_len; // note, we append the len
    memcpy(msg + new_len, &bits_len, 4);           // in bits at the end of the buffer

    // Process the message in successive 512-bit chunks:
    //for each 512-bit chunk of message:
    int offset;
    for(offset=0; offset<new_len; offset += (512/8)) {

        // break chunk into sixteen 32-bit words w[j], 0 ≤ j ≤ 15
        uint32_t *w = (uint32_t *) (msg + offset);

#ifdef DEBUG
        printf("offset: %d %x\n", offset, offset);

        int j;
        for(j =0; j < 64; j++) printf("%x ", ((uint8_t *) w)[j]);
        puts("");
#endif

        // Initialize hash value for this chunk:
        uint32_t a = h0;
        uint32_t b = h1;
        uint32_t c = h2;
        uint32_t d = h3;

        // Main loop:
        uint32_t i;
        for(i = 0; i<64; i++) {

            uint32_t f, g;

             if (i < 16) {
                f = (b & c) | ((~b) & d);
                g = i;
            } else if (i < 32) {
                f = (d & b) | ((~d) & c);
                g = (5*i + 1) % 16;
            } else if (i < 48) {
                f = b ^ c ^ d;
                g = (3*i + 5) % 16;
            } else {
                f = c ^ (b | (~d));
                g = (7*i) % 16;
            }

             uint32_t temp = d;
            d = c;
            c = b;
            b = b + LEFTROTATE((a + f + k[i] + w[g]), r[i]);
            a = temp;

        }

        // Add this chunk's hash to result so far:
        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;

    }

    // cleanup
    free(msg);

}

char* MD5Generator(char* challenge)
{
		char *msg = malloc(strlen(challenge));
		strcpy(msg, challenge);
     	size_t len = strlen(msg);
		// benchmark
		int i;
		for (i = 0; i < 1000000; i++) {
		    md5(msg, len);
		}

		//var char digest[16] := h0 append h1 append h2 append h3 //(Output is in little-endian)
		uint8_t *p;

		// display result
		char * md5 = malloc(sizeof(p));
		char * final = malloc(4 * sizeof(p));

		p=(uint8_t *)&h0;
		snprintf(md5, sizeof(p) + 1, "%02x%02x%02x%02x", p[0], p[1], p[2], p[3], h0);
		strcpy(final, md5);
		p=(uint8_t *)&h1;
		snprintf(md5, sizeof(p) + 1, "%02x%02x%02x%02x", p[0], p[1], p[2], p[3], h1);
		strcat(final, md5);
		p=(uint8_t *)&h2;
		snprintf(md5, sizeof(p) + 1, "%02x%02x%02x%02x", p[0], p[1], p[2], p[3], h2);
		strcat(final, md5);
		p=(uint8_t *)&h3;
		snprintf(md5, sizeof(p) + 1, "%02x%02x%02x%02x", p[0], p[1], p[2], p[3], h3);
		strcat(final, md5);

		free(md5);

		return final;
}
///////////////////////////////////// FIN FUNCIONES MD5 ////////////////////////////////////////////

////////////////////////// INICIO FUNCIONES MANEJO DE LA LISTA ////////////////////////////////////

/*  Función: inicializar
    Entrada:
        nombre: Apuntador al nombre de la bomba
       desafio: Apuntador al desafio que se le asigno a la bomba
         lista: Apuntador a la dirección en memoria de la lista
    Salida:
        N/A 
    Función que inicializa la lista que almacena tipo bomba
*/
void inicializar(char* nombre, char* desafio, bomba* lista){

    strcpy(lista->nombre,nombre);
    int i;
  	for (i = 0; i < 16; i++){
        lista->desafio[i] = desafio[i];
    }
	lista->desafio[16] = '\0';
    lista->tiempoVencimiento = 0; 
    lista->siguiente = NULL; 

}

/*  Función: insertar
    Entrada:
            lista: Apuntador a la dirección en memoria de la lista
           bNueva: Apuntador a la dirección en memoria de la bomba a insertar en la lista
    Salida:
        N/A 
    Función que inserta a la lista del tipo bomba, la bomba indicada en bNueva
*/
void insertar(bomba* lista, bomba* bNueva){
    bomba *temp = lista;
    if(strcmp(temp->nombre,"NIL")!=0){
        while(temp->siguiente != NULL){
            temp = temp->siguiente;
        }
        bomba *ele =  (bomba *) malloc(sizeof(bomba));
        inicializar(bNueva->nombre,bNueva->desafio,ele);
        temp->siguiente = ele;
    }else{
        inicializar(bNueva->nombre,bNueva->desafio,lista);
    }
}

/*  Función: imprimir
    Entrada:
        lista: Apuntador a la dirección en memoria de la lista
    Salida:
        N/A 
    Función que imprime la lista que almacena tipo bomba
*/
void imprimir(bomba* lista){
    bomba *aux = lista;
    printf("Lista de Bombas:\n");
    while(aux != NULL){   
        printf("Nombre bomba: %s\n",aux->nombre);
        printf("Desafio: %s\n",aux->desafio);   
        printf("Tiempo Vencimiento: %d\n",aux->tiempoVencimiento); 
        printf("\n"); 
        aux = aux->siguiente; 
    }
}


///////////////////////////// FIN FUNCIONES MANEJO DE LA LISTA ////////////////////////////////////


/*  Función: random_string
    Entrada:
        lista: Apuntador a la direccion en memoria donde se almacenara el string 
        lenght: Entero que indica la longitud del string
    Salida:
        N/A 
    Función que genera un string random 
*/
void random_string(char * string, int length){

  int i;
  int randNum;

  srand(time(NULL));

  for (i = 0; i < length; ++i){

	  randNum = 26 * (rand() / (RAND_MAX +1.0));
	  randNum = randNum + 97;
      string[i] = (char) randNum;
  }


  string[length] = '\0';

}

///////////////////////////////// INICIO FUNCIONES DE RPC ///////////////////////////////////////

/*  Función: solicitar_desafio_1_svc
    Entrada:
        argp: apuntador a la estructura de tipo datos 
        rqstp: apuntador a la estructura de tipo svc_req
    Salida:
        result: apuntador a la direccion que apunta a una cadena de caracteres
    Función que retorna el desafio a la bomba indicada en argp->nombre
*/
char **solicitar_desafio_1_svc(datos *argp, struct svc_req *rqstp){

	static char * result;
	//char *nombreBomba;
        //strcpy(nombreBomba,argp->nombre);
	//sprintf(nombreBomba,"%s",argp->nombre); 
	char desafio[14];
	int i;
	int existe = 0;

	random_string(desafio, 14);
	result = malloc(strlen(desafio));
	for (i = 0; i < strlen(desafio) -1; i++){
		result[i] = desafio[i];
	}
	result[14] = '\00';
    	
    bomba *aux = listaBombas;

	char * md5hash = MD5Generator(result);

	while(aux != NULL){   
	      if(strcmp(aux->nombre,argp->nombre)==0){
	      	for (i = 0; i < 16; i++){
		        aux->desafio[i] = md5hash[i];
	        }
			aux->desafio[16] = '\0';
			existe = 1;
			break;
	      } 
	      aux = aux->siguiente;
	}

	if(existe == 0){
		bomba *ele =  (bomba *) malloc(sizeof(bomba));
        inicializar(argp->nombre,md5hash,ele);
		insertar(listaBombas,ele);
	}
	//printf("nombre de la bomba solicitante %s\n",nombreBomba);  
	//printf("desafio %s\n",result);
        //imprimir(listaBombas);

	return &result;
}

/*  Función: solicitar_desafio_1_svc
    Entrada:
        argp: apuntador a la estructura de tipo datos 
        rqstp: apuntador a la estructura de tipo svc_req
    Salida:
        result: apuntador a la direccion que apunta a una cadena de caracteres
    Función que retorna el ticket a la bomba indicada en argp->nombre
*/
char **respodnder_desafio_1_svc(datos *argp, struct svc_req *rqstp){
	static char * result;
	char *nombreBomba = argp->nombre; 
    char *respuesta = argp->respuesta;
    bomba *aux = listaBombas;
    char *ticket = (char *) malloc(strlen(nombreBomba)+strlen(nombreServidor)+5);
    int existe = 0;
	//imprimir(listaBombas); 
    while(aux != NULL){  
	//printf("nombre de la bomba solicitante %s, almacenada %s\n",nombreBomba,aux->nombre);  
	      if(strcmp(aux->nombre,nombreBomba)==0){
		//printf("almacenada %s recibida %s\n",aux->desafio,respuesta);
	      	if(strcmp(aux->desafio,respuesta)==0){
				sprintf(ticket,"%s##%s##%d\00",nombreBomba,nombreServidor,tiempoEjecucionServidor);
				aux->tiempoVencimiento = tiempoEjecucionServidor + 45;
	      	}else{
    			strcpy(ticket,"invalido");
	      	}
	      	existe = 1;
			break;
	      } 
	      aux = aux->siguiente;
	}
	if(existe == 0){
    	strcpy(ticket,"invalido");
	}
        imprimir(listaBombas); 
	result = malloc(strlen(ticket)); 
        strcpy(result,ticket);
	return &result;
}

/*  Función: solicitar_tiempo_1_svc
    Entrada:
        argp: apuntador a la cadena de caracteres
        rqstp: apuntador a la estructura de tipo svc_req
    Salida:
        result: entero
    Función que retorna el tiempo de respuesto del centro
*/
int *solicitar_tiempo_1_svc(char **argp, struct svc_req *rqstp){
	static int  result;
	result = tiempoRespuestaServidor;
	//printf("Solicitud tiempo respuesta: %d\n",tiempoRespuestaServidor);

	return &result;
}

/*  Función: olicitar_gasolina_1_svc
    Entrada:
        argp: apuntador a la cadena de caracteres, que representa el ticket
        rqstp: apuntador a la estructura de tipo svc_req
    Salida:
        result: entero
    Función que retorna un entero según la posibilidad de enviar gasolina
*/
int *solicitar_gasolina_1_svc(char **argp, struct svc_req *rqstp){

	static int  result;
	char *datos = NULL;
    char nombreBomba[256];
    char nombreCentro[256];

     
	datos = strtok (*argp,"##");
	strcpy(nombreBomba, trim(datos));
	datos = strtok (NULL, "##");
    strcpy(nombreCentro,trim(datos));

    bomba *aux = listaBombas;
    int existe = 0;
    if(strcmp(nombreServidor,nombreCentro)==0){
	    while(aux != NULL){   
		      if(strcmp(aux->nombre,nombreBomba)==0){
		      	if(aux->tiempoVencimiento>=tiempoEjecucionServidor){
	      		    pthread_mutex_lock(&mute1);
				    if (inventarioServidor >= 38000) {        
				      inventarioServidor = inventarioServidor - 38000;
				      printf("Solicitud gasolina: %d, exitosa\n",tiempoEjecucionServidor);
				      result = 1;
				      // Escribir el resultado de la peticion en log
				      // RECORDAR TOMAR LOS 
				      sprintf(entry, "Suministro: %d, %s, enviado, inventario: %d\n", tiempoEjecucionServidor, nombreBomba,inventarioServidor);
				      fprintf(logServer,"%s",entry);
				      fflush(logServer);
				    }else{
				      result = 2;
				      printf("Solicitud gasolina: %d, sin inventario\n",tiempoEjecucionServidor);
                      sprintf(entry, "Suministro: %d, %s, sin inventario, inventario: %d\n", tiempoEjecucionServidor, nombreBomba,inventarioServidor);
                      fprintf(logServer,"%s",entry);
                      fflush(logServer);
				   }
				   pthread_mutex_unlock(&mute1);
		      	}else{
		      		result = -1;
                    printf("Solicitud gasolina: %d, negada: ticket vencido\n",tiempoEjecucionServidor);
                    sprintf(entry, "Suministro: %d, %s, negada: ticket vencido, inventario: %d\n", tiempoEjecucionServidor, nombreBomba,inventarioServidor);
                    fprintf(logServer,"%s",entry);
                    fflush(logServer);
		      	}
		      	existe = 1;
				break;
		      } 
		      aux = aux->siguiente;
		}
		if(existe == 0){
	    	result = -1;
            printf("Solicitud gasolina: %d, negada: ticket invalido\n",tiempoEjecucionServidor);
            sprintf(entry, "Suministro: %d, %s, negada: ticket invalido, inventario: %d\n", tiempoEjecucionServidor, nombreBomba,inventarioServidor);
            fprintf(logServer,"%s",entry);
            fflush(logServer);
		}
	}else{
		result = -1;
        printf("Solicitud gasolina: %d, negada: ticket invalido\n",tiempoEjecucionServidor);
        sprintf(entry, "Suministro: %d, %s, negada: ticket invalido, inventario: %d\n", tiempoEjecucionServidor, nombreBomba,inventarioServidor);
        fprintf(logServer,"%s",entry);
        fflush(logServer);
	}

   return &result;
}
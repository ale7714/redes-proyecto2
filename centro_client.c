/* 
 * Universidad Simón Bolivar
 * CI4835 - Redes de Computadoras
 * Proyecto 2
 * 
 * Archivo:     centro_client.c
 * Autor:       Alexandra Paredes 05-38680
 *              Javier Rodriguez  05-38838
 * Descripción: Programa que implemente las funciones de la bomba (cliente)
 * 
 */

/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "centro.h"


/*
 * VARIABLES GLOBALES
 */

pthread_mutex_t mute = PTHREAD_MUTEX_INITIALIZER;

// Direccion IP del cliente
char addressBuffer[INET_ADDRSTRLEN];

// Mensaje de uso
char usoBomba[720]="\nUso: \t$./bomba -n nombreBomba -cp capacidadMaxima \n\
    \t-i inventario -c consumo -fc ficheroCentros\n\
    Donde:\n\
    \t1) nombreBomba: es una cadena de caracteres sin espacio que \
    indica el nobre de la bomba.\n\
    \t2) capacidadMaxima: entero entre 38.000 y 380.000 que indica la capacidad\
    maxima.\n\
    \t3) inventario: entero entre 0 y capacidadMaxima que indica el \
    inventario actual de la bomba.\n\
    \t4) consumo: entero entre 0 y 1.000 que indica el consumo promedio en\
    litros por minutos.\n\
    \t5) ficheroCentros: nombre del fichero que contiene el listado de los\
    centros de distribucion.\n";

// Datos de la bomba 
char *ficheroCentros = NULL;
char *nombreBomba = NULL;
char dns[128];
char nombreCentro[128];
int inventario;
int consumo;
int capacidadMaxima;
int tiempoEjecucion;
int tiempoRespuestaMin;
int inicioSesion = 0;


// Para el log
FILE *logClient;
char entryLog[256];
int solicitando;


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
        char *msg = (char *) malloc(strlen(challenge));
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

// Para la lista de servidores (centros de distribucion)
typedef struct servidor{
    char nombre[128];    
    char dns[128];    
    int tiempoRespuesta;
    struct servidor *siguiente;
} servidor;

/*  Función: crear
    Entrada:
        lista: Apuntador a la dirección en memoria de la lista
    Salida:
        N/A 
    Función que crea la lista que almacena tipo servidor
*/
void crear(servidor* lista){
    strcpy(lista->nombre,"NIL");
    strcpy(lista->dns,"NIL"); 
    lista->tiempoRespuesta = 0; 
    lista->siguiente = NULL; 
}

/*  Función: inicializar
    Entrada:
        nombre: Apuntador al nombre del servidor 
           dns: Apuntador al dns del servidor
        puerto: Entero que representa el puerto
         lista: Apuntador a la dirección en memoria de la lista
    Salida:
        N/A 
    Función que inicializa la lista que almacena tipo servidor
*/
void inicializar(char* nombre, char* dns, servidor* lista){
    strcpy(lista->nombre,nombre);
    strcpy(lista->dns,dns); 
    lista->tiempoRespuesta = 0; 
    lista->siguiente = NULL;   
}


/*  Función: insertar
    Entrada:
            lista: Apuntador a la dirección en memoria de la lista
        servNuevo: Apuntador a la dirección en memoria del servidor a insertar en la lista
    Salida:
        N/A 
    Función que inserta a la lista del tipo servidor, el servidor indicado en servNuevo
*/
void insertar(servidor* lista, servidor* servNuevo){
    servidor *temp = lista;
    if(strcmp(temp->nombre,"NIL")!=0){
        while(temp->siguiente != NULL){
            temp = temp->siguiente;
        }
        servidor *ele =  (servidor *) malloc(sizeof(servidor));
        inicializar(servNuevo->nombre,servNuevo->dns,ele);
        temp->siguiente = ele;
    }else{
        inicializar(servNuevo->nombre,servNuevo->dns,lista);
    }
}

/*  Función: imprimir
    Entrada:
        lista: Apuntador a la dirección en memoria de la lista
    Salida:
        N/A 
    Función que imprime la lista que almacena tipo servidor
*/
void imprimir(servidor* lista){
    servidor *aux = lista;
    printf("Lista de Centros de Servicio:\n");
    while(aux != NULL){   
        printf("Nombre centro: %s\n",aux->nombre);
        printf("DNS: %s\n",aux->dns);   
        printf("Tiempo de Respuesta: %d\n",aux->tiempoRespuesta); 
        printf("\n"); 
        aux = aux->siguiente; 
    }
}

servidor *listaCentros = NULL;
///////////////////////////// FIN FUNCIONES MANEJO DE LA LISTA ////////////////////////////////////

int *tiempoServidor = NULL;
int *solicitud = NULL;
char **desafio = NULL;
char **ticket = NULL;
char digest[17];
char *nombreIP = NULL;

////////////////////////////////// INICIO FUNCIONES BOMBAS ////////////////////////////////////////


/*  Función: centro_prog_1
    Entrada:
        host: Apuntador a la cadena de caracteres que contiene la direccion del host
        option: Entero que reoresenta la función de RPC a llamar
    Salida:
        N/A 
    Función que realizar las funciones de RPC del lado del cliente.
        1: Solicitar desafio
        2: Responder desafio
        3: Solicitar tiempo
        4: Solicitar gasolina
*/
void centro_prog_1(char *host,int option){

    CLIENT *clnt;
    datos  solicitar_desafio_1_arg;
    datos  respodnder_desafio_1_arg;
    char * solicitar_tiempo_1_arg;
    char * solicitar_gasolina_1_arg;

    //char * nombre = (char *) malloc(strlen(nombreBomba)+strlen(addressBuffer));
    //sprintf(nombre,"%s&&%s",nombreBomba,addressBuffer);
    //int i;

#ifndef DEBUG
    clnt = clnt_create (host, CENTRO_PROG, CENTRO_VERS, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror (host);
        usleep(5*100000);
    } else {
#endif  /* DEBUG */

        if(option == 1){
            //printf("Estoy solicitando desafio %s\n",nombreIP);
            solicitar_desafio_1_arg.nombre = nombreIP;
            solicitar_desafio_1_arg.respuesta = nombreIP;
            desafio = solicitar_desafio_1(&solicitar_desafio_1_arg, clnt);
            if (desafio == (char **) NULL) {
                clnt_perror (clnt, "Solicitud de desafio fallido");
            }
        } else if (option == 2){
            //printf("Estoy enviando respuesta %s\n",digest);
            respodnder_desafio_1_arg.nombre = nombreIP;
            respodnder_desafio_1_arg.respuesta = digest;
            ticket = respodnder_desafio_1(&respodnder_desafio_1_arg, clnt);
            if (ticket == (char **) NULL) {
                clnt_perror (clnt, "Envio de respuesta de desafio fallido");
            }
        } else if (option == 3){
            //printf("Estoy solicitando tiempo\n");
            solicitar_tiempo_1_arg = "ticket";
            tiempoServidor = solicitar_tiempo_1(&solicitar_tiempo_1_arg, clnt);
            if (tiempoServidor == (int *) NULL) {
                clnt_perror (clnt, "Solicitud de tiempo de respuesta fallido");
            }
        } else if (option == 4)
        {
            //printf("Estoy solicitando gasolina\n");
            solicitar_gasolina_1_arg = *ticket;
            solicitud = solicitar_gasolina_1(&solicitar_gasolina_1_arg, clnt);
            if (solicitud == (int *) NULL) {
                clnt_perror (clnt, "Solicitud de gasolina fallida");
            }
        }

#ifndef DEBUG
    clnt_destroy (clnt);
    //free(nombre);
#endif   /* DEBUG */
   }
}


/*  Función: verificarArgs  
    Entrada:
        cantArg: Cantidad de parametros introducidos.
           args: Arreglo con los parametros introducidos.
    Salida:
        N/A 
    Función que permite verificar los parametros de entrada al llamar
    el programa. En caso de diferencias con el formato de 
    entrada se muestra el mensaje de uso del progama
*/
void verificarArgs(int cantArg,char*args[]){
    int opt;

    struct option longopts[] = {
       { "cp", required_argument, NULL, 'm'},
       { "fc", required_argument, NULL, 'f'},
       { 0, 0, 0, 0}
    };

    if(cantArg == 11){
         int long_index =0;
        /* Ciclo que toma los arguemtos de cada uno de los flags sin importar el orden en
           el cual son colocado los flags al invocar el programa */
        while ((opt = getopt_long_only(cantArg, args, "n:i:c:m:f:",longopts,&long_index)) != -1){
            switch (opt) {
                case 'n':
                    // Nombre de la bomba
                    if (nombreBomba != NULL) 
                        free(nombreBomba);
                    if (asprintf(&nombreBomba, "%s", optarg) == -1) {
                            fatalerror("Error copiando string para manejo de argumentos",
                                EXIT_BAD_ARG);
                    }
                    //printf("Nombre bomba:%s\n",nombreBomba);
                    break;

                case 'm':
                    // Capacidad Máxima
                    capacidadMaxima = atoi(optarg);
                    //printf("Capacidad:%d\n",capacidadMaxima);
                    break;

                case 'i':
                    // Inventario
                    inventario = atoi(optarg);
                    //printf("Inventario:%d\n",inventario);
                    break;

                case 'c':
                    // Consumo
                    consumo = atoi(optarg);
                    //printf("Consumo:%d\n",consumo);
                    break;

                case 'f':
                    // Fichero de los Centros de distribución
                    if (ficheroCentros != NULL) 
                        free(ficheroCentros);
                    if (asprintf(&ficheroCentros, "%s", optarg) == -1) {
                            fatalerror("Error copiando string para manejo de argumentos",
                                EXIT_BAD_ARG);
                    }
                    //printf("Fichero Centros: %s\n",ficheroCentros);
                    break;


                default:
                       fatalerror(usoBomba,EXIT_BAD_ARG);
            }

        }

        if(capacidadMaxima < 38000 || capacidadMaxima > 380000){
            fatalerror(usoBomba,EXIT_BAD_ARG);
        }
        if(inventario < 0 || inventario > capacidadMaxima){
            fatalerror(usoBomba,EXIT_BAD_ARG);
        }
        if(consumo < 0 || consumo > 1000){
            fatalerror(usoBomba,EXIT_BAD_ARG);
        }
    }else{
        fatalerror(usoBomba,EXIT_BAD_ARG);
    }
}

/*  Función: crearListaCentros
    Entrada:
        lista: Apuntador a la dirección en memoria de la lista de centros de distribución
     nFichero: Nombre del fichero que contiene los datos de los  centro
    Salida:
        N/A 
    Función que crea la lista de los centros distribución
*/
void crearListaCentros(servidor* lista, char* nFichero){
    char contenido[256];
    char* datos= NULL;
    char nombre[52];
    char dns[52];
    int puerto;
    servidor *aux = (servidor *) malloc(sizeof(servidor));

    FILE *file = fopen(nFichero, "rt");

    if(file==NULL){
        fatalerror("Error al abrir el fichero",EXIT_OPEN_FILE);
    } 

    while (fgets(contenido, 256, file) != NULL) {
        //printf("contenido %s\n",contenido);    
        datos = strtok (contenido,"&");
        //printf("datos %s\n",datos);
        strcpy(nombre, trim(datos));
        //printf("Lectura centros\n");
        //printf("Nombre centro: %s\n",nombre);
        datos = strtok (NULL, "&");
        strcpy(dns,trim(datos));
        //printf("DNS: %s\n",dns);
        crear(aux);
        inicializar(nombre,dns,aux);
        insertar(lista,aux);
        //imprimir(lista);
        //printf("ciclo\n");    
    }
    
    fclose(file); 
}

/*  Función: obtenerServidorTiempoMinimo
    Entrada:
        lista: apuntador a la lista de servidores
    Salida:
        servidorTiempoMinimo: apuntador al servidor con el tiempo minimo de respuesta
    Función que retorna el servidor con el tiempo minimo de respuesta
*/
servidor *obtenerServidorTiempoMinimo(servidor* lista){

    servidor *ptr = lista;  
    int tiempoMinimo = 50000;     
    servidor *servidorTiempoMinimo = NULL;    

    while (1) {
        
        //printf("\nConectando a: %s ",ptr->nombre);
        //printf("\n");
        centro_prog_1(ptr->dns,3);
        if ( tiempoServidor != NULL && *tiempoServidor < tiempoMinimo) {  

            tiempoMinimo = *tiempoServidor;
            ptr->tiempoRespuesta = *tiempoServidor;
            servidorTiempoMinimo = ptr;
            //printf("\nCentro con menor tiempo es %s, tiempo de respuesta: %d\n", servidorTiempoMinimo->nombre,servidorTiempoMinimo->tiempoRespuesta);
        }
    
        if (ptr->siguiente == NULL){
            //printf("Se termino la lista!\n");
            break;
        } else {
            ptr = ptr->siguiente;
        }

    }

    return servidorTiempoMinimo;
}


/*  Función: manejadorCliente
    Entrada:
        lista: es un apuntador a la dirección en memoria a la lista de serivodres
    Salida:
        N/A
    Función que se encarga de mandar el mensaje de solicitud al servidor y 
    si la respuesta es afirmativa, se aumenta el inventario de gasolina. 
*/
void* manejadorCliente(void *lista){

    servidor *aux = (servidor *) malloc(sizeof(servidor));
    inicializar(((servidor *)lista)->nombre,((servidor *)lista)->dns,aux);
    aux->siguiente = ((servidor *)lista)->siguiente;
    servidor *minimo = NULL; 
    
    while(aux != NULL) {
        if(inicioSesion == 0){
            minimo = obtenerServidorTiempoMinimo(aux);
            //imprimir(minimo); 
            if(minimo != NULL){
                strcpy(dns,minimo->dns);
                strcpy(nombreCentro,minimo->nombre);
                tiempoRespuestaMin = minimo->tiempoRespuesta;
                int intentos = 0;
                while(intentos < 3){
                    intentos++;
                    centro_prog_1(minimo->dns,1);
                    //printf("desafio %s\n",*desafio);
                    if(desafio != NULL){
                        char * md5hash = MD5Generator(*desafio);
                        int i;
                        for (i = 0; i < 16; i++){
                          digest[i] = md5hash[i];
                        }
                        digest[16] = '\0';
                        centro_prog_1(minimo->dns,2);
                        desafio = NULL;
                        if(ticket!= NULL){
			    //printf(" entro tickect\n");
                            //printf("ticket %s\n",*ticket);
                            sprintf(entryLog, "Ticket asignado: %s, por centro: %s, en %d.\n", *ticket, nombreCentro, tiempoEjecucion);
                            fprintf(logClient,"%s",entryLog);
                            fflush(logClient);    
                            printf("A %d, se asigno %s\n", tiempoEjecucion,*ticket);
                            if(strcmp(*ticket,"invalido")!=0){
                                inicioSesion = 1;
                                break;
                            }  
                        }

                    }
                }
            }else{  
                //printf("no consegui minimo\n");
                aux = aux->siguiente;
            }
        } else{
            centro_prog_1(dns,4);
            if(solicitud != NULL){
                if(*solicitud == 1){
                    sprintf(entryLog, "Peticion: %d, %s, exitosa\n", tiempoEjecucion, nombreCentro);
                    fprintf(logClient,"%s",entryLog);
                    fflush(logClient);  

                    ///////////////////////// INICIO ZONA CRITICA CONCURRENCIA /////////////////////////
                    pthread_mutex_lock(&mute);
                    //printf("inventario before: %d\n",inventario);
                    if( (inventario = inventario  + 38000) > capacidadMaxima){
                        inventario = capacidadMaxima;
                    } 
                    //printf("inventario after: %d\n",inventario);      
                    pthread_mutex_unlock(&mute);  
                    /////////////////////////// FIN ZONA CRITICA CONCURRENCIA //////////////////////////
                    usleep(tiempoRespuestaMin*100000);
                    sprintf(entryLog, "Llegada de la gandola: %d, %d\n", tiempoEjecucion, inventario);
                    fprintf(logClient,"%s",entryLog);
                    fflush(logClient);  
                    printf("Recibio gasolina de %s a %d\n",nombreCentro,tiempoEjecucion);
                    break;
                } else if(*solicitud == 2){
                    sprintf(entryLog, "Peticion: %d, %s, fallida\n", tiempoEjecucion, nombreCentro);
                    fprintf(logClient,"%s",entryLog);
                    fflush(logClient);    
                    printf("A %d, %s no tiene disponibilidad\n", tiempoEjecucion, nombreCentro);
                    inicioSesion = 0;
                } else if(*solicitud == -1){
                    sprintf(entryLog, "Peticion: %d, %s, negada.\n", tiempoEjecucion, nombreCentro);
                    fprintf(logClient,"%s",entryLog);
                    fflush(logClient);    
                    printf("A %d, %s nego el acceso\n", tiempoEjecucion,nombreCentro);
                    ticket = NULL;
                    inicioSesion = 0;          
                }
                solicitud = NULL; 
            }else {
                printf("Error al realizar solicitud al centro de distribucion\n");
            }           
        }
        minimo = NULL;
        solicitud = NULL;
        //printf("El siguiente\n");
    }

    solicitando = FALSE;
    //exit(EXIT_SUCCESS);    
}

int main (int argc, char *argv[])
{
	
	struct ifaddrs * ifAddrStruct=NULL;
	struct ifaddrs * ifa=NULL;
	void * tmpAddrPtr=NULL;

	getifaddrs(&ifAddrStruct);

	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
    	if (ifa->ifa_addr->sa_family==AF_INET) { // check it is IP4
    	   // is a valid IP4 Address
    	   tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
           char address[INET_ADDRSTRLEN];
    	   inet_ntop(AF_INET, tmpAddrPtr, address, INET_ADDRSTRLEN);
    	   
    	   if (strcmp(ifa->ifa_name,"lo") != 0) {
    	       //printf("%s IP de la bomba %s\n", ifa->ifa_name, address); 
               strcpy(addressBuffer, address);
    	   }
    	} 
	}

    int tError;

    programname = argv[0]; // Guardar nombre del programa para errores

    verificarArgs(argc,argv); 

    // Crear y abrir archivo en modo de escritura
    char log_name[128];
    sprintf(log_name, "log_%s.txt", nombreBomba);
    logClient = fopen(log_name, "w");
    fprintf(logClient, "\0");
    fclose(logClient);
    logClient = fopen(log_name, "a+");

    // Escribir estado inicial del centro en el log
    sprintf(entryLog, "Estado incial: %d\n", inventario);
    fprintf(logClient,"%s",entryLog);
    fflush(logClient);

    tiempoEjecucion = 0;
    solicitando = FALSE;
    servidor *listaCentros = (servidor *) malloc(sizeof(servidor));
    crear(listaCentros);
    crearListaCentros(listaCentros,ficheroCentros);
    //imprimir(listaCentros);

    nombreIP = (char *) malloc(strlen(nombreBomba)+strlen(addressBuffer));
    sprintf(nombreIP,"%s&&%s",nombreBomba,addressBuffer);

    while(tiempoEjecucion<=480){


        if(inventario == capacidadMaxima){
            sprintf(entryLog, "Tanque full: %d\n", tiempoEjecucion);
            fprintf(logClient,"%s",entryLog);
            fflush(logClient);
        }

        if(inventario == 0){
            sprintf(entryLog, "Tanque vacio: %d\n", tiempoEjecucion);
            fprintf(logClient,"%s",entryLog);
            fflush(logClient);
        }

        if( ((capacidadMaxima - inventario) >= 38000) && (solicitando == FALSE)){
            pthread_t threadSolicitud = (pthread_t *) malloc(sizeof (pthread_t));
            solicitando = TRUE;
            if((tError=pthread_create(&threadSolicitud,NULL,manejadorCliente,(void *)listaCentros)) < 0)
                fatalerror("Error al crear el thread",tError);
            solicitando = TRUE;
        }
        //imprimir(listaCentros);
        tiempoEjecucion++;
        if( (inventario = inventario  - consumo) < 0){
            inventario = 0;
        }
        usleep(100*1000);
    }

    if (fclose(logClient) == 0){

        printf("Termino actividad del cliente.\n");

    }
    
    exit(EXIT_SUCCESS);

    /*	char *host;

    	if (argc < 2) {
    		printf ("usage: %s server_host\n", argv[0]);
    		exit (1);
    	}
    	host = argv[1];
    	centro_prog_1 (host);
        exit (0);*/
}

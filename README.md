______________________________________________________________________________
|                                                                             |
|Sartenejas, 18 de marzo de 2013.  		                            	          |
|Proyecto # 2 de Redes (12%).						                                      |
|	GRUPO # 2										                                                |
|	Integrantes: 	Alexandra Paredes #05-38680.	                                |
|					Javier Rodríguez #05-38838.		                                      |
|-----------------------------------------------------------------------------|				
|													                                                    | 
|	* Archivos que componen el proyecto2.tar.gz:	                              |
|       - centro.x: Define el programa y los servicios hacer generados por    |
|            el RPC.	                                                        |
|		- centro.h: Contiene las firmas y declaraciones de las estructuras        |  
|           utilizadas en el RPC tanto para el cliente como para el servidor  |
|       - centro_client.c: es el programa cliente, que se encarga de la       |
|           gestión del estado del mismo verificando el inventario. Cuando es |
|           necesario se encarga de realizar las solicitud de gasolina a los  |
|           centros de distribución indicados. Este programa consta de varias |
|           operaciones: verificarargs (verifica los argumentos de entrada,   |
|           los flags sin importar su orden, es decir, que sean validos),     |
|           crearListaCentros (función que crea la lista de los centros       |
|           distribución a partir de un fichero), centro_prog_1               |
|           (llama a la función apropiada de rpc de acuerdo a la solicitud).  |
|       - centro_clnt.c: programa que genera rpcgen que se encarga de         |
|           realizar y enviar las peticiones al servidor.                     | 
|       - centro_server.c: Posee las funciones que dan los servicios del      |
|           servidor. solicitar_desafio, responder_desafio, solicitar_tiempo  |
|       - centro_svc.c: Se encarga de la gestión del servidor, inventario,    |
|           suministro, atender solicitudes. Posee operaciones similares al   |
|           cliente para verificar los argumentos y verficación del estado    |
|           del servidor.                                                     |
|       - encomun.h: Contiene las declaraciones y firmas de las funciones     |
|       - encomun.c: Contiene todas las funciones que se usan, tanto en el    |
|           servidor como en el cliente.                                      |
|       - gestioncentro.h: Contiene las variables globales para la gestión    |
|           del centro.                                                       |
|       - centro_xdr.c: Archivo generado por rpcgen.                          |
|-----------------------------------------------------------------------------|                                                                             |                                                                             |
|   * Funcionalidad del Programa:                                             |
|        - Cliente: Lee el fichero de centros de distribución, construye la   |
|            lista de centros de distribución, gestiona el cliclo de la bomba,|
|            verifica el estado del inventario y realiza las solicitudes al   |
|            servidor cuando sea necesario.                                   |
|        - Servidor: Gestiona el ciclo de funcionamiento del servidor,        |
|            recibe, maneja y responde las solicitudes del cliente.           |
|                                                                             |
|-----------------------------------------------------------------------------|                                                                            |                                                                             |
|   * Observaciones:                                                          | 
|        - Si los argumentos de entrada del programa no son los correctos se  |
|          muestra el mensaje de uso.                                         |
|        - El fichero donde se incluyen los centros de distribución sigue el  |
|          formato del enunciado del Proyecto 1. El archivo no puede tener    |
|          lineas en blanco al final del archivo.                             |
|        - El nombre de la bomba no solo puede contener caracteres            |
|          alfabeticos, es decir no puede contener numeros o simbolos.        |
|        - El algoritmo utilizado para realizar el cifrado se extrajo del     |
|          siguiente enlace:                                                  |
|                                                                             |
|_____________________________________________________________________________|
          
    

                    

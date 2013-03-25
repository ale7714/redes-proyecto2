/* 
 * Universidad Simón Bolivar
 * CI4835 - Redes de Computadoras
 * Proyecto 2
 * 
 * Archivo:     centro.x
 * Autor:       Alexandra Paredes 05-38680
 *              Javier Rodriguez  05-38838
 * Descripción: Programa con los servicios del servidor 
 * 
 */


const MAXLEN = 512;		
typedef string str<MAXLEN>;

	
struct datos {
	str nombre;
	str respuesta; 	
};

program CENTRO_PROG {
        version CENTRO_VERS {
            string SOLICITAR_DESAFIO(datos) = 1;
            string RESPODNDER_DESAFIO(datos) = 2;
            int SOLICITAR_TIEMPO(string) = 3;
            int SOLICITAR_GASOLINA(string) = 4;
        } = 1;
} = 0x23868011;

#include "api_comunicacion_intercargo.h"

#include <string>
using namespace std;
#include "api_constants.h"

ApiComunicacionIntercargo::ApiComunicacionIntercargo(const char * directorio_de_trabajo) {
	//cola_cargadores_equipaje


	cola_cargadores_equipaje = new MessageQueue(
			string(directorio_de_trabajo).append(PATH_COLA_ROBOTS_INTERCARGO).c_str(), 0);
}

ApiComunicacionIntercargo::~ApiComunicacionIntercargo() {
	delete cola_cargadores_equipaje;
}

void ApiComunicacionIntercargo::informar_vuelo_entrante(int numero_vuelo_entrante,
		int numero_vuelo_destino) {
	mensaje.mtype = numero_vuelo_destino;
	mensaje.vuelo_entrante = numero_vuelo_entrante;
	cola_cargadores_equipaje->push(&mensaje, sizeof(MENSAJE_VUELO_ENTRANTE) - sizeof(long));
}

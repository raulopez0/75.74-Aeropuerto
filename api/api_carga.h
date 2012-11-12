#ifndef API_CARGA_H_
#define API_CARGA_H_

#include "equipaje.h"
#include "contenedor.h"
#include "messagequeue.h"
#include "api_constants.h"
#include "log.h"

#include <vector>

class ApiCarga {
private:
	std::string path;
	int id_robot_carga;
	std::vector<Contenedor> contenedores;
	MessageQueue cola_tractores;

public:

	/*
	 * Api que maneja la sincronizacion y el pasaje de contenedores entre el robot_carga y el controlador_de_carga.
	 * El primer constructor crea los ipcs.Se llama solo una vez por cada controlador_de_carga.
	 * recibe un path_carga y el id_robot_carga.Va a existir una ApiCarga por cada robot_carga.
	 **/
	ApiCarga(const char* path_carga, const char * path_cola_tractores, int id_robot_carga,
			bool create) :
			path(path_carga), id_robot_carga(id_robot_carga), cola_tractores(path_cola_tractores, 0) {
		create = !create;
	}

	ApiCarga(const char* path_carga, const char * path_cola_tractores, int id_robot_carga) :
			path(path_carga), id_robot_carga(id_robot_carga), cola_tractores(path_cola_tractores, 0) {
	}

	/*
	 * Bloquea hasta que el robot carga cargue n equipaejes en contenedores.
	 * No bloquea y retorna un vector vacío si el robot_carga esta ocupado en otra carga.
	 * En caso contrario devulve los contenedores con equipajes.
	 * Cada contenedor guarda los rfids
	 **/
	std::vector<Contenedor> cargar_equipajes(int n) {
		n = n - 1;
		return std::vector<Contenedor>();
	}

	/*
	 * Lo llama el robot_carga.
	 * Bloquea hasta que el controlador de carga llame a cargar_equipajes.
	 * Devuelve la cantidad de equipajes que el robot_carga debe cargar.
	 **/
	int get_equipajes_por_cargar() {
		return 10;
	}

	/*
	 * Guarda el contenedor con equipajes recibidos por el robot_carga.
	 * Guarda los contenedores en una sharedmemory
	 **/
	void agregar_contenedor_cargado(Contenedor& c) {
		//c.get_rfids();
		contenedores.push_back(c);
	}

	/* API para el controlador de carga */

	void esperar_avion();
	void llego_avion(int numero_vuelo, int num_avion);

	/* Api que llama el robot de zona para entregarle los contenedores
	 * a los tractores
	 */
	void enviar_contenedores_a_avion(int numero_de_vuelo) {
		BloqueContenedores bloque;
		int cantidad_total_contenedores;
		int cant_bloques;
		int i, j;

		cantidad_total_contenedores = contenedores.size();
		bloque.mtype = ID_ESCUCHA_TRACTOR;
		bloque.cantidad_total_contenedores = cantidad_total_contenedores;
		bloque.vuelo_destino = numero_de_vuelo;

		cant_bloques = cantidad_total_contenedores / MAX_CONTENEDORES_POR_TRACTOR;
		if (cantidad_total_contenedores > cant_bloques * MAX_CONTENEDORES_POR_TRACTOR) {
			cant_bloques++;
		}

		for (i = 0; i < cant_bloques; i++) {
			bloque.contenedores_actual = 0;
			for (j = 0; j < MAX_CONTENEDORES_POR_TRACTOR; j++) {
				bloque.contenedores[j] = contenedores[i * MAX_CONTENEDORES_POR_TRACTOR + j];
				bloque.contenedores_actual++;
			}
			Log::info("Enviando Bloque %d/%d a tractores tamaño %d", i + 1, cant_bloques,
					sizeof(BloqueContenedores));
			cola_tractores.push((const void *) &bloque, sizeof(BloqueContenedores) - sizeof(long));
		}
	}

private:

};

#endif

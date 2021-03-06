#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "api_scanner.h"
#include "log.h"
#include "cintas.h"
#include "constants.h"

int main(int argc, char *argv[])
try {
	int id_robot;
	int id_cinta_scanner;
	if (argc < 4) {
		Log::crit(
				"Insuficientes parametros para scanner, se esperaba (directorio_de_trabajo, id, id_cinta_scanner)\n");
		return (1);
	}

	id_robot = atoi(argv[2]); // de 1 a N

	if (id_robot < 1) {
		Log::crit("ID de robot_scanner incorrecto %d\n", id_robot);
		exit(1);
	}

	id_cinta_scanner = atoi(argv[3]);

	CintaScanner<Equipaje> cinta_scanner(std::string(argv[1]).append(PATH_CINTA_SCANNER).c_str(),
			id_cinta_scanner, id_robot);
	ApiScanner api_escaner_cinta_central(argv[1], id_robot);

	Log::info("Iniciando scanner(%s), %s\n", argv[2], argv[3]);

	for (;;) {
		Log::info("Intentando tomar un nuevo equipaje de cinta de scanner\n");
		Equipaje equipaje = cinta_scanner.sacar_equipaje();

		Log::info("Escaneando equipaje %s\n", equipaje.toString().c_str());
		equipaje.set_sospechoso((rand() % CANT_SOSPECHOSOS) == 0);

		sleep(rand() % SLEEP_ROBOT_SCANNER);

		if (equipaje.es_sospechoso()) {
			Log::info("se encontro sospechoso el equipaje %s\n", equipaje.toString().c_str());
		} else {
			Log::info("equipaje limpio: %s\n", equipaje.toString().c_str());
		}

		Log::info("pasando equipaje a cinta central (%s)\n", argv[2]);
		api_escaner_cinta_central.colocar_equipaje_en_cinta_principal(equipaje);
	}

}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}


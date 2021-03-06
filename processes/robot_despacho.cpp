#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <cstdio>

#include "api_despachante.h"
#include "equipaje.h"
#include "constants.h"
#include "cintas.h"

#include "database.h"
#include "stmt.h"
#include "tupleiter.h"

#include "log.h"

#include <string>
#include "yasper.h"

int main(int argc, char** argv) try {

	if (argc < 4) {
		Log::info(
				"insuf. param para robot de despacho,se esperaba(directorio_de_trabajo, id, zona_desde, zona_hasta...)\n");
		exit(1);
	}

	int id_robot = atoi(argv[2]);

	if (id_robot < 1) {
		Log::crit("ID de robot_despacho incorrecto %d\n", id_robot);
		exit(1);
	}

	int zona_desde = atoi(argv[3]);
	int zona_hasta = atoi(argv[4]);

	ApiDespachante despachante_cinta_central(argv[1],id_robot);


	for (;;) {

		sleep(rand() % SLEEP_ROBOT_DESPACHO);

		Log::info("Intentando tomar un nuevo equipaje de cinta central\n");

		Rfid rfid_equipaje = despachante_cinta_central.leer_proximo_equipaje();
      int num_zona = despachante_cinta_central.get_zona(rfid_equipaje.numero_de_vuelo_destino);

      Log::info("toma el equipaje %d con destino a zona (%d)",
                rfid_equipaje.rfid,  num_zona);
            
      if (rfid_equipaje.sospechoso) {
         
         Log::info("Equipaje %d sospechoso, vuelvo a ponerlo en la cinta central\n",
                   rfid_equipaje.rfid);
         despachante_cinta_central.avanzar_cinta();

      } else if ((num_zona == -1) || (num_zona < zona_desde) || (num_zona > zona_hasta)) {
               
         Log::info("Equipaje %d no pertenece a mis zonas, vuelve a cinta central\n",
                    rfid_equipaje.rfid);
         despachante_cinta_central.avanzar_cinta();
               
      } else {

         Log::info("OK, es para mi.Equipaje %d limpio envio a robot_carga zona %d\n",
                    rfid_equipaje.rfid, num_zona);
         Equipaje equipaje = despachante_cinta_central.extraer_equipaje();
         CintaContenedor cinta_contenedor(std::string(argv[1]).append(PATH_CINTA_CONTENEDOR).c_str(), num_zona);
         cinta_contenedor.poner_equipaje(equipaje, 1);
         despachante_cinta_central.avanzar_cinta();
      }
   }

 } catch(const std::exception &e) {
   Log::crit("%s", e.what());
 } catch(...) {
   Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

#ifndef APICHECKIN_H_
#define APICHECKIN_H_

#include "equipaje.h"
#include "semaphoreset.h"
#include "mutex.h"
#include "messagequeue.h"
#include <memory>
#include "sharedobject.h"
#include "cintas.h"

#define MAX_EQUIPAJES_POR_PERSONA 5

typedef struct t_msg_pasajero {
   long mtype;// id_checkin
   int id_pasajero;
   int cant_equipajes;
   Equipaje equipajes[MAX_EQUIPAJES_POR_PERSONA];
} tMensajePasajeroCheckin;


typedef struct t_msg_checkin {
   long mtype;// id_puesto_checkin
   bool iniciar_checkin; //false --> cerrar_checkin
   int num_vuelo;
   int num_zona;
} tMeansajeCheckin;

typedef struct vuelo_en_checkin {
   int num_vuelo;
   int cant_equipajes;
   int id_cinta_checkin;
   vuelo_en_checkin():num_vuelo(-1), cant_equipajes(0), id_cinta_checkin(-1) {
   }
   vuelo_en_checkin(int id_cinta_checkin):num_vuelo(-1), cant_equipajes(0), id_cinta_checkin(id_cinta_checkin) {
   }
} tVueloEnCheckin;

class PuestoCheckinSinVueloAsignado {
private:
   int id_checkin;
public:
   PuestoCheckinSinVueloAsignado(int id_checkin):id_checkin(id_checkin) {
   }
};

class ApiCheckIn {
public:

   ApiCheckIn(const char* directorio_de_trabajo,int id_checkin );
   virtual ~ApiCheckIn();

   /*
    * Asigna un vuelo al puesto de checkin.
    **/
   void iniciar_checkin( int numero_vuelo );

   /*
    * Cierra el checkin iniciado con #iniciar_checkin
    * El checkin debe haber sido abi
    **/
   int cerrar_checkin();

   /*
    * Bloquea hasta que llegue un mensaje de iniciar_checkin
    */
   void esperar_inicio_checkin();

   /*
    * Bloquea hasta que llegue un mensaje de controlador_de_checkin
    */
   void recibir_mensaje_controlador_checkin(tMeansajeCheckin&);

   /*
    *  Registra un equipaje que hace checkin.
    *  A partir de aca el avion no sale hasta que llega el equipaje
    **/
   void registrar_equipaje( Equipaje& );

   /*
    * Retorna el numero_vuelo que esta haciendo checkin en ese momento.
    * Si no se esta haciendo checkin en ese momento lanza la excepcion
    * PuestoCheckinSinVueloAsignado
    **/
   int get_vuelo_actual();

   /*
    * Indica que hay un checkin de pasajero en curso.
    * hasta que se llame a fin_checkin_pasajero
    **/
   void comienza_checkin_pasajero();

   /*
    * Llamar despues de registrar los equipajes de un pasajero
    *
    **/
   void fin_checkin_pasajero();

   void recibir_pasajero_para_checkin(int& id_pasajero, std::vector<Equipaje>& equipajes);

   void llego_pasajero_para_checkin(int id_pasajero, const std::vector<Equipaje>& equipajes);

   int get_cinta_checkin();

private:
   static const int cant_ipcs = 3;

   std::string path_to_locks;
   int id_checkin;
   SharedObject<tVueloEnCheckin> vuelo_actual;
   CintaCheckin cinta_checkin_out;
   SemaphoreSet sem_set;
   Mutex mutex_checkin;
   MessageQueue queue_pasajeros;

};

#endif /* APICHECKIN_H_ */

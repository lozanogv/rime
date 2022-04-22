#include "construccion_arbol.h"

///////////////////////////////////
//////////////FUNCIONES////////////
///////////////////////////////////

void load_beacon( beacon_struct_send *myself_information, linkaddr_t id, signed int rssi_c){
  linkaddr_copy(&myself_information->myself_node_id, &id);
  myself_information->myself_rssi = rssi_c;
}

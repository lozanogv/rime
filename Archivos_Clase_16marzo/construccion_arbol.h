
#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"

#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>
#include "contiki.h"
#include "list.h"

#define INF_NEG -999
#define True 1 
#define False 0

////////////////////////
//////ESTRUCTURAS///////
///////////////////////

typedef struct beacon_struct_send{
  linkaddr_t myself_node_id;
  packetbuf_attr_t myself_rssi;
}beacon_struct_send;


typedef struct beacon_struct_recv{
  linkaddr_t node_id;
  packetbuf_attr_t node_rssi;
  packetbuf_attr_t node_rssi_message;  
}beacon_struct_recv;

typedef struct node{
  linkaddr_t favorite_father;
  packetbuf_attr_t rssi_father;
}node;


struct list_struct{
struct list_struct *next; 
beacon_struct_recv beacon_recv_list; 
}; 


//////////////////////////////////
//////DIRECCION ESTRUCTURAS///////
//////////////////////////////////

typedef struct list_struct list_struct; 

beacon_struct_send myself_information;

struct node father_information;




/////////////////////////////////
//////////FUNCIONES/////////////
////////////////////////////////

void load_beacon(beacon_struct_send * myself_information, linkaddr_t id, signed int rssi_c);

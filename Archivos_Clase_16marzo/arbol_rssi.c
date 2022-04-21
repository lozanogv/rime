

#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"

#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>
#include "lib/list.h"
#include "construccion_arbol.h"


/*---------------------------------------------------------------------------*/
PROCESS(send_beacon, "Enviar Beacons");
PROCESS(select_parent, "selecionar padre");
PROCESS(clear_table,"limpiar tabla");
AUTOSTART_PROCESSES(&send_beacon,&select_parent,&clear_table);
/*---------------------------------------------------------------------------*/


LIST(node_list);
MEMB(node_list_mem, list_struct, 16);



static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from){
  
  list_struct *linked_list = NULL;
  beacon_struct_recv *beacon_recv = NULL;

  beacon_recv = ( (beacon_struct_recv*) packetbuf_dataptr());
  beacon_recv->node_rssi_message = packetbuf_attr(PACKETBUF_ATTR_RSSI);

  for(linked_list = list_head(node_list); linked_list != NULL; linked_list = list_item_next(linked_list))
    if(linkaddr_cmp(&linked_list->beacon_recv_list.node_id,from))break; 
    
  if(linked_list == NULL){
    linked_list = memb_alloc(&node_list_mem);
    if(linked_list == NULL)return;  

    linkaddr_copy(&linked_list->beacon_recv_list.node_id,from);
    list_add(node_list,linked_list); 
  }

  linked_list->beacon_recv_list.node_rssi = beacon_recv->node_rssi;
  linked_list->beacon_recv_list.node_rssi_message = beacon_recv->node_rssi_message;


  /*
  printf("//-----------------------broadcast-----------------------------------//");
  printf("\r\nbroadcast message received from %d\r\n with rssi =%d\r\n with rssi_enlace=%d\n ",
         beacon_recv->node_id.u8[0], beacon_recv->node_rssi, beacon_recv->node_rssi_message);
  printf("\n//-----------------------------------------------------------//");
  */
  process_post(&select_parent, PROCESS_EVENT_CONTINUE, NULL);
}

/* This function is called for every incoming unicast packet. */
static void recv_uc(struct unicast_conn *c, const linkaddr_t *from){
    printf("unicast ping received from %d.%d\n",
          from->u8[0], from->u8[1]); 

}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
static struct unicast_conn unicast;


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(send_beacon, ev, data){
  static struct etimer et;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();

  broadcast_open(&broadcast, 129, &broadcast_call);

 if(linkaddr_node_addr.u8[0] == 1)father_information.rssi_father = -1;
 else father_information.rssi_father = INF_NEG;
 
  while(True) {

    etimer_set(&et, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    load_beacon(&myself_information, linkaddr_node_addr, father_information.rssi_father);
    packetbuf_copyfrom(&myself_information, sizeof(beacon_struct_send));
    broadcast_send(&broadcast);
    //printf("\nbroadcast message sent --rssi %d\n",father_information.rssi_father);

  }

  PROCESS_END();
}

PROCESS_THREAD(select_parent,ev,data){

  PROCESS_EXITHANDLER(unicast_close(&unicast);)
  PROCESS_BEGIN();
  unicast_open(&unicast, 146, &unicast_callbacks);

  list_struct *data_list; 
  packetbuf_attr_t mayor = INF_NEG;
  
  while(True){

    PROCESS_YIELD();

    mayor = INF_NEG;
    printf("#L %d 0\n",father_information.favorite_father.u8[0]);
    if(father_information.rssi_father != -1){

      for(data_list = (list_struct * )list_head(node_list); data_list != NULL; data_list = (list_struct *)list_item_next(data_list)) {
        if(data_list->beacon_recv_list.node_rssi > mayor){
          mayor = data_list->beacon_recv_list.node_rssi; 
          father_information.rssi_father = data_list->beacon_recv_list.node_rssi_message + data_list->beacon_recv_list.node_rssi; 
          father_information.favorite_father = data_list->beacon_recv_list.node_id;
        }          
      }
      printf("#L %d 1\n",father_information.favorite_father.u8[0]); 
      unicast_send(&unicast,&father_information.favorite_father); 
     
    }

  } 
 
  PROCESS_END();

}


PROCESS_THREAD(clear_table,ev,data){

    static struct etimer etimer;
  PROCESS_BEGIN();


  list_struct *data_list; 

  while(True){

    etimer_set(&etimer, CLOCK_SECOND*30);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&etimer));
    for(data_list = list_head(node_list); data_list!= NULL; data_list = list_item_next(data_list)){
      list_remove(node_list, data_list);
      memb_free(&node_list_mem, data_list);
    } 
    printf("tabla limpia");
  }
   
  PROCESS_END();

}


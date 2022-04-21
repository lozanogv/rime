
#include "contiki.h"
#include "net/rime/rime.h"


#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>


typedef struct beacon_info{
  linkaddr_t addr_information_mote; 
  packetbuf_attr_t RSSI_information_mote;
}beacon_info;
beacon_info beacon_information = {0,0};
 
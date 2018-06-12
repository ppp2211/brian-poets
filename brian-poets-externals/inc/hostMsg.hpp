#ifndef HOSTMSG_H
#define HOSTMSG_H

/*
   hostMsg.hpp

   define the host message format for PCIe communication with POETS-XML graphs

*/

#define TinselWordsPerFlit 4

#define HOSTMSG_FLIT_SIZE 3 //4 flits (Maximum host message size in flits) 
#define HOST_MSG_PAYLOAD (TinselWordsPerFlit*HOSTMSG_FLIT_SIZE)+1// first flit is dedicated to the header 
#define HOSTMSG_MBOX_SLOT 1 // Reserve mailbox slot for host messages  

// Some kind of address. Just made this up.
#pragma pack(push,1)
typedef struct _address_t
{
    uint32_t thread;    // hardware
    uint16_t device;    // softare
    uint8_t pin;       // software
    uint8_t flag; //=0; // software
}address_t;
#pragma pack(pop)

//HostMsg Special Reserved Types
// 0x0F - RETURN (payload[0] returned value)
// 0x01 - STDOUT (payload[0] string address, payload[1-N] string parameter values)

//Format of messages recv to the host
#pragma pack(push,1)
typedef struct {
  address_t source; //where the message came from 
  uint32_t type; // host message type - unique identifier to the type in the xml
  uint32_t payload[HOST_MSG_PAYLOAD];
} hostMsg;
#pragma pack(pop)

// Format of messages that are to be injected into the system
#pragma pack(push,1)
typedef struct _packet_header_t
{
  address_t dest;
  address_t source;
  uint32_t size;  // total size, including header
}packet_header_t;
#pragma pack(pop)

// poets_packet
typedef struct _extern_to_pts_t
{
  packet_header_t header;
  uint32_t payload[11];
} extern_to_pts_t; // message that is the same format as the poets messages

// Definition of the server - executive message structure
typedef struct _pts_to_extern_t
{
  packet_header_t header;
  uint32_t type;
  uint32_t payload[10];
} pts_to_extern_t;


#endif /* HOSTMSG_H */

#ifndef EXECUTIVE_HH
#define EXECUTIVE_HH
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <stdio.h>
#include "hostMsg.hpp"
#include <iostream>
#include <fstream>
#include <boost/asio.hpp>

// PtsServe class
// used to help in the construction of POETS executives
// provides functionality to connect to the pts-serve server via named pipes
// also it is used to translate from named POETS application pins to addresses that the POETS network understands

class PtsServe {
  public:
    // constructor
    PtsServe(std::string appInAddrMapFile, std::string messageTypesFile, std::string socket_name);
    ~PtsServe(); // destructor

    // address and message translation
    void load_devpins(); // constructs the devpin_to_addr and addr_to_devpins mappings
    void load_messages(); // constructs the messages_to_numid and numid_to_messages mappings
    std::string formatPtsAddr(address_t in); // format the address in a string <thread>.<device>.<pin>
    std::string addr_to_dev(address_t in); // for a given poets address return the name of the device
    std::string numid_to_message(int in); // returns the message name for a given message numerical ID
    std::vector<std::string> getAppInPins(); // returns a copy of the app_in_pins vector
    address_t dev_to_addr(std::string dev); // returns the address of a given device pin name
    bool devExists(std::string dev); // checks to see if a device exists
  
    // server control 
    void connect(); // connect to pts-serve via the named pipes
    unsigned tryRecv(pts_to_extern_t *msg); // recv a message non-blocking 
    void send(extern_to_pts_t *msg); // send a message to the POETS server
    
    // debug
    void print(); // prints the mappings to stdout

  private:
    std::string appInAddrMapFile_; // csv file containing the translation from POETS app pin names to addresses
    std::string messageTypesFile_; // csv file containing the unique numerical identifiers for each POETS message
    std::ifstream appInAddrMap_; // loaded file for appInAddrMapFile
    std::ifstream messageTypes_; // loaded file for appInAddrMapFile
    std::map<std::string, address_t> devpin_to_addr_; // mapping from a named device pin to an address
    std::map<std::string, int> message_to_numid_; // mapping from a named message type to a numerical ID
    // can't have a struct as a key -- so using an address formatted as a string
    std::map<std::string, std::string> addr_to_devpin_; // mapping from a POETS address to a named device pin
    std::map<int, std::string> numid_to_message_; // mapping from message numerical id to named type
    std::vector<std::string> app_in_pins_; // a vector of the app input pin names
    std::vector<std::string> app_out_pins_; // a vector of the app output pin names

    // socket
    boost::asio::io_service *ptsserve_ioservice_;
    boost::asio::local::stream_protocol::endpoint* exec_ep_; // endpoint for the send channel
    boost::asio::local::stream_protocol::socket* exec_socket_; // socket for the send channel 
   
    std::string socket_; // name for the socket to connect to pts-serve
}; 

// sends a message to hte poets server which injects it into the network
void PtsServe::send(extern_to_pts_t *msg) {
  exec_socket_->send(boost::asio::buffer(msg, sizeof(pts_to_extern_t)));
}

// non-blocking receive from the poets server
// returns the number of bytes that were accepted
unsigned PtsServe::tryRecv(pts_to_extern_t *msg) {
  if(exec_socket_->available() >= sizeof(pts_to_extern_t)) { // check if packet is available
    exec_socket_->receive(boost::asio::buffer(msg, sizeof(pts_to_extern_t)));
    return sizeof(pts_to_extern_t);
  }
  return 0;
} 

//! checks to see if a device exists
bool PtsServe::devExists(std::string dev) {
   if(devpin_to_addr_.find(dev) == devpin_to_addr_.end()) {
     return false;
   } 
   return true;
}

// returns the address of a given device pin name
address_t PtsServe::dev_to_addr(std::string dev) {
   return devpin_to_addr_[dev];
}

// returns the message name for a given message numerical ID
std::string PtsServe::numid_to_message(int in){ 
  return numid_to_message_[in];
}

std::string PtsServe::formatPtsAddr(address_t in) {
  std::stringstream ss;
  ss << in.thread << "." << in.device << "." << unsigned(in.pin) << "\0";
  return ss.str(); 
}

// for a given poets address return the name of the device
std::string PtsServe::addr_to_dev(address_t in) {
   std::string addr = formatPtsAddr(in);
   if(addr_to_devpin_.find(addr) == addr_to_devpin_.end()) {
     std::cerr << "Error: no corrosponding pin for address: "<<addr<<"\n";
     exit(1);
   } else {
     // found the address
     return addr_to_devpin_[addr]; 
   }
}

// constructs the messages_to_numid and numid_to_messages mappings
void PtsServe::load_messages() {
  std::string line;
  while(std::getline(messageTypes_, line)){
    // we have the line, split by comma
    std::stringstream ss(line);
    // get the message type name
    std::string message_type;
    std::getline(ss, message_type, ',');

    // get the unique ident for the message
    std::string message_numid_str;
    uint32_t message_numid;
    std::getline(ss, message_numid_str, '\n');
    message_numid = std::stoi(message_numid_str);
    
    // adding element to the mappings
    message_to_numid_[message_type] = message_numid;
    numid_to_message_[message_numid] = message_type;
  }
} 

// returns a copy of the app_in_pins vector
std::vector<std::string> PtsServe::getAppInPins() {
  return app_in_pins_;
}

// constructs the devpin_to_addr and addr_to_devpins mappings
void PtsServe::load_devpins() {
  std::string line;
  while(std::getline(appInAddrMap_, line)){
    // we have the line, split by comma
    std::stringstream ss(line);

    // get device pin name
    std::string dev_pin_name;
    std::getline(ss, dev_pin_name, ',');

    // get the direction
    std::string dir;
    std::getline(ss, dir, ',');
    // assign to the corresponding vector
    if(dir == "in") { 
      app_in_pins_.push_back(dev_pin_name); 
    } else {
      app_out_pins_.push_back(dev_pin_name); 
    }

    // get the thread id
    std::string thread_str;
    std::getline(ss, thread_str, ',');
    uint32_t thread = std::stoi(thread_str);
   
    // get the device id
    std::string device_str;
    std::getline(ss, device_str, ',');
    uint32_t device = std::stoi(device_str);
   
    // get the pin id
    std::string pin_str;
    std::getline(ss, pin_str, '\n');
    uint32_t pin = std::stoi(pin_str);
    
    // constructed the address 
    address_t adr;
    adr.thread = thread;
    adr.device = (uint16_t)device;
    adr.pin = (uint8_t)pin;
    
    std::stringstream formatted_adr;
    formatted_adr << thread << "." << device << "." << unsigned(pin) << "\0";
    std::string formatted_adr_str = formatted_adr.str();  
 
    // adding element to the mappings  
    devpin_to_addr_[dev_pin_name] = adr;
    addr_to_devpin_[formatted_adr_str] = dev_pin_name;
  } 
  return;
}

// Constructor
PtsServe::PtsServe(std::string appInAddrMapFile, std::string messageTypesFile, std::string socket_name) {

  std::string socket_ = socket_name;  

  ptsserve_ioservice_ = new boost::asio::io_service();
  exec_ep_ = new boost::asio::local::stream_protocol::endpoint(socket_name);
  exec_socket_= new boost::asio::local::stream_protocol::socket(*ptsserve_ioservice_);
  exec_socket_->connect(*exec_ep_); // send data down the send_pipe 

  ptsserve_ioservice_->run();
 
  // Open the files
  appInAddrMapFile_ = appInAddrMapFile; 
  messageTypesFile_ = messageTypesFile;
  appInAddrMap_.open(appInAddrMapFile_);
  messageTypes_.open(messageTypesFile_);

  // load the data into the maps
  load_devpins();
  load_messages();

}

// prints the mappings for debugging
void PtsServe::print() {
  std::cout << " DevPin to Address mappings\n" 
            << "--------------------------------\n";
  for(auto const& x : devpin_to_addr_) {
    std::cout << x.first << " : " << x.second.thread << "." << x.second.device << "." << x.second.pin << "\n";
  }

  std::cout << "\n Message to numid mappings\n" 
  	  << "--------------------------------\n";
  for(auto const& x : message_to_numid_) {
    std::cout << x.first << " : " << x.second << "\n";
  }
  
}

// destructor
PtsServe::~PtsServe()
{
  // close the pipes and file descriptors
  
}

#endif /* EXECUTIVE_HH */

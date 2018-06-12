#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>
#include <queue>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "hostMsg.hpp"
#include "PtsServe.hpp"
#include <ctime>
#include <chrono>
// I think I have been collecting header files... need to figure out what I need

/*
     external 
  --------------------------------------------------
  A simple external application for the firefly application.

*/

namespace po = boost::program_options;

std::string print_info() {
  std::stringstream o;
  o << "--------------------------------------------------\n";
  o << "                  external\n";
  o << "--------------------------------------------------\n";
  o << "A simple external application for the firefly application.\n";
  o << "collects flashes from the fireflies and sends them to be rendered.\n";
  o << "Fireflies synchronise with each other, but can be forced to flash by\n";
  o << "sending a message\n";
  o << "\n";

return o.str();
} 

// check if a file exists
// https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
inline bool fileExists(const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}


int main(int ac, const char *av[])
{
  try {
  // Boost program options
  po::options_description desc("Options");
  desc.add_options()
  ("help,h", "prints usage")
  ("message-types,m", po::value<std::string>()->default_value("pingMessageTypes.csv"), "the list of IDs associated with each message type -- generated by ptsxmlc")
  ("app-pin-addrs,a", po::value<std::string>()->default_value("pingAppInPinAddrs.csv"), "file mapping device pin names (as specified in the xml) to ")
  ("socket,s", po::value<std::string>()->default_value("~/.ptssocket"), "the name of the socket to connect the external and pts-serve");

  po::variables_map vm;
  po::store(po::parse_command_line(ac, av, desc), vm);

  //print info and usage
  if(vm.count("help")) {
     std::cout << print_info();
     std::cout << desc << std::endl;
     return 1;
  }

  // named pipe arguments
  std::string socket_name = vm["socket"].as<std::string>();

  // message types and pin address mappings
  std::string message_types = vm["message-types"].as<std::string>();
  std::string app_pin_addrs = vm["app-pin-addrs"].as<std::string>();

  // create the poets-server object
  std::cerr << "connecting to pts-serve...";
  PtsServe pts_server(app_pin_addrs,message_types,socket_name);
  std::cerr << " done\n";

  std::vector<std::string> app_in_pins = pts_server.getAppInPins();

  auto start = std::chrono::high_resolution_clock::now();
  auto last_refresh = std::chrono::high_resolution_clock::now();

  // read from the named fifo coming back from node.js
  unsigned seenchar = 0;
  sleep(2); // make sure the node.js server has started
  int to_poets_fd = open("./to_poets_devices.sock", O_RDONLY | O_NONBLOCK);
  if(to_poets_fd == -1) {
    std::cerr << "ERROR: could not open shared pipe with node.js server\n";
    exit(0);
  }
  bool dev_rdy = false;
  std::string device;
  std::ostringstream dev_ss;

  while(1) {
    
    auto curr_t = std::chrono::high_resolution_clock::now(); 

    // try and get data from the pipe
    pts_to_extern_t inMsg;
    if(pts_server.tryRecv(&inMsg)) {
      std::string dev = pts_server.addr_to_dev(inMsg.header.source);
      std::cout << dev << "\n"; // print on stdout
      fflush(stdout);
    }  

   char c;
   int bytes = read(to_poets_fd, &c, sizeof(char));
   if(bytes == 1) {
     if(c == ';') {
       device = dev_ss.str();
       dev_rdy = true;
     } else {
       dev_ss << c;
       dev_rdy = false;
       seenchar++;
     }
   }
   

   if(dev_rdy) { // send to pts-serve
     dev_rdy = false;
     if(pts_server.devExists(device)){
       extern_to_pts_t item;
       item.header.size = sizeof(extern_to_pts_t);
       item.header.dest = pts_server.dev_to_addr(device);
       item.header.source.thread = 0;
       item.header.source.device = 0;
       item.header.source.pin = 0;
       pts_server.send(&item); // send to the poets network
     }
     device.clear(); //erase the string
     dev_ss.str(std::string()); // clear the stringstream
   }

  }

  return 0;

  } catch(std::exception& e) {
    std::cout << e.what() << "\n";
  }
}

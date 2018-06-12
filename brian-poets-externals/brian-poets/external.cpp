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

/*
     external 
  --------------------------------------------------
  A simple external application for the ping application.

*/

namespace po = boost::program_options;

std::string print_info() {
  std::stringstream o;
  o << "--------------------------------------------------\n";
  o << "                  external\n";
  o << "--------------------------------------------------\n";
  o << "A simple external application for the ping application.\n";
  o << "Sends a message to every exposed application pin in the xml\n";
  o << "expecting the same answer to come back\n";
  o << "\n";

return o.str();
} 


int main(int ac, const char *av[])
{
  try {
  // Boost program options
  po::options_description desc("Options");
  desc.add_options()
  ("help,h", "prints usage")
  ("message-types,m", po::value<std::string>()->default_value("snnMessageTypes.csv"), "the list of IDs associated with each message type -- generated by ptsxmlc")
  ("app-pin-addrs,a", po::value<std::string>()->default_value("snnAppInPinAddrs.csv"), "file mapping device pin names (as specified in the xml) to ")
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

  // create a vector of all the application input pins in the POETS XML
  std::vector<std::string> app_in_pins = pts_server.getAppInPins();
  
  while(1) {
    // try and get data from the pipe
    pts_to_extern_t inMsg;
    if(pts_server.tryRecv(&inMsg)) {
      int p= inMsg.payload[1];
      float *pf = (float *)(&p);
      double pd = double(*pf);
      printf("%u, %f\n", inMsg.payload[0], pd);
      }
    }  
  return 0;

  } catch(std::exception& e) {
    std::cout << e.what() << "\n";
  }
}

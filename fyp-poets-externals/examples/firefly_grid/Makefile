INC=../../inc

#-----------------------------------------------------------
# edit these
#-----------------------------------------------------------
# remote section
REMOTE_USER=sf306
REMOTE_SERVER=aesop.cl.cam.ac.uk
REMOTE_SOCKET=/home/$(REMOTE_USER)/poets-ecosystem/examples/externals/firefly/firefly.sock
REMOTE_MESSAGE_TYPES=/home/$(REMOTE_USER)/poets-ecosystem/examples/externals/firefly/fireflyMessageTypes.csv
REMOTE_PIN_ADDRS=/home/$(REMOTE_USER)/poets-ecosystem/examples/externals/firefly/fireflyAppInPinAddrs.csv

# local section
LOCAL_SOCKET=firefly.sock
LOCAL_MESSAGE_TYPES=fireflyMessageTypes.csv
LOCAL_APP_PIN_ADDRS=fireflyAppPinAddrs.csv
#-----------------------------------------------------------

REMOTE_FULL=$(REMOTE_USER)@$(REMOTE_SERVER)

all: external build_webserver

node_modules:
	npm install express
	npm install ws

webserver/data.json: firefly.xml
	python scripts/generate_d3_json_data.py --input firefly.xml > webserver/data.json

webserver/index.html: server_files/index.html
	cp server_files/index.html webserver/index.html

webserver/main.js: server_files/main.js
	cp server_files/main.js webserver/main.js

webserver:
	mkdir -p webserver

build_webserver: webserver node_modules webserver/data.json webserver/index.html webserver/main.js

external: external.cpp
	g++ -std=c++11 -O2 -I$(INC) -o external external.cpp -lboost_program_options -lboost_filesystem -lboost_system

$(LOCAL_MESSAGE_TYPES): 
	scp $(REMOTE_FULL):$(REMOTE_MESSAGE_TYPES) $(LOCAL_MESSAGE_TYPES)
	
$(LOCAL_APP_PIN_ADDRS):
	scp $(REMOTE_FULL):$(REMOTE_PIN_ADDRS) $(LOCAL_APP_PIN_ADDRS)  


launch_webserver: external build_webserver $(LOCAL_MESSAGE_TYPES) $(LOCAL_APP_PIN_ADDRS)
	../../scripts/socat_script $(LOCAL_SOCKET) $(REMOTE_FULL) $(REMOTE_SOCKET)
	./external --socket $(LOCAL_SOCKET) --message-types $(LOCAL_MESSAGE_TYPES) --app-pin-addrs $(LOCAL_APP_PIN_ADDRS) | nodejs webserver/main 
	

clean:
	rm -f *.csv external *.sock
	rm -rf webserver node_modules
	rm -f to_poets_devices.sock 
	rm *.json

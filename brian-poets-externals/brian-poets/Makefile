INC=../inc

#-----------------------------------------------------------
# edit these
#-----------------------------------------------------------
# remote section
REMOTE_USER=pp3114
REMOTE_SERVER=aesop.cl.cam.ac.uk
REMOTE_DIR=/home/$(REMOTE_USER)/brian-poets/
REMOTE_SOCKET=$(REMOTE_DIR)/snn.sock
REMOTE_MESSAGE_TYPES=$(REMOTE_DIR)/snnMessageTypes.csv
REMOTE_PIN_ADDRS=$(REMOTE_DIR)/snnAppPinAddrMap.csv

# local section
LOCAL_SOCKET=snn.sock
LOCAL_MESSAGE_TYPES=snnMessageTypes.csv
LOCAL_APP_PIN_ADDRS=snnAppPinAddrMap.csv
#-----------------------------------------------------------

REMOTE_FULL=$(REMOTE_USER)@$(REMOTE_SERVER)

all: external

external: external.cpp
	g++ -std=c++11 -O2 -I$(INC) -o external external.cpp -lboost_program_options -lboost_filesystem -lboost_system

run_external: external
	scp $(REMOTE_FULL):$(REMOTE_MESSAGE_TYPES) $(LOCAL_MESSAGE_TYPES)
	scp $(REMOTE_FULL):$(REMOTE_PIN_ADDRS) $(LOCAL_APP_PIN_ADDRS)  
	#socat "UNIX-LISTEN:$(LOCAL_SOCKET)" EXEC:"ssh $(REMOTE_FULL) socat STDIO UNIX-CONNECT\:$(REMOTE_SOCKET)" &
	../scripts/socat_script $(LOCAL_SOCKET) $(REMOTE_FULL) $(REMOTE_SOCKET)
	./external --socket $(LOCAL_SOCKET) --message-types $(LOCAL_MESSAGE_TYPES) --app-pin-addrs $(LOCAL_APP_PIN_ADDRS)

clean:
	rm -f *.csv external *.sock

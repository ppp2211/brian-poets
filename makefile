GS = ../graph_schema/
JING = $(GS)external/jing-20081028/bin/jing.jar

CPPFLAGS += -I $(GS)include -W -Wall -Wno-unused-parameter -Wno-unused-variable
CPPFLAGS += $(shell pkg-config --cflags libxml++-2.6)
CPPFLAGS += -Wno-unused-local-typedefs
CPPFLAGS += -I providers
CPPFLAGS += -std=c++11 -g

LDLIBS += $(shell pkg-config --libs-only-l libxml++-2.6)
LDFLAGS += $(shell pkg-config --libs-only-L --libs-only-other libxml++-2.6)

PYTHON = python3

ifeq ($(OS),Windows_NT)
SO_CPPFLAGS += -shared
else
# http://stackoverflow.com/a/12099167
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
SO_CPPFLAGS += -dynamiclib -fPIC
else
SO_CPPFLAGS += -shared -fPIC
LDFLAGS += -pthread
LDLIBS += -ldl -fPIC
endif
endif


$(JING) : $(GS)external/jing-20081028.zip
	if [[ ! -f $(JING) ]] ; then \
		(cd $(GS)external && unzip -o jing-20081028.zip) \
	fi
	touch $@

define provider_rules_template

providers/$1.graph.cpp providers/$1.graph.hpp : apps/$1/$1_graph_type.xml $(JING)
	mkdir -p providers
	java -jar $(JING) -c $(GS)master/virtual-graph-schema-v2.rnc apps/$1/$1_graph_type.xml
	$$(PYTHON) $(GS)tools/render_graph_as_cpp.py apps/$1/$1_graph_type.xml providers/$1.graph.cpp
	$$(PYTHON) $(GS)tools/render_graph_as_cpp.py --header < apps/$1/$1_graph_type.xml > providers/$1.graph.hpp

providers/$1.graph.so : providers/$1.graph.cpp
	g++ $$(CPPFLAGS) -Wno-unused-but-set-variable $$(SO_CPPFLAGS) $$< -o $$@ $$(LDFLAGS) $(LDLIBS)

$1_provider : providers/$1.graph.so

all_providers : $1_provider

endef

include apps/snn_test/makefile.inc
include apps/snn_noclock_test/makefile.inc
include apps/snn_always_spike/makefile.inc
include apps/snn_taylor/makefile.inc

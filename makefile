# Run make to see list of commands.

########################## Executable (Falcon.out) arguments ##########################
# arg[0]: Falcon.out
# arg[1]: Party number (0,1,2)
# arg[2]: File containing IP addresses of all parties
# arg[3]: AES key for local randomness generation (each party has a different file)
# arg[4]: AES key for common randomness generation (with next party)
# arg[5]: AES key for common randomness generation (with prev party)

# OPTIONAL ARGUMENTS: If supplied, these arguments take precedence over the ones in src/main.cpp
# 					  Refer to Makefile Parameters section below for details.
# arg[6]: Which predefined network
# arg[7]: What dataset to use
# arg[8]: Which adversarial model


############################## Makefile parameters ######################################
# Location of OPENSSL installation if running over servers without sudo access
OPEN_SSL_LOC := /data/swagh/conda
# RUN_TYPE {localhost, LAN or WAN} 
RUN_TYPE := localhost
# NETWORK {SecureML, Sarda, MiniONN, LeNet, AlexNet, and VGG16}
NETWORK := MiniONN
# Dataset {MNIST, CIFAR10, and ImageNet}
DATASET	:= MNIST
# Security {Semi-honest or Malicious} 
SECURITY:= Semi-honest
#########################################################################################




#########################################################################################
CXX=g++
SRC_CPP_FILES     := $(wildcard src/*.cpp)
OBJ_CPP_FILES     := $(wildcard util/*.cpp)
OBJ_FILES    	  := $(patsubst src/%.cpp, src/%.o,$(SRC_CPP_FILES))
OBJ_FILES    	  += $(patsubst util/%.cpp, util/%.o,$(OBJ_CPP_FILES))
HEADER_FILES       = $(wildcard src/*.h)

# FLAGS := -static -g -O0 -w -std=c++11 -pthread -msse4.1 -maes -msse2 -mpclmul -fpermissive -fpic
FLAGS := -O3 -w -std=c++11 -pthread -msse4.1 -maes -msse2 -mpclmul -fpic
LIBS := -lcrypto -lssl
OBJ_INCLUDES := -I 'lib_eigen/' -I 'util/Miracl/' -I 'util/' -I '$(OPEN_SSL_LOC)/include/'
BMR_INCLUDES := -L./ -L$(OPEN_SSL_LOC)/lib/ $(OBJ_INCLUDES) 


help: ## Run make or make help to see list of options
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-15s\033[0m %s\n", $$1, $$2}'

all: Falcon.out ## Just compile the code

Falcon.out: $(OBJ_FILES)
	g++ $(FLAGS) -o $@ $(OBJ_FILES) $(BMR_INCLUDES) $(LIBS)

%.o: %.cpp $(HEADER_FILES)
	$(CXX) $(FLAGS) -c $< -o $@ $(OBJ_INCLUDES)

clean: ## Run this to clean all files
	rm -rf Falcon.out
	rm -rf src/*.o util/*.o

################################# Remote runs ##########################################
terminal: Falcon.out ## Run this to print the output of (only) Party 0 to terminal
	./Falcon.out 2 files/IP_$(RUN_TYPE) files/keyC files/keyAC files/keyBC >/dev/null &
	./Falcon.out 1 files/IP_$(RUN_TYPE) files/keyB files/keyBC files/keyAB >/dev/null &
	./Falcon.out 0 files/IP_$(RUN_TYPE) files/keyA files/keyAB files/keyAC 
	@echo "Execution completed"

file: Falcon.out ## Run this to append the output of (only) Party 0 to file output/3PC.txt
	mkdir -p output
	./Falcon.out 2 files/IP_$(RUN_TYPE) files/keyC files/keyAC files/keyBC >/dev/null &
	./Falcon.out 1 files/IP_$(RUN_TYPE) files/keyB files/keyBC files/keyAB >/dev/null &
	./Falcon.out 0 files/IP_$(RUN_TYPE) files/keyA files/keyAB files/keyAC >>output/3PC.txt
	@echo "Execution completed"

valg: Falcon.out ## Run this to execute (only) Party 0 using valgrind. Change FLAGS to -O0.
	./Falcon.out 2 files/IP_$(RUN_TYPE) files/keyC files/keyAC files/keyBC >/dev/null &
	./Falcon.out 1 files/IP_$(RUN_TYPE) files/keyB files/keyBC files/keyAB >/dev/null &
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --dsymutil=yes ./Falcon.out 0 files/IP_$(RUN_TYPE) files/keyA files/keyAB files/keyAC

command: Falcon.out ## Run this to use the run parameters specified in the makefile. 
	./Falcon.out 2 files/IP_$(RUN_TYPE) files/keyC files/keyAC files/keyBC $(NETWORK) $(DATASET) $(SECURITY) >/dev/null &
	./Falcon.out 1 files/IP_$(RUN_TYPE) files/keyB files/keyBC files/keyAB $(NETWORK) $(DATASET) $(SECURITY) >/dev/null &
	./Falcon.out 0 files/IP_$(RUN_TYPE) files/keyA files/keyAB files/keyAC $(NETWORK) $(DATASET) $(SECURITY) 
	@echo "Execution completed"


################################## tmux runs ############################################
zero: Falcon.out ## Run this to only execute Party 0 (useful for multiple terminal runs)
	./Falcon.out 0 files/IP_$(RUN_TYPE) files/keyA files/keyAB files/keyAC 

one: Falcon.out ## Run this to only execute Party 1 (useful for multiple terminal runs)
	./Falcon.out 1 files/IP_$(RUN_TYPE) files/keyB files/keyBC files/keyAB

two: Falcon.out ## Run this to only execute Party 2 (useful for multiple terminal runs)
	./Falcon.out 2 files/IP_$(RUN_TYPE) files/keyC files/keyAC files/keyBC
#########################################################################################

.PHONY: help


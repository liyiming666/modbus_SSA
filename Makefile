BIN=bin
BUILD=build
LIB=lib
SRC=src
INCLUDE=/home/liym/code/cumulocity-sdk-c
INC=.
PROJ=modbus
#OBJS = modbus.o
SOURCE=$(wildcard $(SRC)/*.cc)
OBJS=$(addprefix $(BUILD)/, $(SOURCE:.cc=.o))
CXX = g++
RM = rm -rf 
CFLAGS = -c 

$(BUILD)/%.o:%.cc
		mkdir -p $(dir $@)
		#mkdir -p build
		$(CXX)  $(CFLAGS) $^ -I$(INCLUDE)/include -Iinclude/modbus -I$(INC)/include -L$(C8Y_LIB_PATH)/lib -L$(LIB) -lsera -std=c++11 -o $@ -lpthread -lmodbus

$(BIN)/$(PROJ):$(OBJS)
		mkdir -p $(dir $@)
		#mkdir -p bin
		$(CXX) $^ -I$(INCLUDE)/include -I$(INC)/include  -std=c++11 -L$(C8Y_LIB_PATH)/lib -L$(LIB) -lsera -o $@ -lpthread -lmodbus

clean:
		$(RM) $(BIN)/* $(BUILD)/* 

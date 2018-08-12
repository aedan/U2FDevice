#!/usr/bin/env make

SRC_DIR  := .
OBJ_DIR  := obj
LDFLAGS  := -lmbedcrypto
CPPFLAGS :=
CXXFLAGS := --std=c++14

CXXFLAGS += -MMD -MP -Wall -Wfatal-errors -Wextra
MODULES  := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS  := $(MODULES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

U2FDevice: $(OBJECTS) libuECC.o libcppb64.o
	g++ $(LDFLAGS) -o $@ $^

install: U2FDevice
	install -m775 -t /usr/bin U2FDevice
	install -m775 -t /etc/systemd/system U2FDevice.service
	install -d /usr/share/U2FDevice/

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

-include $(OBJECTS:.o=.d)

clean:
	rm $(OBJ_DIR)/*
	rm U2FDevice

.PHONY: libuECC.o libcppb64.so clean install
libuECC.o:
	$(MAKE) -C micro-ecc
	cp micro-ecc/libuECC.o libuECC.o

libcppb64.o:
	$(MAKE) -C cpp-base64
	cp cpp-base64/libcppb64.o libcppb64.o
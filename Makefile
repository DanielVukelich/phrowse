CXX=g++
INC_DIR=src
CXXFLAGS=-std=c++11 -Werror -Wall -I$(INC_DIR)
LDLIBS=-lncurses
OBJS=client.o connection.o menu.o display.o history.o
BIN=gopher

release:	$(BIN)

debug:	CXXFLAGS += -DDEBUG -g
debug:	$(BIN)

.PHONY: clean all debug release

$(BIN): $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(OBJS) $(LDLIBS)

%.o: %.cpp %.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -f $(BIN)
	rm -r $(OBJS)

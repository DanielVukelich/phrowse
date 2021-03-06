CXX=g++
INC_DIR=src
CXXFLAGS=-std=c++11 -Werror -Wall -I$(INC_DIR)
LDLIBS=-lncurses
OBJS=client.o connection.o menu.o display.o history.o guri.o menu_item.o
BIN=phrowse

release:	$(BIN)

debug:	CXXFLAGS += -DDEBUG -g -fsanitize=address -fsanitize=undefined
debug:	LDLIBS += -lubsan
debug:	$(BIN)

.PHONY: clean all debug release

$(BIN): $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(OBJS) $(LDLIBS)

%.o: %.cpp %.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -f $(BIN)
	rm -f $(OBJS)

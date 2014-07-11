OBJ = SPaths.o operazioni_grafo.o disegna_grafo.o operazioni_file.o
DEBUG_FLAGS=-g -D DEBUG_MODE
CXXFLAGS=$(DEBUG_FLAGS) -Wall `pkg-config --cflags gtk+-3.0 glib-2.0`
LDFLAGS=-export-dynamic `pkg-config --libs gtk+-3.0 glib-2.0`

SPaths: $(OBJ)
	g++ -o SPaths $(OBJ) $(LDFLAGS)

-include dependencies

.PHONY: depend clean cleanall

depend:
	g++ -MM *.cc > dependencies
clean:
	rm -f *.o SPaths dependencies
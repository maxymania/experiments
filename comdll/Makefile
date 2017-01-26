
GCC=gcc
GXX=g++
CFLAGS=-Iinclude
DLLFLA=-fvisibility=hidden -fPIC

LFLAGS=-lpthread -lzmq

backend=zeromq

threading=pthread

ifeq ($(backend),zeromq)
lib += src/dll/zmqbak/recv_send.o
lib += src/dll/zmqbak/init.o
lib += src/dll/zmqbak/connect.o
lib += src/dll/zmqbak/keygen.o
endif



ifeq ($(threading),pthread)
lib += src/dll/posixthr/threads.o
endif


lib += src/dll/service.o
lib += src/dll/svcmap.o

sho=cle64.dll

prog += src/main.o

prog: $(prog) $(sho)
	$(GCC) $(prog) $(sho) $(LFLAGS) -o prog

$(sho): $(lib)
	$(GXX) -shared -Wl,-soname,$(sho) $(lib) $(LFLAGS) -o $(sho)

src/dll/%.o: src/dll/%.c
	$(GCC) -c $(DLLFLA) $(CFLAGS) $< -o $@

src/dll/%.o: src/dll/%.cc
	$(GXX) -c $(DLLFLA) $(CFLAGS) $< -o $@

src/%.o: src/%.c
	$(GCC) -c $(CFLAGS) $< -o $@

clean:
	rm $(prog) $(sho) $(lib)

#do LD_LIBRARY_PATH=$(pwd) ./prog


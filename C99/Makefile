CC ?= clang
CFLAGS ?= -O3 -Wall -Wextra -std=c99 -I../include -fPIC
LDFLAGS ?= 

all: libunicagd_aml.dylib unicagd_aml_c99

libunicagd_aml.dylib: unicagd_aml_core.c ../include/unicagd_aml_core.h
	$(CC) $(CFLAGS) -shared -install_name @rpath/libunicagd_aml.dylib unicagd_aml_core.c -o libunicagd_aml.dylib

libunicagd_aml.so: unicagd_aml_core.c ../include/unicagd_aml_core.h
	$(CC) $(CFLAGS) -shared unicagd_aml_core.c -o libunicagd_aml.so

unicagd_aml_c99: main.c unicagd_aml_core.c ../include/unicagd_aml_core.h
	$(CC) $(CFLAGS) main.c unicagd_aml_core.c -o unicagd_aml_c99 $(LDFLAGS)

# Cosmopolitan Actually Portable Executable (APE) target
cosmo:
	cosmocc -O3 -std=c99 -I../include main.c unicagd_aml_core.c -o unicagd_aml.com

clean:
	rm -f *.o *.dylib *.so *.dll unicagd_aml_c99 unicagd_aml.com

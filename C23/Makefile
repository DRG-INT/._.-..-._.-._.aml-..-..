CC ?= clang
CFLAGS ?= -O3 -Wall -Wextra -std=c2x -I../include
LDFLAGS ?= -L../C99 -lunicagd_aml

all: unicagd_aml_c23

unicagd_aml_c23: main.c
	$(CC) $(CFLAGS) main.c ../C99/unicagd_aml_core.c -o unicagd_aml_c23

clean:
	rm -f unicagd_aml_c23

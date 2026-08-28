CXX ?= clang++
CXXFLAGS ?= -O3 -Wall -Wextra -std=c++23 -I../include

all: unicagd_aml_cpp23

unicagd_aml_cpp23: main.cpp ../C99/unicagd_aml_core.c
	$(CXX) $(CXXFLAGS) main.cpp ../C99/unicagd_aml_core.c -o unicagd_aml_cpp23

clean:
	rm -f unicagd_aml_cpp23

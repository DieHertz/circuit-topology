CC=g++
CXX_FLAGS=-std=c++11 -Wall -Werror -g

OUT_NAME=main

H_FILES=\
	circuit.hpp element.hpp \
	matrix.hpp range.hpp \
	relations.hpp topology.hpp

CPP_FILES=main.cpp

default: $(H_FILES) $(CPP_FILES)
	$(CC) $(CPP_FILES) -o $(OUT_NAME) $(CXX_FLAGS)

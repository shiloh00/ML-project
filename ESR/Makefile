CXXFLAGS= -Iesr/ -march=core-avx2 -O3 -pipe -fomit-frame-pointer --std=c++11 -Wall -g
#CXXFLAGS= -Iesr/ -O2 --std=c++11 -g -Wall
LDFLAGS= `pkg-config --libs opencv`
CC=g++

OBJ_LIST = alignment.o regressor.o fern.o mat_serialize.o

all: esr-train esr-predict esr-predict-server err-detect

esr-train: train_main.o $(OBJ_LIST)
	$(CC) -o $@ $^ $(LDFLAGS)

esr-predict: predict_main.o $(OBJ_LIST)
	$(CC) -o $@ $^ $(LDFLAGS)

err-detect: err_detect.o $(OBJ_LIST)
	$(CC) -o $@ $^ $(LDFLAGS)

esr-predict-server: predict_server.o $(OBJ_LIST)
	$(CC) -o $@ $^ $(LDFLAGS)

err_detect.o: err_detect.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $^

train_main.o: train_main.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $^

predict_main.o: predict_main.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $^

predict_server.o: predict_server.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $^

alignment.o: esr/alignment.cpp esr/alignment.hpp
	$(CC) -c $(CXXFLAGS) -o $@ $<

regressor.o: esr/regressor.cpp esr/regressor.hpp
	$(CC) -c $(CXXFLAGS) -o $@ $<

fern.o: esr/fern.cpp esr/fern.hpp
	$(CC) -c $(CXXFLAGS) -o $@ $<

mat_serialize.o: esr/mat_serialize.cpp esr/mat_serialize.hpp
	$(CC) -c $(CXXFLAGS) -o $@ $<

.PHONY: clean
clean: 
	rm -rf *.o esr-train esr-predict *.out

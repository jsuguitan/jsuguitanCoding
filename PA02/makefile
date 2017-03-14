CC = mpic++
CXXFLAGS=-Wall
LIBS=-lpmi

FILE1=src/sequential.cpp
FILE2=src/PIMFuncs.cpp
FILE3=src/PIMFuncs.h
FILE4=src/dynamic.cpp

OUT1=sequence
OUT2=dynamic



all: $(OUT1) $(OUT2)

$(OUT1): $(FILE1) PIMFuncs.o
	$(CC) $(CXXFLAGS) -o $(OUT1) $(FILE1) PIMFuncs.o $(LIBS)

$(OUT2): $(FILE4) PIMFuncs.o
	$(CC) $(CXXFLAGS) -o $(OUT2) $(FILE4) PIMFuncs.o $(LIBS)
	
PIMFuncs.o: $(FILE2) $(FILE3)
	g++ -Wall -c -g $(FILE2)

clean:
	rm *.o -rf ~$(OUT1) ~$(OUT2) *~

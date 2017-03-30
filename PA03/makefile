CC = mpic++
CXXFLAGS=-Wall
LIBS=-lpmi

FILE1=src/sequential.cpp
FILE2=src/createValues.cpp

OUT1=sequence
OUT2=createValues

all: $(OUT1) $(OUT2)

$(OUT1): $(FILE1) 
	$(CC) $(CXXFLAGS) -o $(OUT1) $(FILE1) $(LIBS)

$(OUT2): $(FILE2) 
	$(CC) $(CXXFLAGS) -o $(OUT2) $(FILE2) $(LIBS)

clean:
	rm *.o -rf ~$(OUT1) ~$(OUT2) *~

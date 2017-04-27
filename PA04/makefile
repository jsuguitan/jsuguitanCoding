CC = mpic++
CXXFLAGS=-Wall
LIBS=-lpmi

FILE1=src/sequential.cpp
FILE2=src/parallel.cpp
FILE3=src/createValues.cpp

OUT1=sequence
OUT2=dynamic
OUT3=createValues

all: $(OUT1) $(OUT2) $(OUT3)

$(OUT1): $(FILE1) 
	$(CC) $(CXXFLAGS) -o $(OUT1) $(FILE1) $(LIBS)
$(OUT2): $(FILE2) 
	$(CC) $(CXXFLAGS) -o $(OUT2) $(FILE2) $(LIBS)
$(OUT3): $(FILE3) 
	$(CC) $(CXXFLAGS) -o $(OUT3) $(FILE3) $(LIBS)

clean:
	rm *.o *.sh~ -rf ~$(OUT1) ~$(OUT2) ~$(OUT3) *~a *~

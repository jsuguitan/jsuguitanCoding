CC = mpic++
CXXFLAGS=-Wall
LIBS=-lpmi

FILE1=src/pa01.cpp
FILE2=src/pa01p3.cpp

OUT1=pa01
OUT2=pa01p3


all: $(OUT1) $(OUT2)

$(OUT1): $(FILE1)
	$(CC) $(CXXFLAGS) -o $(OUT1) $(FILE1) $(LIBS)
   
$(OUT2): $(FILE2)
	$(CC) $(CXXFLAGS) -o $(OUT2) $(FILE2) $(LIBS)
	
clean:
	rm -rf ~$(OUT1) ~$(OUT2)

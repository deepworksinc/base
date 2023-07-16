CC = clang++
CFLAGS = -Wall -std=c++17
INCLUDES = -I/usr/local/include/eigen3 -I/usr/local/include
LFLAGS = -L/usr/local/lib
LIBS = -lgsl -lgslcblas -lm -lboost_date_time

SOURCES = main.cpp
OBJECTS = $(addprefix build/, $(SOURCES:.cpp=.o))
TARGET = build/app

all: build $(TARGET)

build:
	mkdir -p build

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJECTS) $(LFLAGS) $(LIBS)

build/%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(TARGET)
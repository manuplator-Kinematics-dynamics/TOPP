SHELL=/bin/bash

SOURCE=$(wildcard *.cpp)
OBJECTS=$(SOURCE:.cpp=.o)
TARGET=TOPPbindings.so
LIB=-lboost_python
INCLUDE=$(shell python-config --includes)
CC=g++ -std=c++0x -O2 -Wall -fPIC
CCG=g++ -g -std=c++0x -Wall -fPIC

TESTS=$(wildcard tests/*.py)
PYTHON=python


so: $(OBJECTS)
	$(CC) $(INCLUDE) $(SOURCE) -shared $(LIB) -o $(TARGET)

%.o: %.cpp
	$(CC) $(INCLUDE) -c $< 

debug: $(SOURCE) 
	$(CCG) $(INCLUDE) $(SOURCE) -shared $(LIB) -o $(TARGET)

clean:
	rm -f $(OBJECTS)
	rm -f *~

distclean: clean
	rm -f $(TARGET)

unit_tests:
	@for f in $(TESTS); do $(PYTHON) $$f; done


.PHONY: so debug clean distclean unit_tests
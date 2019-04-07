CXX = g++
CXXFLAGS = -std=c++11

all: qclient cserver clean

cserver: question.o server.o cserver.o
	$(CXX) $(CXXFLAGS) -o cserver question.o server.o cserver.o

question.o: question.cpp question.h
	$(CXX) $(CXXFLAGS) -c question.cpp
server.o: server.cpp server.h
	$(CXX) $(CXXFLAGS) -c server.cpp
cserver.o: cserver.cpp server.h
	$(CXX) $(CXXFLAGS) -c cserver.cpp

qclient: contestmeister.o qclient.o
	$(CXX) $(CXXFLAGS) -o qclient contestmeister.o qclient.o

qclient.o: qclient.cpp contestmeister.h
	$(CXX) $(CXXFLAGS) -c qclient.cpp
contestmeister.o: contestmeister.cpp contestmeister.h
	$(CXX) $(CXXFLAGS) -c contestmeister.cpp

clean:
	rm contestmeister.o qclient.o question.o server.o cserver.o

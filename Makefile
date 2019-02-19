CXX = g++
CXXFLAGS = -std=c++11

all: qclient qserver

qserver: question.o server.o qserver.o
	$(CXX) $(CXXFLAGS) -o qserver question.o server.o qserver.o

question.o: question.cpp question.h
	$(CXX) $(CXXFLAGS) -c question.cpp
server.o: server.cpp server.h
	$(CXX) $(CXXFLAGS) -c server.cpp
qserver.o: qserver.cpp server.h
	$(CXX) $(CXXFLAGS) -c qserver.cpp

qclient: client.o qclient.o
	$(CXX) $(CXXFLAGS) -o qclient client.o qclient.o

qclient.o: qclient.cpp client.h
	$(CXX) $(CXXFLAGS) -c qclient.cpp
client.o: client.cpp client.h
	$(CXX) $(CXXFLAGS) -c client.cpp

clean: 
	rm client.o qclient.o question.o server.o qserver.o
CXX = g++
CXXFLAGS = -std=c++11

all: contestmeister contestant cserver

cserver: question.o server.o cserver.o contest.o
	$(CXX) $(CXXFLAGS) -o cserver question.o server.o cserver.o contest.o -lpthread

question.o: question.cpp question.h
	$(CXX) $(CXXFLAGS) -c question.cpp
server.o: server.cpp server.h
	$(CXX) $(CXXFLAGS) -c server.cpp
cserver.o: cserver.cpp server.h
	$(CXX) $(CXXFLAGS) -c cserver.cpp
contest.o: contest.cpp contest.h
	$(CXX) $(CXXFLAGS) -c contest.cpp


contestmeister: contestmeister.o qclient.o
	$(CXX) $(CXXFLAGS) -o contestmeister contestmeister.o qclient.o
qclient.o: qclient.cpp contestmeister.h
	$(CXX) $(CXXFLAGS) -c qclient.cpp
contestmeister.o: contestmeister.cpp contestmeister.h
	$(CXX) $(CXXFLAGS) -c contestmeister.cpp

contestant: contestant.o cclient.o
	$(CXX) $(CXXFLAGS) -o contestant contestant.o cclient.o
contestant.o: contestant.cpp contestant.h
	$(CXX) $(CXXFLAGS) -c contestant.cpp
cclient.o: cclient.cpp
	$(CXX) $(CXXFLAGS) -c cclient.cpp


.PHONY: clean
clean:
	rm contestmeister.o qclient.o question.o server.o cserver.o contest.o contestant.o cclient.o

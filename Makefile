CXX=g++
CFLAGS=-std=c++11 -O2 -Wall -g
CXXFLAGS=-std=c++11 -O2 -Wall -g

TARGET:=myserver
SOURCE:=$(wildcard ../*.cc)
OBJS=./buffer.cc ./HTTPrequest.cc ./HTTPresponse.cc ./HTTPConnection.cc \
	 ./timer.cc ./epoller.cc ./webServer.cc ./main.cc

$(TARGET):$(OBJS)
	$(CXX) $(CXXFLAGS)	$(OBJS) -o ./bin/$(TARGET) -pthread

clean:
	rm ./bin/*
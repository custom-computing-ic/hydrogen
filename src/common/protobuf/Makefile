

all: packet.pb.h

packet.pb.h: packet.proto
	protoc --cpp_out=./ $^


clean:
	rm packet.pb.h
	rm packet.pb.cc

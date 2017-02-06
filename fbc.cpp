// Homework 2
// fbc.cpp
// Facebook Chat Room Client
// Colin Banigan and Katherine Drake

/*
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>
*/

#include <sstream>

#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
//#include "helper.h"
#include "facebook.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using facebookChat::fbChatRoom;
using facebookChat::ListRequest;
using facebookChat::ListReply;
using facebookChat::JoinRequest;
using facebookChat::JoinReply;
using facebookChat::LeaveRequest;
using facebookChat::LeaveReply;

using namespace std;

bool chatMode = false; //client starts in commandMode

class facebookClient {
    private: 
    unique_ptr<fbChatRoom::Stub> stub;
    
    public:
    facebookClient(string address) {
        // create a new channel to server
        shared_ptr<Channel> channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
        
        cout << "Client is connected on: " << address << endl;
        
        stub = fbChatRoom::NewStub(channel);
    }
    
    void testJoin() {
        // create join request and reply objects
        JoinRequest request;
        JoinReply reply;
        
        request.set_name("test");
        
        // send join request to server
        ClientContext context;
        Status status = stub->Join(&context, request, &reply); 
        
        // check if request was successful
        if (!status.ok()) {
            cout << "Error Happened\n";
            cout << "Join Reply: " << reply.name() << endl;
        }
        else {
            cout << "Join Reply: " << reply.name();
        }
    }
    /*
    
    Join(JoinRequest) returns (JoinReply) {}
    
    void GetFeature() {
    Point point;
    Feature feature;
    point = MakePoint(409146138, -746188906);
    GetOneFeature(point, &feature);
    point = MakePoint(0, 0);
    GetOneFeature(point, &feature);
  }
  
    bool GetOneFeature(const Point& point, Feature* feature) {
    ClientContext context;
    Status status = stub_->GetFeature(&context, point, feature);
    if (!status.ok()) {
      std::cout << "GetFeature rpc failed." << std::endl;
      return false;
    }
    if (!feature->has_location()) {
      std::cout << "Server returns incomplete feature." << std::endl;
      return false;
    }
    if (feature->name().empty()) {
      std::cout << "Found no feature at "
                << feature->location().latitude()/kCoordFactor_ << ", "
                << feature->location().longitude()/kCoordFactor_ << std::endl;
    } else {
      std::cout << "Found feature called " << feature->name()  << " at "
                << feature->location().latitude()/kCoordFactor_ << ", "
                << feature->location().longitude()/kCoordFactor_ << std::endl;
    }
    return true;
  }
  */
};

//string split functions below
void split(const string &s, char delim, vector<string> &elems) {
	stringstream ss;
	ss.str(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
}

vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}


int main(int argc, char* argv[]) {
    
    string hostName = "localhost";
    string portNumber = "16230";
    
    if (argc >= 3) {
        hostName = argv[1];
        portNumber = argv[2];
    }
    
    // create facebook chat client
    facebookClient client(hostName + ":" + portNumber);
    
    client.testJoin();
    
    return 0;
}
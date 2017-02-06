// Homework 2
// fbc.cpp
// Facebook Chat Room Client
// Colin Banigan and Katherine Drake

#include <sstream>

#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
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
    string portNumber = "16231";
    
    if (argc >= 3) {
        hostName = argv[1];
        portNumber = argv[2];
    }
    
    // create facebook chat client
    facebookClient client(hostName + ":" + portNumber);
    
    client.testJoin();
    
    return 0;
}
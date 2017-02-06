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
using facebookChat::ListRequest;
using facebookChat::ListReply;
using facebookChat::JoinRequest;
using facebookChat::JoinReply;
using facebookChat::LeaveRequest;
using facebookChat::LeaveReply;

using namespace std;

bool chatMode = false; //client starts in commandMode

class facebookClient {
 public:
    facebookClient() {}
    /*
  facebookClient(shared_ptr<Channel> channel, const string& db)
      : stub_(RouteGuide::NewStub(channel)) {
    routeguide::ParseDb(db, &feature_list_);
  }
    */
  //unique_ptr<RouteGuide::Stub> stub_;
  //vector<Feature> feature_list_;
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
    //string db = routeguide::GetDbFileContent(argc, argv);
    
    string portNumber;
    if (argc != 2) {
        portNumber = "16230";
    }
    else {
        portNumber = argv[1];
    }
    
    facebookClient client();
    /*
    RouteGuideClient guide(
    grpc::CreateChannel(portNumber,
                      grpc::InsecureChannelCredentials()),
    db);
*/
    return 0;
}
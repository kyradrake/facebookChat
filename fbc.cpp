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
using facebookChat::LoginRequest;
using facebookChat::LoginReply;
using facebookChat::ListRequest;
using facebookChat::ListReply;
using facebookChat::JoinRequest;
using facebookChat::JoinReply;
using facebookChat::LeaveRequest;
using facebookChat::LeaveReply;
using facebookChat::ChatRequest;
using facebookChat::ChatReply;

using namespace std;

bool chatMode = false; //client starts in commandMode

class facebookClient {
    private: 
    unique_ptr<fbChatRoom::Stub> stub;
    string username;
    
    public:
    
    facebookClient(string address, string username) {
        // create a new channel to server
        shared_ptr<Channel> channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
        
        cout << "Client is connected on " << address << endl;
        
        stub = fbChatRoom::NewStub(channel);
        
        username = username;
    }
    
    void login() {
        // create login request and reply objects
        LoginRequest request;
        LoginReply reply;
        
        // set request username
        request.set_username(username);
        
        // send login request to server
        ClientContext context;
        Status status = stub->Login(&context, request, &reply);
        
        // check if request was successful
        if (!status.ok()) {
            cout << "Error Occured: Server Cannot Login.\n";
        }
        /*else if (!reply.has_name()) { 
            cout << "Error Occured: Server Returned Incomplete Data.\n";
        }*/
        else {
            // print server's reply
            cout << reply.reply();
        }
    }
    
    void list() {
        // create list request and reply objects
        ListRequest request;
        ListReply reply;
        
        // set request username
        request.set_username();
        
        // send list request to server
        ClientContext context;
        Status status = stub->List(&context, request, &reply);
        
        // check if request was successful
        if (!status.ok()) {
            cout << "Error Occured: Server Cannot List Users.\n";
        }
        /*else if (!reply.has_name()) { 
            cout << "Error Occured: Server Returned Incomplete Data.\n";
        }*/
        else {
            // print server's reply
            cout << reply.reply();
        }
    }
        
    void join(string chatRoom) {
        // create join request and reply objects
        JoinRequest request;
        JoinReply reply;
        
        // set request username and chat room to join
        request.set_username(username);
        request.set_chatRoom(chatRoom);
        
        // send join request to server
        ClientContext context;
        Status status = stub->Join(&context, request, &reply);
        
        // check if request was successful
        if (!status.ok()) {
            cout << "Error Occured: Server Cannot Join Chat Room.\n";
        }
        /*else if (!reply.has_name()) { 
            cout << "Error Occured: Server Returned Incomplete Data.\n";
        }*/
        else {
            // print server's reply
            cout << reply.reply();
        }
    }
    
    void leave(string chatRoom) {
        // create leave request and reply objects
        LeaveRequest request;
        LeaveReply reply;
        
        // set request username and chat room to leave
        request.set_username(username);
        request.set_chatRoom(chatRoom);
        
        // send leave request to server
        ClientContext context;
        Status status = stub->Leave(&context, request, &reply);
        
        // check if request was successful
        if (!status.ok()) {
            cout << "Error Occured: Server Cannot Leave Chat Room.\n";
        }
        /*else if (!reply.has_name()) { 
            cout << "Error Occured: Server Returned Incomplete Data.\n";
        }*/
        else {
            // print server's reply
            cout << reply.name();
        }
    }
    
    void chat() {
        // create chat request and reply objects
        ChatRequest request;
        ChatReply reply;
        
        // set request username
        request.set_username(username);
        
        // send chat request to server
        ClientContext context;
        Status status = stub->Chat(&context, request, &reply);
        
        // check if request was successful
        if (!status.ok()) {
            cout << "Error Occured: Server Cannot Initiate Chat.\n";
        }
        /*else if (!reply.has_name()) { 
            cout << "Error Occured: Server Returned Incomplete Data.\n";
        }*/
        else {
            // print server's reply
            cout << reply.reply();
            
            // change from command mode to chat mode
            chatMode = true;
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

void commandMode(facebookClient* client) {
    string command;
    getline(cin, command);
    
    vector<string> splitCommand = split(command, ' ');

    if (command.compare(0, 4, "LIST") == 0) {
        client->list();
    }
    else if (command.compare(0, 4, "JOIN") == 0 && splitCommand.size() == 2) {
        string chatRoom = splitCommand[1];
        client->join(chatRoom);
    }
    else if (command.compare(0, 5, "LEAVE") == 0 && splitCommand.size() == 2) {
        string chatRoom = splitCommand[1];
        client->leave(chatRoom);
    }
    else if (command.compare(0, 4, "CHAT") == 0) {
        client->chat();
    }
    else {
        cout << command << " is not a valid command! Please enter LIST, JOIN, LEAVE, or CHAT: \n";
    }
}

int main(int argc, char* argv[]) {
    
    string hostName = "localhost";
    string portNumber = "16231";
    string username;
    
    if (argc >= 4) {
        hostName = argv[1];
        portNumber = argv[2];
        username = argv[3];
    }
    else {
        cerr << "Command Line Arguments Not Provided... Program is Terminating\n";
        return 0;
    }
    
    // create facebook chat client
    facebookClient client(hostName + ":" + portNumber, username);
    
    client.login();
    
    while (!chatMode) {
        commandMode(&client);
    }
    
    return 0;
}
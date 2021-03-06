// Homework 2
// fbc.cpp
// Facebook Chat Room Client
// Colin Banigan and Katherine Drake

#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>

#include <pthread.h>

#include "helper.h"

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
using facebookChat::ChatMessage;
using facebookChat::ChatReply;

using namespace std;

bool inChatMode = false;    //client starts in commandMode

class facebookClient {
private:
    unique_ptr<fbChatRoom::Stub> stub;
    string username;
    
public:
    // client constructor
    facebookClient(string address, string uname) {
        // create a new channel to server
        shared_ptr<Channel> channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
        
        cout << "Client is connected on " << address << endl;
        
        // generate stub
        stub = fbChatRoom::NewStub(channel);
        
        // set client's username
        username = uname;
    }
    
    // client "logs in" to server 
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
        else {
            // print server's reply
            cout << reply.reply();
        }
    }
    
    // LIST command
    void list() {
        // create list request and reply objects
        ListRequest request;
        ListReply reply;
        
        // set request username
        request.set_username(username);
        
        // send list request to server
        ClientContext context;
        Status status = stub->List(&context, request, &reply);
        
        // check if request was successful
        if (!status.ok()) {
            cout << "Error Occured: Server Cannot List Users.\n";
        }
        else {
            // print server's reply
            cout << reply.reply();
        }
    }
    
    // JOIN command
    void join(string chatRoom) {
        // create join request and reply objects
        JoinRequest request;
        JoinReply reply;
        
        // set request username and chat room to join
        request.set_username(username);
        request.set_chatroom(chatRoom);
        
        // send join request to server
        ClientContext context;
        Status status = stub->Join(&context, request, &reply);
        
        // check if request was successful
        if (!status.ok()) {
            cout << "Error Occured: Server Cannot Join Chat Room.\n";
        }
        else {
            // print server's reply
            cout << reply.reply();
        }
    }
    
    // LEAVE command
    void leave(string chatRoom) {
        // create leave request and reply objects
        LeaveRequest request;
        LeaveReply reply;
        
        // set request username and chat room to leave
        request.set_username(username);
        request.set_chatroom(chatRoom);
        
        // send leave request to server
        ClientContext context;
        Status status = stub->Leave(&context, request, &reply);
        
        // check if request was successful
        if (!status.ok()) {
            cout << "Error Occured: Server Cannot Leave Chat Room.\n";
        }
        else {
            // print server's reply
            cout << reply.reply();
        }
    }
    
    // CHAT command
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
        else {
            // print server's reply
            cout << reply.message();
            
            // change from command mode to chat mode
            inChatMode = true;
        }
    }
    
    // client sends chat message to server
    void sendChat(string msg) const {         
        // create chat message and reply objects
        ChatMessage message;
        ChatReply reply;
        
        // set chat message username and message
        message.set_username(username);
        message.set_message(msg);
        
        // send chat message to server
        ClientContext context;
        Status status = stub->SendChatToServer(&context, message, &reply);
        
        // check if message was successful
        if (!status.ok()) {
            cout << "Error Occured: Server Cannot Receive Message.\n";
        }
    }
    
    // client receives chat from server
    void receiveChat() {
        // create chat request and reply objects
        ChatRequest request;
        ChatReply reply;
        
        // set request username
        request.set_username(username);
        
        // send chat request to server
        ClientContext context;
        Status status = stub->SendChatToClient(&context, request, &reply);
        
        // check if message was successful
        if (!status.ok()) {
            cout << "Error Occured: Server Cannot Receive Message.\n";
        }
        else if (reply.message() != "") {
            cout << reply.message() << endl;
        }
    }
};

// manages commands while client is in command mode
void commandMode(facebookClient* client) {
    string command;
    getline(cin, command);
    
    // read in the command and split it on whitespace
    vector<string> splitCommand = split(command, ' ');
    
    // check the first charcters to see if it matches a related command
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

// continuously waits for client chat messages
void* chatMode(void* client) {
    facebookClient* fbClient = (facebookClient*) client;
    
    while (true) {
        // wait for message via command line
        string chatMessage;
        getline(cin, chatMessage);

        // check for valid message
        if (chatMessage != "" && chatMessage != "\n") {
            // send chat message to server
            fbClient->sendChat(chatMessage);
        }
    }
}


int main(int argc, char* argv[]) {
    
    string hostName;
    string portNumber;
    string username;
    
    // if arguments are provided, set them
    if (argc >= 4) {
        hostName = argv[1];
        portNumber = argv[2];
        username = argv[3];
    }
    else {      // else, terminate program
        cerr << "Command Line Arguments Not Provided... Program is Terminating\n";
        return 0;
    }
    
    // create facebook chat client
    facebookClient client(hostName + ":" + portNumber, username);
    
    // send "login" request to server
    client.login();
    
    // start in command mode
    while (!inChatMode) {
        commandMode(&client);
    }
    
    // create thread for client to send messages to server
    pthread_t chatThread; 
    int createChatThread = pthread_create (&chatThread, NULL, chatMode, (void*) &client);
    if (createChatThread < 0) {
        cout << "ERROR: Couldn't Create Chat Thread\n";
    }
    
    // continuously poll for messages from server
    while (true) {
        client.receiveChat();
    }
    
    // join chat thread
    pthread_join(chatThread, NULL); 
    
    return 0;
}
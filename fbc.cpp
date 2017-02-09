// Homework 2
// fbc.cpp
// Facebook Chat Room Client
// Colin Banigan and Katherine Drake

#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>

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

bool inChatMode = false; //client starts in commandMode

class facebookClient {
private:
    unique_ptr<fbChatRoom::Stub> stub;
    string username;
    
public:
    
    facebookClient(string address, string uname) {
        // create a new channel to server
        shared_ptr<Channel> channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
        
        cout << "Client is connected on " << address << endl;
        
        // generate stub
        stub = fbChatRoom::NewStub(channel);
        
        // set client's username
        username = uname;
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
            cout << reply.reply();
            
            // change from command mode to chat mode
            inChatMode = true;
        }
    }
    
    void sendChat() {         

        ClientContext context;
        
        static string name = username;

        shared_ptr<ClientReaderWriter<ChatMessage, ChatMessage> > stream(
            stub->ChatStream(&context));

        thread writer([stream]() {
            while (true) {
                string msg;
                getline(cin, msg);
                
                ChatMessage ChatMsg;
                ChatMsg.set_username(name);
                ChatMsg.set_message(msg);
                ChatMsg.set_datetime("");
                
                stream->Write(ChatMsg);
            }
            
            
            //stream->WritesDone();
        });

        /*
        // create a reader thread to read in messages from the server
        thread reader([stream]() {
            ChatMessage serverMsg;
            
            while (stream->Read(&serverMsg)) {
              cout << "Got message " << serverMsg.message() << endl;
            }
        });
        */
        
        
        while (true) {
            ChatMessage serverMsg;
            while (stream->Read(&serverMsg)) {
                cout << "Got message " << serverMsg.message() << endl;
                        //<< " at " << server_note.location().latitude() << ", "
                        //<< server_note.location().longitude() << std::endl;
            }
        }
        
        
        
        
        stream->WritesDone();
        writer.join();
        //reader.join();
        Status status = stream->Finish();
        if (!status.ok()) {
          std::cout << "RouteChat rpc failed." << std::endl;
        }
        
    }
};

// switches the client into command mode, the client stays here until a chat command is issued
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

/*
void chatMode(facebookClient* client) {
    // wait for message via command line
    string chatMessage;
    getline(cin, chatMessage);
    
    // get current date and time
    string time = getDateAndTime();
    
    cout << "Time: " << time << endl;
    
    // send chat message to server
    client->sendChat(chatMessage, time);
}
*/
/*
void RouteChat() {
    ClientContext context;

    std::shared_ptr<ClientReaderWriter<RouteNote, RouteNote> > stream(
        stub_->RouteChat(&context));

    std::thread writer([stream]() {
      std::vector<RouteNote> notes{
        MakeRouteNote("First message", 0, 0),
        MakeRouteNote("Second message", 0, 1),
        MakeRouteNote("Third message", 1, 0),
        MakeRouteNote("Fourth message", 0, 0)};
      for (const RouteNote& note : notes) {
        std::cout << "Sending message " << note.message()
                  << " at " << note.location().latitude() << ", "
                  << note.location().longitude() << std::endl;
        stream->Write(note);
      }
      stream->WritesDone();
    });

    RouteNote server_note;
    while (stream->Read(&server_note)) {
      std::cout << "Got message " << server_note.message()
                << " at " << server_note.location().latitude() << ", "
                << server_note.location().longitude() << std::endl;
    }
    writer.join();
    Status status = stream->Finish();
    if (!status.ok()) {
      std::cout << "RouteChat rpc failed." << std::endl;
    }
  }*/

int main(int argc, char* argv[]) {
    
    // default values
    string hostName;
    string portNumber;
    string username;
    
    // if arguments are provided, set them
    if (argc >= 4) {
        hostName = argv[1];
        portNumber = argv[2];
        username = argv[3];
    }
    else { // else, terminate
        cerr << "Command Line Arguments Not Provided... Program is Terminating\n";
        return 0;
    }
    
    // create facebook chat client
    facebookClient client(hostName + ":" + portNumber, username);
    
    client.login();
    
    while (!inChatMode) {
        commandMode(&client);
    }
    
    while (true) {
        //chatMode(&client);
        client.sendChat();
    }
    return 0;
}
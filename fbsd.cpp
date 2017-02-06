// Homework 2
// fbsd.cpp
// Facebook Chat Room Server
// Colin Banigan and Katherine Drake

#include <fstream>
#include <iostream>
#include "UserData.h"

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
#include "facebook.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
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

vector<UserData> listOfUsers; //global list of all the connected clients

//send all of the users and their connected chat rooms back to the specified user
void listCommand(string user){
    string totalList = "";
    for(int i = 0; i < listOfUsers.size(); i++){
        totalList += "User: " + listOfUsers[i].name + "\n";
        totalList += "Users connected: [";
        for(int j = 0; j < listOfUsers[i].usersConnectedTo.size(); j++){
            totalList += listOfUsers[i].usersConnectedTo[j];
            if(j != listOfUsers[i].usersConnectedTo.size() - 1){
                totalList += ", ";
            }
        }
        totalList += "]\n";
        totalList += "-------------------------------------------------------------\n";
    }
    
    //send totalList back to the client somehow right here
}

//take a specified user and add another specified user to their list of connected users
void joinCommand(string user, string userToJoin){
    for(int i = 0; i < listOfUsers.size(); i++){
        if(listOfUsers[i].name == userToJoin){
            bool exists = false;
            for(int j = 0; j < listOfUsers[i].usersConnectedTo.size(); j++){
                if(listOfUsers[i].usersConnectedTo[j] == user) {
                    exists = true;
                }
            }
            if(!exists){
                listOfUsers[i].usersConnectedTo.push_back(user);
                cout << "Added " << user << " to " << userToJoin << " successfully" << endl;
            }
        }
    }
}

//take a specified user and remove another specified user from their list of connected users
void leaveCommand(string user, string userToLeave){
    //iterate through users to see if the current user exists
    for(int i = 0; i < listOfUsers.size(); i++){
        if(listOfUsers[i].name == user){
            //user exists, check to see if the user 
            for(int j = 0; j < listOfUsers[i].usersConnectedTo.size(); j++){
                if(listOfUsers[i].usersConnectedTo[j] == userToLeave) {
                    listOfUsers[i].usersConnectedTo.erase(listOfUsers[i].usersConnectedTo.begin()+j);
                    cout << "Left " << userToLeave << "'s chat room on " << user << "'s account successfully" << endl;
                }
            }
        }
    }
}

//takes the chat sent from the specified user and appends it to their chat database file
void saveChatToFile(string user, string chat){
    ofstream outfile;

    outfile.open(user+".txt", ios_base::app);
    outfile << chat;
}

void chatCommand(string user, string chat){
    saveChatToFile(user, chat);
    for(int i = 0; i < listOfUsers.size(); i++){
        for(int j = 0; listOfUsers[i].usersConnectedTo.size(); j++){
            if(listOfUsers[i].usersConnectedTo[j] == user){
                //send chat string to listOfUsers[i].name right here
            }
        }
    }
}


    
class facebookServer final : public fbChatRoom::Service {
    public:
    facebookServer() {}
    
    // process client JOIN command
    Status Join(ServerContext* context, const JoinRequest* request,
                     JoinReply* reply) override {
        cout << "Server Context: " << context << endl;
        reply->set_name("Joined Chat Room " + request->name());
        return Status::OK;
    }
};

void startServer(string portNumber) {
    // create facebookServer object
    facebookServer facebookServer;
    
    // gRPC class object to create a gRPC server
    // man page for ServerBuilder: http://www.grpc.io/grpc/cpp/classgrpc_1_1_server_builder.html#a0b06b5828b892feeb6541c8eeae2d542
    ServerBuilder serverBuilder;
    
    // binds server to localhost address
    serverBuilder.AddListeningPort("localhost:" + portNumber, grpc::InsecureServerCredentials());
    
    serverBuilder.RegisterService(&facebookServer);
    
    // return a running server that is ready to process calls
    unique_ptr<Server> server(serverBuilder.BuildAndStart());
    
    cout << "Server is running on: " << "localhost:" + portNumber << endl;
    
    // server waits until killed
    server->Wait();
}

int main(int argc, char* argv[]) {
    string portNumber = "16231";
    if (argc >= 2) {
        portNumber = argv[1];
    }

    startServer(portNumber);
    
    return 0;
}

// Homework 2
// fbsd.cpp
// Facebook Chat Room Server
// Colin Banigan and Katherine Drake

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>

#include "UserData.h"
#include "helper.h"
 
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
using facebookChat::ChatMessage;
using facebookChat::ChatReply;
 
using namespace std;
 
vector<UserData> listOfUsers;   // global list of all users 

// check is a user exists 
bool userExists(string user) {
    for (int i = 0; i < listOfUsers.size(); i++) {
        if (listOfUsers[i].name == user) {
            return true;
        }
    }
    return false;
}

// send all of the users and their connected chat rooms back to the specified user
string listCommand() {
    string totalList = "";
    for (int i = 0; i < listOfUsers.size(); i++) {
        totalList += "User: " + listOfUsers[i].name + "\n";
        totalList += "Users connected: [";
        for (int j = 0; j < listOfUsers[i].usersConnectedTo.size(); j++) {
            totalList += listOfUsers[i].usersConnectedTo[j];
            if (j != listOfUsers[i].usersConnectedTo.size() - 1) {
                totalList += ", ";
            }
        }
        totalList += "]\n";
        totalList += "-------------------------------------------------------------\n";
    }
    return totalList;
}
 
// take a specified user and add another specified user to their list of connected users
string joinCommand(string user, string userToJoin) {
    if (!userExists(userToJoin)){
        string rValue = "ERROR: " + userToJoin + " does not exist\n";
        return rValue;
    }
    for (int i = 0; i < listOfUsers.size(); i++) {
        if (listOfUsers[i].name == user){
            bool exists = false;
            for (int j = 0; j < listOfUsers[i].usersConnectedTo.size(); j++) {
                if (listOfUsers[i].usersConnectedTo[j] == userToJoin) {
                    exists = true;
                }
            }
            if (!exists) {
                listOfUsers[i].usersConnectedTo.push_back(userToJoin);
                string rValue = "Added " + user + " to " + userToJoin + " successfully\n";
                return rValue;
            }
        }
    }
    string rValue = "ERROR: could not join " + userToJoin + "'s chat room on " + user + "'s account\n";
    return rValue;
}
 
// take a specified user and remove another specified user from their list of connected users
string leaveCommand(string user, string userToLeave) {
    if (!userExists(userToLeave)) {
        string rValue = "ERROR: " + userToLeave + " does not exist\n";
        return rValue;
    }
    // iterate through users to see if the current user exists
    for (int i = 0; i < listOfUsers.size(); i++) {
        if (listOfUsers[i].name == user) {
            // user exists, check to see if the user
            for (int j = 0; j < listOfUsers[i].usersConnectedTo.size(); j++) {
                if (listOfUsers[i].usersConnectedTo[j] == userToLeave) {
                    listOfUsers[i].usersConnectedTo.erase(listOfUsers[i].usersConnectedTo.begin()+j);
                    string rValue = "Left " + userToLeave + "'s chat room on " + user + "'s account successfully\n";
                    return rValue;
                }
            }
        }
    }
    string rValue = "ERROR: could not leave " + userToLeave + "'s chat room on " + user + "'s account\n";
    return rValue;
}
 
// takes the chat sent from the specified user and appends it to their chat database file
void saveChatToFile(string chat) {
    ofstream outfile;
   
    outfile.open("chathistory.txt", ios_base::app);
    outfile << chat << endl;
    outfile.close();
}

vector<string> readInUserChats(vector<string> users) {
    string line;
	ifstream inputFile;
    vector<string> rValue;
    
    inputFile.open("chathistory.txt");

	while (getline(inputFile, line)) {
        //formatted in user, time, chat
        vector<string> formatted = split(line, '|');
        for(int i = 0; i < users.size(); i++){
            if (users[i] == formatted[0]) {
                rValue.push_back(line);
            }
        }
    }
    inputFile.close();
    return rValue;
}

void writeUserDataToFile() {
    ofstream outfile;
   
    outfile.open("userdata.txt", ofstream::out | ofstream::trunc);
    for (int i = 0; i < listOfUsers.size(); i++) {
        string userDataText = listOfUsers[i].name + "|";
        for (int j = 0; j < listOfUsers[i].usersConnectedTo.size(); j++) {
            userDataText += listOfUsers[i].usersConnectedTo[j] + "|";
        }
        outfile << userDataText << endl;
    }
    outfile.close();
}
 
void readUserDataFromFile() {
    string line;
    ifstream inputFile;
    vector<UserData> rValue;
   
    inputFile.open("userdata.txt");
 
    while (getline(inputFile, line)) {
        UserData newUser;
        vector<string> formatted = split(line, '|');
        for (int i = 0; i < formatted.size(); i++) {
            if (i == 0) {
                newUser.name = formatted[i];
            } else {
                newUser.usersConnectedTo.push_back(formatted[i]);
            }
        }
        rValue.push_back(newUser);
    }
    listOfUsers = rValue;
    inputFile.close();
}
 
string lastTwentyChats(string user) {
    string rValue = "";
    int userIndex = -1;
    for (int i = 0; i < listOfUsers.size(); i++) {
        if (listOfUsers[i].name == user) {
            userIndex = i;
        }
    }
    vector<string> totalRelevantChats = readInUserChats(listOfUsers[userIndex].usersConnectedTo);
    vector<string> reversed;
    for (int i = (totalRelevantChats.size()-1); i >= 0; i--) {
        reversed.push_back(totalRelevantChats[i]);
    }
    for (int i = 19; i >= 0; i--) {
        if (i > reversed.size() - 1) {
            i = reversed.size() - 1;
        }
        vector<string> formatted = split(reversed[i], '|');
        rValue += "[" + formatted[1] + "]<" + formatted[0] + "> " + formatted[2] + "\n";
    }
    return rValue;
}
 
class facebookServer final : public fbChatRoom::Service {
public:
    facebookServer() {}
   
    // process client Login command
    Status Login(ServerContext* context, const LoginRequest* request,
                 LoginReply* reply) override {
        if (!userExists(request->username())) {
            UserData newUser = UserData(request->username());
            listOfUsers.push_back(newUser);
            writeUserDataToFile();
        }
        reply->set_reply("Welcome, " + request->username() + "\n");
        return Status::OK;
    }
   
    // process client List command
    Status List(ServerContext* context, const ListRequest* request,
                ListReply* reply) override {
        reply->set_reply(listCommand() + "\n");
        return Status::OK;
    }
   
    // process client Leave command
    Status Leave(ServerContext* context, const LeaveRequest* request,
                 LeaveReply* reply) override {
        reply->set_reply(leaveCommand(request->username(), request->chatroom()));
        writeUserDataToFile();
        return Status::OK;
    }
   
    // process client Join command
    Status Join(ServerContext* context, const JoinRequest* request,
                JoinReply* reply) override {
        reply->set_reply(joinCommand(request->username(), request->chatroom()));
        writeUserDataToFile();
        return Status::OK;
    }
   
    // process client Chat command
    Status Chat(ServerContext* context, const ChatRequest* request,
                ChatReply* reply) override {
        reply->set_message(lastTwentyChats(request->username()));
        return Status::OK;
    }
    
    Status SendChatToServer(ServerContext* context, const ChatMessage* chatMessage,
                ChatReply* reply) override {
        // get current time and date
        string time = getDateAndTime();
        
        // get username for client who send message
        string clientUsername = chatMessage->username();
            
        // save new chat to file
        string dataForTextFile = clientUsername + "|" + time + "|" + chatMessage->message();
        saveChatToFile(dataForTextFile);
        
        // format message for other clients
        string formatedMessage = "[" + time + "]<" + clientUsername + 
            "> " + chatMessage->message();

        // search through users to find all users connected 
        // to client that sent the message and add the new
        // message to their message queue
        for (int i=0; i<listOfUsers.size(); i++) {
            for (int j=0; j<listOfUsers[i].usersConnectedTo.size(); j++) {
                if(clientUsername == listOfUsers[i].usersConnectedTo[j]) {
                    listOfUsers[i].messagesToWrite.push(formatedMessage);
                    cout << "Pushing message to " << listOfUsers[i].name << "\n";
                }
            }
        }
        return Status::OK;
    }
    
    Status SendChatToClient (ServerContext* context, const ChatRequest* request,
                ChatReply* reply) override {
        
        // get username for client who sent chat request
        string clientUsername = request->username();
        
        // find client in list of users
        for (int i=0; i<listOfUsers.size(); i++) {
            if(clientUsername == listOfUsers[i].name) {
                
                // check client's message queue for a message to write to the client
                if(listOfUsers[i].messagesToWrite.size() > 0) {
                    
                    // pop top message from client's queue
                    string message = listOfUsers[i].messagesToWrite.front();
                    listOfUsers[i].messagesToWrite.pop();

                    cout << "trying to send message to client\n";
                    
                    // send popped message to client
                    reply->set_message(message);
                }
                else {
                    // if message queue is empty, send blank message to client
                    reply->set_message("");
                }
            }
        }
        return Status::OK;
    }
    
};
 
void startServer(string portNumber) {
    // create facebookServer object
    facebookServer facebookServer;
   
    // gRPC class object to create a gRPC server
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
   
    readUserDataFromFile();
    startServer(portNumber);
   
    return 0;
}
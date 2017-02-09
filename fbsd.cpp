// Homework 2
// fbsd.cpp
// Facebook Chat Room Server
// Colin Banigan and Katherine Drake
 
#include <fstream>
#include <iostream>
#include "UserData.h"
#include <sstream>
#include <string>
 
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
using facebookChat::ChatMessage;
using facebookChat::ChatReply;
 
using namespace std;
 
vector<UserData> listOfUsers; //global list of all the connected clients

// returns current date and time in a string
// time format: DD-MM-YYYY-HH:MM:SS
string getDateAndTime() {
    // get current date and time
    time_t t = time(nullptr);
    struct tm* time = localtime(&t);
    
    // convert time to char[]
    char buffer[100];
    strftime(buffer,100,"%d-%m-%Y-%H:%M:%S",time);
    
    // convert time into a string
    string dateAndTime(buffer);
    
    return dateAndTime;
}

bool userExists(string user){
    for(int i = 0; i < listOfUsers.size(); i++){
        if(listOfUsers[i].name == user){
            return true;
        }
    }
    return false;
}

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

//send all of the users and their connected chat rooms back to the specified user
string listCommand(){
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
   
    return totalList;
}
 
//take a specified user and add another specified user to their list of connected users
string joinCommand(string user, string userToJoin){
    if(!userExists(userToJoin)){
        string rValue = "ERROR: " + userToJoin + " does not exist\n";
        return rValue;
    }
    for(int i = 0; i < listOfUsers.size(); i++){
        if(listOfUsers[i].name == user){
            bool exists = false;
            for(int j = 0; j < listOfUsers[i].usersConnectedTo.size(); j++){
                if(listOfUsers[i].usersConnectedTo[j] == userToJoin) {
                    exists = true;
                }
            }
            if(!exists){
                listOfUsers[i].usersConnectedTo.push_back(userToJoin);
                string rValue = "Added " + user + " to " + userToJoin + " successfully\n";
                return rValue;
            }
        }
    }
    string rValue = "ERROR: could not join " + userToJoin + "'s chat room on " + user + "'s account\n";
    return rValue;
}
 
//take a specified user and remove another specified user from their list of connected users
string leaveCommand(string user, string userToLeave){
    if(!userExists(userToLeave)){
        string rValue = "ERROR: " + userToLeave + " does not exist\n";
        return rValue;
    }
    //iterate through users to see if the current user exists
    for(int i = 0; i < listOfUsers.size(); i++){
        if(listOfUsers[i].name == user){
            //user exists, check to see if the user
            for(int j = 0; j < listOfUsers[i].usersConnectedTo.size(); j++){
                if(listOfUsers[i].usersConnectedTo[j] == userToLeave) {
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
 
//takes the chat sent from the specified user and appends it to their chat database file
void saveChatToFile(string chat){
    ofstream outfile;
   
    outfile.open("chathistory.txt", ios_base::app);
    outfile << chat << endl;
}
 
void chatSend(string user, string chat, string time){
    string formatted = user + "|" + time + "|" + chat;
    saveChatToFile(formatted);
    int counter = 0;
    for(int i = 0; i < listOfUsers.size(); i++){
        for(int j = 0; listOfUsers[i].usersConnectedTo.size(); j++){
            if(listOfUsers[i].usersConnectedTo[j] == user){
                //send formatted to listOfUsers[i].name right here
                counter++;
            }
        }
    }
}

vector<string> readInUserChats(string user){
    string line;
	ifstream inputFile;
    vector<string> rValue;
    
    inputFile.open("chathistory.txt");

	while (getline(inputFile, line)) {
        //formatted in user, time, chat
        vector<string> formatted = split(line, '|');
		if (user == formatted[0]) {
            rValue.push_back(line);
        }
    }
    return rValue;
}

// return true if the date string at lhs is earlier than rhs
bool compareDates(string lhs, string rhs){
    vector<string> getDateLHS = split(lhs, '|');
    vector<string> getDateRHS = split(rhs, '|');
    vector<string> fLHS = split(getDateLHS[1], '-');
    vector<string> fRHS = split(getDateRHS[1], '-');
    if(stoi(fLHS[2]) < stoi(fRHS[2])){ //check year
        return true;
    }
    if(stoi(fLHS[1]) < stoi(fRHS[1])){ //check month
        return true;
    }
    if(stoi(fLHS[0]) < stoi(fRHS[0])){ //check day
        return true;
    }
    if(stoi(fLHS[3]) < stoi(fRHS[3])){ //check hour
        return true;
    }
    if(stoi(fLHS[4]) < stoi(fRHS[4])){ //check minute
        return true;
    }
    if(stoi(fLHS[5]) < stoi(fRHS[5])){ //check second
        return true;
    }
    return false;
}
 
//parses the text file for the 20 most recent chat messages
string lastTwentyChats(string user){
    string rValue;
    
    //find the index of the user specified in listOfUsers
    int userIndex = -1;
    for(int i = 0; i < listOfUsers.size(); i++){
        if(listOfUsers[i].name == user){
            userIndex = i;
        }
    }
    
    //parse the text file for messages from the clients the user is subscribed code
    vector<string> totalRelevantChats;
    for(int i = 0; i < listOfUsers[userIndex].usersConnectedTo.size(); i++){
        vector<string> usersChats = readInUserChats(listOfUsers[userIndex].usersConnectedTo[i]);
        for(int j = 0; j < usersChats.size(); j++){
            totalRelevantChats.push_back(usersChats[j]);
        }
    }
    
    //sort the messages retrieved by most recent
    sort(totalRelevantChats.begin(), totalRelevantChats.end(), compareDates);
    
    //return the 20 most recent chat messages
    for(int i = 0; i < totalRelevantChats.size() && i < 20; i++){
        vector<string> formatted = split(totalRelevantChats[i], '|');
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
        cout << "Server in Login function\n";
        if(!userExists(request->username())){
            UserData newUser = UserData(request->username());
            listOfUsers.push_back(newUser);
        }
        reply->set_reply("Welcome, " + request->username() + "\n");
        return Status::OK;
    }
   
    // process client List command
    Status List(ServerContext* context, const ListRequest* request,
                ListReply* reply) override {
        cout << "Server in List function\n";
        reply->set_reply(listCommand() + "\n");
        return Status::OK;
    }
   
    // process client Leave command
    Status Leave(ServerContext* context, const LeaveRequest* request,
                 LeaveReply* reply) override {
        cout << "Server in Leave function\n";
        reply->set_reply(leaveCommand(request->username(), request->chatroom()));
        return Status::OK;
    }
   
    // process client Join command
    Status Join(ServerContext* context, const JoinRequest* request,
                JoinReply* reply) override {
        cout << "Server in Join function\n";
        reply->set_reply(joinCommand(request->username(), request->chatroom()));
        return Status::OK;
    }
   
    // process client Chat command
    Status Chat(ServerContext* context, const ChatRequest* request,
                ChatReply* reply) override {
        cout << "Server in Chat function\n";
        reply->set_reply(lastTwentyChats(request->username()));
        return Status::OK;
    }
    
    //process client ChatStream command
    Status ChatStream(ServerContext* context, ServerReaderWriter<ChatMessage, ChatMessage>* stream) override {
        
        cout << "Server in ChatStream function\n";
        
        thread reader([stream]() {
            ChatMessage msg;
            while (stream->Read(&msg)) {
                cout << "Message Received: " << msg.message() << "\n\n";

                ChatMessage reply;
                string time = getDateAndTime();

                chatSend(msg.username(), msg.message(), time);

                reply.set_username(msg.username());
                reply.set_message(msg.message());
                reply.set_datetime(time);

                stream->Write(reply);
            } 
        });
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
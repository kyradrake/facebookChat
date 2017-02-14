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

// helper function to check if a user exists 
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
    
    // empty string to return with the list data
    string totalList = "";
    
    // iterate through all of the users
    for (int i = 0; i < listOfUsers.size(); i++) {
        
        // add the current username and the users connected of the user in the for loop
        totalList += "User: " + listOfUsers[i].name + "\n";
        totalList += "Users connected: [";
        
        // add the users connected to the string, formatted with a comma if not the last user in the vector
        for (int j = 0; j < listOfUsers[i].usersConnectedTo.size(); j++) {
            totalList += listOfUsers[i].usersConnectedTo[j];
            if (j != listOfUsers[i].usersConnectedTo.size() - 1) {
                totalList += ", ";
            }
        }
        
        // formatting to look nice
        totalList += "]\n";
        totalList += "-------------------------------------------------------------\n";
    }
    return totalList;
}
 
// take a specified user and add another specified user to their list of connected users
string joinCommand(string user, string userToJoin) {
    
    // check to see if user exists
    if (!userExists(userToJoin)) {
        string rValue = "ERROR: " + userToJoin + " does not exist\n";
        return rValue;
    }
    
    // iterate through all of the users
    for (int i = 0; i < listOfUsers.size(); i++) {
        
        // check if we're on the correct set of user data
        if (listOfUsers[i].name == user) {
            
            // loop through the users previously joined to see if we've already joined the specified user
            bool exists = false;
            for (int j = 0; j < listOfUsers[i].usersConnectedTo.size(); j++) {
                if (listOfUsers[i].usersConnectedTo[j] == userToJoin) {
                    exists = true;
                }
            }
            
            // if we haven't already joined, join the specified user
            if (!exists) {
                listOfUsers[i].usersConnectedTo.push_back(userToJoin);
                string rValue = "Added " + user + " to " + userToJoin + " successfully\n";
                return rValue;
            }
        }
    }
    
    // couldn't join the user, print out error statement for client
    string rValue = "ERROR: could not join " + userToJoin + "'s chat room on " + user + "'s account\n";
    return rValue;
}
 
// take a specified user and remove another specified user from their list of connected users
string leaveCommand(string user, string userToLeave) {
    
    // check to see if user exists
    if (!userExists(userToLeave)) {
        string rValue = "ERROR: " + userToLeave + " does not exist\n";
        return rValue;
    }
    
    // iterate through users to see if the current user exists
    for (int i = 0; i < listOfUsers.size(); i++) {
        if (listOfUsers[i].name == user) {
            
            // user exists, check to see if the user is already connected
            for (int j = 0; j < listOfUsers[i].usersConnectedTo.size(); j++) {
                
                // if the user is already connected, remove them from the list of users connected
                if (listOfUsers[i].usersConnectedTo[j] == userToLeave) {
                    listOfUsers[i].usersConnectedTo.erase(listOfUsers[i].usersConnectedTo.begin()+j);
                    string rValue = "Left " + userToLeave + "'s chat room on " + user + "'s account successfully\n";
                    return rValue;
                }
            }
        }
    }
    
    // couldn't leave the user, print out error statement for client
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

// reads in the messages from the users specified in the argument
vector<string> readInUserChats(vector<string> users) {
    string line;
	ifstream inputFile;
    vector<string> rValue;
    
    inputFile.open("chathistory.txt");

    // read in all the lines of chathistory.txt
	while (getline(inputFile, line)) {
        
        // split the data into a vector of strings in the order of user, time, chat
        vector<string> formatted = split(line, '|');
        
        for (int i = 0; i < users.size(); i++) {
            
            // if the user in the message was in the arguments, save the message
            if (users[i] == formatted[0]) {
                rValue.push_back(line);
            }
        }
    }
    
    inputFile.close();
    return rValue;
}

// writes all of the saved user data in listOfUsers to the file
void writeUserDataToFile() {
    ofstream outfile;
   
    // open userdata.txt at the very beginning of the file, prepare to overwrite
    outfile.open("userdata.txt", ofstream::out | ofstream::trunc);
    
    // for each line, write the username and the list of users connected
    for (int i = 0; i < listOfUsers.size(); i++) {
        string userDataText = listOfUsers[i].name + "|";
        for (int j = 0; j < listOfUsers[i].usersConnectedTo.size(); j++) {
            userDataText += listOfUsers[i].usersConnectedTo[j] + "|";
        }
        outfile << userDataText << endl;
    }
    outfile.close();
}

// reads in all of the saved user data in userdata.txt and reloads the global data structure
void readUserDataFromFile() {
    string line;
    ifstream inputFile;
    vector<UserData> rValue;
   
    inputFile.open("userdata.txt");
 
    // read in all the lines of userdata.txt
    while (getline(inputFile, line)) {
        
        // create a new user
        UserData newUser;
        
        // format the line read in into username and the users subscribed to
        vector<string> formatted = split(line, '|');
        for (int i = 0; i < formatted.size(); i++) {
            
            // if we're on the first username, set that as the name. Otherwise, it's a user connected to
            if (i == 0) {
                newUser.name = formatted[i];
            } else {
                newUser.usersConnectedTo.push_back(formatted[i]);
            }
        }
        
        // pushback the new user into the vector
        rValue.push_back(newUser);
    }
    
    //set listOfUsers to the data read in from the text file
    listOfUsers = rValue;
    inputFile.close();
}

// prints out the most recent twenty chats of the users the specified user is connected to
string lastTwentyChats(string user) {
    string rValue = "";
    int userIndex = -1;
    
    // find the index of the user specified
    for (int i = 0; i < listOfUsers.size(); i++) {
        if (listOfUsers[i].name == user) {
            userIndex = i;
        }
    }
    
    // read in the relevant chats from the chat history
    vector<string> totalRelevantChats = readInUserChats(listOfUsers[userIndex].usersConnectedTo);
    
    // reverse the data read in to start from the most recent first
    vector<string> reversed;
    for (int i = (totalRelevantChats.size()-1); i >= 0; i--) {
        reversed.push_back(totalRelevantChats[i]);
    }
    
    // if reversed has no elements, return immediately
    if(reversed.size() == 0){
        return rValue;
    }
    
    // format the most recent data into a string in message format
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
        
        for (int i = 0; i < listOfUsers.size(); i++) {
            if (listOfUsers[i].name == request->username()) {
                // clear user's queue
                queue<string> empty;
                swap(listOfUsers[i].messagesToWrite, empty);
            }
        }
        
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
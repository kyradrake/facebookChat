#include <iostream>
#include <queue>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

using namespace std;

class UserData {
public:
    string name;
    vector<string> usersConnectedTo;
    queue<string> messagesToWrite;
    
    UserData(void){
        name = "";
        usersConnectedTo = vector<string>();
        messagesToWrite = queue<string>();
    }
    
    UserData(string user){
        name = user;
        usersConnectedTo = vector<string>();
        messagesToWrite = queue<string>();
    }
    
    void* addUser(string userToAdd) {
        usersConnectedTo.push_back(userToAdd);
    }
};
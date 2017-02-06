#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

class UserData {
public:
    string name;
    vector<string> usersConnectedTo;
    
    UserData(void){
        name = "";
        usersConnectedTo = vector<string>;
    }
    
    UserData(string user){
        name = user;
        usersConnectedTo = vector<string>;
    }
    
    void* addUser(string userToAdd) {
        usersConnectedTo.push_back(userToAdd);
    }
};
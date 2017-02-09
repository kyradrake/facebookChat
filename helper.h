// Homework 2
// helper.h
// Helper Functions for Facebook Chat
// Colin Banigan and Katherine Drake

#include <ctime>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>

#include "facebook.grpc.pb.h"

using namespace std;

// string split functions below
// splits a string into a vector of strings be delimeter
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
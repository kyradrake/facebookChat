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
// splits a string into a vector of strings by delimeter
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
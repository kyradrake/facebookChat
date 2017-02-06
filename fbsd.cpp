// Homework 2
// fbsd.cpp
// Facebook Chat Room Server
// Colin Banigan and Katherine Drake

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
using facebookChat::ListRequest;
using facebookChat::ListReply;
using facebookChat::JoinRequest;
using facebookChat::JoinReply;
using facebookChat::LeaveRequest;
using facebookChat::LeaveReply;

using namespace std;

class facebookServer final : public fbChatRoom::Service {
    public:
    facebookServer() {}
    
    // process client JOIN command
    Status ClientJoin(ServerContext* context, const JoinRequest* request,
                     JoinReply* reply) {
        reply->set_name("Joined Chat Room " + request->name());
        return Status::OK;
    }
};

void startServer(string portNumber) {
    // create facebookServer object
    facebookServer facebookServer();
    
    // gRPC class object to create a gRPC server
    // man page for ServerBuilder: http://www.grpc.io/grpc/cpp/classgrpc_1_1_server_builder.html#a0b06b5828b892feeb6541c8eeae2d542
    ServerBuilder serverBuilder;
    
    // binds server to localhost address
    serverBuilder.AddListeningPort("localhost:" + portNumber, grpc::InsecureServerCredentials());
    
    // return a running server that is ready to process calls
    unique_ptr<Server> server(serverBuilder.BuildAndStart());
    
    cout << "Server is running on: " << "localhost:" + portNumber << endl;
    
    // server waits until killed
    server->Wait();
}

int main(int argc, char* argv[]) {
    string portNumber = "16230";
    if (argc >= 2) {
        portNumber = argv[1];
    }

    startServer(portNumber);
    
    return 0;
}

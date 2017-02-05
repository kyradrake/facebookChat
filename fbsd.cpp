// Homework 2
// fbsd.cpp
// Facebook Chat Room Server
// Colin Banigan and Katherine Drake

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using facebook::ListRequest;
using facebook::ListReply;
using facebook::JoinRequest;
using facebook::JoinReply;
using facebook::LeaveRequest;
using facebook::LeaveReply;

void startServer(string portNumber) {
    
    ServerBuilder serverBuilder;
    builder.AddListeningPort("127.0.0.1", grpc::InsecureServerCredentials())
    
        /*
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
  */
}



int main(int argc, char* argv[]) {
    string portNumber;
    if (argc != 2) {
        portNumber = "16230";
    }
    else {
        portNumber = argv[1];
    }

    return 0;
}

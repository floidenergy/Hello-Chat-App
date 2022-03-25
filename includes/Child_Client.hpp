#pragma once
#include <jy_jai.hpp>
#include <fstream>

class App;

class Client : public jai::net::Client_interface<Message_id>{
    //std::vector<jai::net::User> FriendsClient;
    std::thread th_Context;
    
    int numOfConnection;

    App *app;

public:
    Client(App *___app);

    Client(std::string ip);

    virtual void OnMessageRecieve();

    void start_Rcv();
    

    ~Client();
};
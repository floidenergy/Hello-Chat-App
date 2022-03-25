#include <jy_jai.hpp>
#include <fstream>
//#include <asio.hpp>
//server

// TODO find a way inform clients how many ppl are connected

class _Server : public jai::net::Server<Message_id>{

    std::vector<std::string> Conversation;
    std::mutex InitMutex;
    std::fstream LogFile;

    int IDS;

public:

    _Server() : jai::net::Server<Message_id>() {
        IDS = 0;
        LogFile.open("Server_Log.txt", std::ios::in );

        if(!LogFile){
            std::cout << "creating log file" << std::endl;
            LogFile.open("Server_Log.txt",std::ios::out | std::ios::trunc);
        }else{
            
            LogFile.ignore();
            while(!LogFile.eof()){
                std::string msg;
                std::getline(LogFile, msg);
                Conversation.push_back(msg);
                std::cout << msg << std::endl;
            }
            LogFile << std::endl;
            LogFile.close();

            LogFile.open("Server_Log.txt", std::ios::out | std::ios::app);
        }
    } 

    virtual void IncomingMessage(std::shared_ptr<jai::net::Connection<Message_id>> Cn){
        switch(Cn->Data.Header.ID){
            case Message_id::RecieveMessege:{

                std::string msg;
                Cn->Data >> msg;
                msg = (std::string(Cn->UserInfo.name) + ": " + msg);

                ArchiveConvo(msg);

                Cn->Data << msg;
                this->TextClients(Cn->Data, Cn);

                break;
            }
            
        }
        AddClientToQueue(Cn);
    }

    virtual void OnClientDisconnect(std::shared_ptr<jai::net::Connection<Message_id>> Cn){
        //std::string msgString(std::string(Cn->UserInfo.name));
        this->Disconnect_Client(Cn);
        std::cout << "[" << Cn->UserInfo.GetUserName() << "] Has Been Disconnected :- " << Connections.size() << " Clients Remaining" << std::endl;
        jai::net::Payload<Message_id> DisconnectedClientInfo;
        DisconnectedClientInfo.Header.ID = Message_id::Disconnect;
        DisconnectedClientInfo << Cn->UserInfo;
        TextClients(DisconnectedClientInfo, Cn);
        ArchiveConvo(std::string(Cn->UserInfo.GetUserName() + " Has Been Disconnected :- " + std::to_string(Connections.size()) + " Clients Remaining"));
    }

    virtual void OnClientConnect(std::shared_ptr<jai::net::Connection<Message_id>> Cn){

        asio::error_code ec;

        // Init Client on the server
        Cn->Recieve();
        {
            jai::net::User tempUser;
            Cn->Data >> tempUser;
            IDS++;
            Cn->SetUserInfo(tempUser.name, IDS);
            Cn->Data << Cn->UserInfo.ID;
            asio::error_code ec;
            Cn->Send(ec);
        }
        
        std::cout << "[" << std::string(Cn->UserInfo.name) << "#" << Cn->UserInfo.ID << "] Has Been Connected : " << Connections.size() << " Clients Remaining" << std::endl;
        
        // Notify other clients that the new client is connected now
        jai::net::Payload<Message_id> UserData;
        UserData.Header.ID = Message_id::ClientConnected;
        UserData << Cn->UserInfo;
        TextClients(UserData, Cn);

        // Sending Connected Clients Data to The Newest Client
        for(std::shared_ptr<jai::net::Connection<Message_id>> _Cn : Connections){
            jai::net::Payload<Message_id> msg;
            msg.Header.ID = Message_id::Init_NewClient;
            msg << _Cn->UserInfo;
            asio::error_code ec;
            Cn->Send(msg, ec);
        }

        // sendig the whole conversation to the new client
        for(std::string _msg : Conversation){
            Cn->Data << _msg;
            Cn->Data.Header.ID = Message_id::RecieveMessege;
            Cn->Send(ec);
        }

        // Adding to Coversation history
        ArchiveConvo(std::string(std::string(Cn->UserInfo.name) + " Has Been Connected : " + std::to_string(Connections.size()) + " Clients Remaining"));

        AddClientToQueue(Cn);
    }

    void ArchiveConvo(std::string msg){
        LogFile << msg << std::endl;
        Conversation.push_back(msg); 
    }
};

int main(){
    system("MODE CON COLS=53 LINES=30");
    _Server Station;
    Station.Start();
    Station.run();
    return 0;
}
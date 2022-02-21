#include <jy_jai.hpp>
#include <fstream>
//#include <asio.hpp>
//server

// TODO Fix to get Notifiy other client that someone has connected
// TODO find a way inform clients how many ppl are connected

class _Server : public jai::net::Server<Message_id>{

    std::vector<std::string> Conversation;
    std::mutex InitMutex;
    std::fstream LogFile;

public:

    _Server() : jai::net::Server<Message_id>() {
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
        std::string msgString(std::string(Cn->UserInfo.name));
        this->Disconnect_Client(Cn);
        std::cout << "[" << Cn->UserInfo.name << "] Has Been Disconnected :- " << Connections.size() << " Clients Remaining" << std::endl;
        jai::net::Payload<Message_id> msg;
        msg.Header.ID = Message_id::Disconnect;
        msg << msgString;
        TextClients(msg, Cn);
        ArchiveConvo(std::string(std::string(Cn->UserInfo.name) + " Has Been Disconnected :- " + std::to_string(Connections.size()) + " Clients Remaining"));
    }

    virtual void OnClientConnect(std::shared_ptr<jai::net::Connection<Message_id>> Cn){

        asio::error_code ec;

        // Init Client
        Cn->Recieve();

        {
            jai::net::User tempUser;
            Cn->Data >> tempUser;
            Cn->SetUserInfo(tempUser.name);
        }
        
        std::cout << "[" << std::string(Cn->UserInfo.name) << "] Has Been Connected : " << Connections.size() << " Clients Remaining" << std::endl;
        
        // Notify other clients that the new client is connected now
        jai::net::Payload<Message_id> pay;
        pay.Header.ID = Message_id::ClientConnected;
        std::string msg(std::string(Cn->UserInfo.name));
        pay << msg;
        
        TextClients(pay, Cn);

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
#include <jy_jai.hpp>
#include <payload.hpp>
#include <typeinfo>
#include <fstream>


struct Client : public jai::net::Client_interface<Message_id>{
    std::thread th_Context;
public:
    Client() = default;

    Client(std::string ip)
        : jai::net::Client_interface<Message_id>(ip){
            
        }

    virtual void OnMessageRecieve(){
        std::string txt;
        switch(_Connection.Data.Header.ID){

            case Message_id::ClientConnected:{
                    std::string UserName;
                    this->_Connection.Data >> UserName;
                    std::cout << UserName << " Has Been Connected!" << std::endl;
                break;
            }

            case Message_id::RecieveMessege:{
                _Connection.Data >> txt;
                std::cout << txt << std::endl;
                break;
            }

            case Message_id::Disconnect:{
                std::string UserName;
                this->_Connection.Data >> UserName;
                std::cout << UserName << " Has Been Disconnected!" << std::endl;
                break;
            }
        }
        
        //async_read();
    }

    void start_Rcv(){
        
        asio::post(this->Context, [this](){
                while(true){
                    this->_Connection.Recieve();
                    this->OnMessageRecieve();
                }
            });

        th_Context = std::thread([this](){Context.run();});
    }

    ~Client(){
        if(th_Context.joinable())
    
            th_Context.join();
    }
};

//client
int main(){
    std::fstream UserFile("profil.txt", std::ios::in | std::ios::out);
    if(!UserFile.is_open()){
        UserFile.open("profil.txt", std::ios::out);
        std::cout << "404! profil.txt not found\n We Create the file Please Open \"profil.txt\" and save the file with the following information: " << std::endl
                    << "-1 Server Address\n-2 Username" << std::endl;
        system("pause");
        return EXIT_FAILURE;
    }

    system("MODE CON COLS=53 LINES=30");
    //system("MODE CON COLS=30 LINES=30");
    std::string ipAddress;
    std::string Username;
    UserFile >> ipAddress;
    UserFile >> Username;

    if(UserFile.fail()){
        std::cout << "no info found on \"Profil.txt\"" << std::endl;
        std::cout << "Please Open \"profil.txt\" and save the file with the following information: " << std::endl
                    << "-1 Server Address\n-2 Username" << std::endl;
        system("pause");
        return EXIT_FAILURE;
    }

    std::cout << "***Chat App Made by FloidEnergy (Rika)***" << std::endl;
    std::cout << "Welcome: " << Username << std::endl;

    Client _Client(ipAddress);
    
    std::cout << "Trying to connect to [" << ipAddress << "]" << std::endl;
    if(!_Client.Connect()){
        std::cout << "couldn't connect!" << std::endl;
        std::cin.ignore();
        return EXIT_FAILURE;
    }
    
    std::cout << "Connected" << std::endl;

    _Client._Connection.SetUserInfo(Username);

    {
        jai::net::Payload<Message_id> initMsg;
        initMsg << _Client._Connection.UserInfo;
        _Client.Send(initMsg);
    }

    _Client.start_Rcv();

    while(true){
        jai::net::Payload<Message_id> package;
        package.Header.ID = Message_id::RecieveMessege;
        std::string message;
        std::getline(std::cin, message);
//        std::cin >> message;
        package << message;
        _Client.Send(package);
    }


    return EXIT_SUCCESS;
}
















// OLDDD

/*
int main(){
    asio::io_context Context;
    asio::ip::tcp::endpoint ep(asio::ip::make_address("127.0.0.1"), 6250);
    asio::ip::tcp::socket Soc(Context);

    Soc.connect(ep);

    jai::net::Payload<Message_id> p;
    std::string name;
    while(Soc.is_open()){
        std::cout << "=> ";
        std::getline(std::cin, name);
        p << name;

        Soc.write_some(asio::buffer(&p.Header, sizeof(jai::net::MessageHeader<Message_id>)));

        Soc.write_some(asio::buffer(p.Body.data(), p.Header.size));
        name.clear();
    }
    system("pause");
    return 0;
}
*/
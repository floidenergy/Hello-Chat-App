#include <Gui.hpp>

#include <jy_jai.hpp>


wxIMPLEMENT_APP(App);




/*
//client
int main_____(){
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
*/
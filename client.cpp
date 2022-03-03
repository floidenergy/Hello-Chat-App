
#include <jy_jai.hpp>
#include <payload.hpp>
#include <typeinfo>
#include <fstream>

// wx library
#include <wx/wx.h>
#include <wx/list.h>
#include <wx/listctrl.h>
#include <wx/splitter.h>

class Frame : public wxFrame{

    wxListBox* ChatBox;
    wxListView* ClientsDisplay;
    wxTextCtrl* UserNameInput;
    wxTextCtrl* AddressInput;
    wxButton* ConnectButton;
    wxButton* ExitButton;
    wxTextCtrl* msgBox;
    wxButton* SendButton;

public:
    Frame(wxWindowID _ID = wxID_ANY, std::string _Title = "Window", wxPoint _Position = wxDefaultPosition, wxSize _Size = wxDefaultSize )
    : wxFrame(nullptr, _ID, _Title, _Position, _Size, wxNO_BORDER){
 
        wxStaticText* UserNameLable = new wxStaticText(this, wxID_ANY, "Username", wxDefaultPosition, wxSize(69, 20));
        UserNameInput = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(116, 21), wxNO_BORDER);
        UserNameInput->SetBackgroundColour(wxColor(255,255,255));

        wxStaticText* AddressLable = new wxStaticText(this, wxID_ANY, "Address", wxDefaultPosition, wxSize(53, 20));
        AddressInput = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(141, 21), wxNO_BORDER);
        AddressInput->SetBackgroundColour(wxColor(255,255,255));

        ConnectButton = new wxButton(this, wxID_ANY, "Connect", wxDefaultPosition, wxSize(75, 21), wxNO_BORDER);
        ConnectButton->SetBackgroundColour(wxColor(255, 255, 255));
        ExitButton = new wxButton(this, wxID_ANY, "Exit", wxDefaultPosition, wxSize(75, 21), wxNO_BORDER);
        ExitButton->SetBackgroundColour(wxColor(255, 255, 255));
        ExitButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent &e){
            this->Close();
        });

        wxSizer* Settings_Sizer = new wxBoxSizer(wxHORIZONTAL);
        Settings_Sizer->Add(UserNameLable, 0);
        Settings_Sizer->Add(UserNameInput, 0, wxLEFT, 10);
        Settings_Sizer->Add(AddressLable, 0, wxLEFT, 10);
        Settings_Sizer->Add(AddressInput, 0, wxLEFT, 10);
        Settings_Sizer->Add(ConnectButton, 0, wxLEFT, 10);
        Settings_Sizer->Add(ExitButton, 0, wxLEFT , 10);

//#########################################################################################

        ChatBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(400, 321));
        ChatBox->SetBackgroundColour(wxColor(255, 255, 255));
        ChatBox->SetExtraStyle(wxNO_BORDER);

        ClientsDisplay = new wxListView(this, wxID_ANY, wxDefaultPosition, wxSize(169, 321));
        ClientsDisplay->SetBackgroundColour(wxColor(255, 255, 255));
        ClientsDisplay->AppendColumn("ID");
        ClientsDisplay->AppendColumn("UserName");
        ClientsDisplay->SetColumnWidth(0, 50);
        ClientsDisplay->SetColumnWidth(1, 150);

        wxSizer* OutputSizer = new wxBoxSizer(wxHORIZONTAL);
        OutputSizer->Add(ChatBox, 0);
        OutputSizer->Add(ClientsDisplay, 0, wxLEFT, 10);

//#########################################################################################
        
        msgBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(464, 21), wxNO_BORDER);
        msgBox->SetBackgroundColour(wxColor(255, 255, 255));
        
        SendButton = new wxButton(this, wxID_ANY, "Send", wxDefaultPosition, wxSize(107, 21), wxNO_BORDER);
        SendButton->SetBackgroundColour(wxColor(255, 255, 255));
        
        wxSizer* ControlSizer = new wxBoxSizer(wxHORIZONTAL);
        ControlSizer->Add(msgBox, 0);
        ControlSizer->Add(SendButton, 0, wxLEFT, 10);
        
//#########################################################################################

        wxSizer* MainSizer = new wxBoxSizer(wxVERTICAL);
        MainSizer->Add(Settings_Sizer, 0, wxALL, 10);
        MainSizer->Add(OutputSizer, 0, wxLEFT | wxRIGHT, 10);
        MainSizer->Add(ControlSizer, 0, wxALL, 10);

        this->SetSizerAndFit(MainSizer);
    }

    ~Frame(){}
};

class App : public wxApp{
    
public:
    App(){}
    ~App(){}

    virtual bool OnInit(){
        Frame *frame = new Frame(wxID_ANY, "Jai-jy chat app", wxPoint(50, 50), wxSize(600, 400));
        frame->SetBackgroundColour(wxColor(238, 238, 238));
        frame->Show();

        return true;
    }
};

wxIMPLEMENT_APP(App);


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

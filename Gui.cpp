#include <Gui.hpp>
#include <Child_Client.hpp>

App::App() : _Client(this){}

App::~App(){}

bool App::OnInit(){
    InitWindow();
    Frame->Show();

    ButtonBinding();

    return true;
}

void App::InitWindow(){
    Frame = new wxFrame(nullptr, wxID_ANY, "Jai-jy Chat App", wxDefaultPosition, wxSize(600, 400));

    wxStaticText* UserNameLable = new wxStaticText(Frame, wxID_ANY, "Username", wxDefaultPosition, wxSize(69, 20));
    UserNameInput = new wxTextCtrl(Frame, wxID_ANY, "", wxDefaultPosition, wxSize(116, 21), wxNO_BORDER);
    UserNameInput->SetBackgroundColour(wxColor(255,255,255));
    UserNameInput->SetLabel("User");
    
    wxStaticText* AddressLable = new wxStaticText(Frame, wxID_ANY, "Address", wxDefaultPosition, wxSize(53, 20));
    AddressInput = new wxTextCtrl(Frame, wxID_ANY, "", wxDefaultPosition, wxSize(141, 21), wxNO_BORDER);
    AddressInput->SetBackgroundColour(wxColor(255,255,255));
    AddressInput->SetLabel("127.0.0.1");

    ConnectButton = new wxButton(Frame, wxID_ANY, "Connect", wxDefaultPosition, wxSize(75, 21), wxNO_BORDER);
    ConnectButton->SetBackgroundColour(wxColor(255, 255, 255));
    ExitButton = new wxButton(Frame, wxID_ANY, "Exit", wxDefaultPosition, wxSize(75, 21), wxNO_BORDER);
    ExitButton->SetBackgroundColour(wxColor(255, 255, 255));

    wxSizer* Settings_Sizer = new wxBoxSizer(wxHORIZONTAL);
    Settings_Sizer->Add(UserNameLable, 0);
    Settings_Sizer->Add(UserNameInput, 0, wxLEFT, 10);
    Settings_Sizer->Add(AddressLable, 0, wxLEFT, 10);
    Settings_Sizer->Add(AddressInput, 0, wxLEFT, 10);
    Settings_Sizer->Add(ConnectButton, 0, wxLEFT, 10);
    Settings_Sizer->Add(ExitButton, 0, wxLEFT , 10);

//#########################################################################################

    ChatBox = new wxListBox(Frame, wxID_ANY, wxDefaultPosition, wxSize(400, 321));
    ChatBox->SetBackgroundColour(wxColor(255, 255, 255));
    ChatBox->SetExtraStyle(wxNO_BORDER);

    ClientsDisplay = new wxListView(Frame, wxID_ANY, wxDefaultPosition, wxSize(169, 321));
    ClientsDisplay->SetBackgroundColour(wxColor(255, 255, 255));
    ClientsDisplay->AppendColumn("ID");
    ClientsDisplay->AppendColumn("UserName");
    ClientsDisplay->SetColumnWidth(0, 50);
    ClientsDisplay->SetColumnWidth(1, 150);

    wxSizer* OutputSizer = new wxBoxSizer(wxHORIZONTAL);
    OutputSizer->Add(ChatBox, 0);
    OutputSizer->Add(ClientsDisplay, 0, wxLEFT, 10);

//#########################################################################################
    
    msgBox = new wxTextCtrl(Frame, wxID_ANY, "", wxDefaultPosition, wxSize(464, 21), wxNO_BORDER);
    msgBox->SetBackgroundColour(wxColor(255, 255, 255));
    
    SendButton = new wxButton(Frame, wxID_ANY, "Send", wxDefaultPosition, wxSize(107, 21), wxNO_BORDER);
    SendButton->SetBackgroundColour(wxColor(255, 255, 255));
    
    wxSizer* ControlSizer = new wxBoxSizer(wxHORIZONTAL);
    ControlSizer->Add(msgBox, 0);
    ControlSizer->Add(SendButton, 0, wxLEFT, 10);
    
//#########################################################################################

    wxSizer* MainSizer = new wxBoxSizer(wxVERTICAL);
    MainSizer->Add(Settings_Sizer, 0, wxALL, 10);
    MainSizer->Add(OutputSizer, 0, wxLEFT | wxRIGHT, 10);
    MainSizer->Add(ControlSizer, 0, wxALL, 10);

    Frame->SetSizerAndFit(MainSizer);
}

void App::ButtonBinding(){
    ExitButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent &e){
        Frame->Close();
    });

// TODO : FIX THIS => IT CONNECTS AND DISCONNECT NORMALY BUT IF U TRY TO CONNECT IT AGAIN IT CRUSHES
    this->ConnectButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent &e){

        if(!_Client.isConnected()){
                _Client._Connection.SetUserInfo(this->UserNameInput->GetValue().ToStdString(), 0);
                _Client.SetIpAddress(AddressInput->GetValue().ToStdString());
                std::cout << _Client.ep.address().to_string() << std::endl;
                if(_Client.Connect()){
                    std::cout << "connected" << std::endl;
                    ConnectButton->SetLabel("Disconnect");
                    {
                        jai::net::Payload<Message_id> initMsg;
                        initMsg.Header.ID = Message_id::Init_NewClient;
                        initMsg << _Client._Connection.UserInfo;
                        _Client.Send(initMsg);

                        _Client._Connection.Recieve();
                        int ID;
                        _Client._Connection.Data >> ID;
                        _Client._Connection.UserInfo.ID = ID;
                        std::cout << "id is " << ID << std::endl;
                        std::cout << "ID IS " << _Client._Connection.UserInfo.ID << std::endl;
                    }
                    _Client.start_Rcv();
                }else{
                    std::cout << "fail to connect" << std::endl;
                }
        }else if(_Client.isConnected()){
            _Client.Disconnect();
            ConnectButton->SetLabel("Connect");
        }
    });
}
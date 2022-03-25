#include <Child_Client.hpp>
#include <Gui.hpp>

Client::Client(App* ___app)
    : jai::net::Client_interface<Message_id>(){
    
    this->app = ___app;
    this->numOfConnection = 0;
}

Client::Client(std::string ip)
    : jai::net::Client_interface<Message_id>(ip){
        
    }

void Client::OnMessageRecieve(){
    std::string txt;
    switch(_Connection.Data.Header.ID){
        
        case Message_id::ClientConnected:{
            numOfConnection++;
        
            jai::net::User Friend;
            this->_Connection.Data >> Friend;
            app->ChatBox->AppendString(wxString(std::string(Friend.name) + std::string(" ==> Has Been Connected!")));

            size_t index = app->ClientsDisplay->GetItemCount();
            
            //std::cout << Friend.name << " Has Been Connected!" << std::endl;

            app->ClientsDisplay->InsertItem(index, wxString(std::to_string(Friend.ID)));
            app->ClientsDisplay->SetItem(index, 1, wxString(Friend.name));
            break;
        }

        case Message_id::Init_NewClient:{
            jai::net::User ConnectedClient;
            this->_Connection.Data >> ConnectedClient;

            size_t index = app->ClientsDisplay->GetItemCount();

            app->ClientsDisplay->InsertItem(index, wxString(std::to_string(ConnectedClient.ID)));
            app->ClientsDisplay->SetItem(index, 1, wxString(ConnectedClient.name));
        }

        case Message_id::RecieveMessege:{
            _Connection.Data >> txt;
            app->ChatBox->AppendString(wxString(txt));
            //std::cout << txt << std::endl;
            break;
        }

        case Message_id::Disconnect:{
            //std::string UserName;
            jai::net::User user;
            this->_Connection.Data >> user;
            app->ChatBox->AppendString(wxString(std::string(user.GetUserName() + "Has Been Disconnected")));
            app->ClientsDisplay->DeleteItem(app->ClientsDisplay->FindItem(0, wxString(std::to_string(user.ID))));
            numOfConnection--;
            break;
        }

        default:
            break;
    }
}

void Client::start_Rcv(){

    asio::post(this->Context, [this](){
                while(this->_Connection._IsConnected){
                    this->_Connection.Recieve();
                    this->OnMessageRecieve();

                }
            });

    th_Context = std::thread([this](){Context.run();});
}

Client::~Client(){
    if(th_Context.joinable())
        th_Context.join();

    delete app;
}
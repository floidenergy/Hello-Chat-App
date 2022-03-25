#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/splitter.h>

class App;
#include <Child_Client.hpp>

class App : public wxApp{
public:
    wxFrame* Frame;
    wxListBox* ChatBox;
    wxListView* ClientsDisplay;
    wxTextCtrl* UserNameInput;
    wxTextCtrl* AddressInput;
    wxButton* ConnectButton;
    wxButton* ExitButton;
    wxTextCtrl* msgBox;
    wxButton* SendButton;

    Client _Client;

public:
    App();
    ~App();

    virtual bool OnInit();

    void InitWindow();
    void ButtonBinding();

};
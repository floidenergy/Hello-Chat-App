#pragma once

#include <jy_jai.hpp>
#include <memory>
#include <chrono>
#include <thread>
#include <cctype>

namespace jai{
    namespace net{

        template<typename T>
        class Server{
        
        protected:
            asio::io_context Context;
            asio::ip::tcp::acceptor Acceptor;

            std::vector<std::shared_ptr<Connection<T>>> Connections;
            std::queue<std::shared_ptr<Connection<T>>> c_Queue;
            std::thread recievingThread;

        public:

            Server() : Acceptor(Context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 6250)){}

            ~Server(){}

            void run(){Context.run();}

            void Start(){
                std::cout << "Listing on port: " << PORT << std::endl;
                WaitingForConnection(); 

                asio::io_context::work idleWork(Context);
            }

            // starting to wait for new clients to connect
            void WaitingForConnection(){
                Acceptor.async_accept([this](asio::error_code ec, asio::ip::tcp::socket Soc){
                    Connection<T> temp(std::make_shared<asio::ip::tcp::socket>(std::move(Soc)), Ownership::Server);
                    Connections.push_back(std::make_shared<Connection<T>>(temp));

                    WaitingForConnection();

                    OnClientConnect(Connections.back());
                });
            }

            // starting to wait for new message from clients
            void ListeningForMessages(){                    
                asio::post(Context, [this](){
                    if(!c_Queue.empty()){
                        
                        size_t times = c_Queue.size();
                        for(int i = 0; i < c_Queue.size(); ){
                            try{
                                async_read_header(c_Queue.front());
                            }catch(asio::error_code ec){
                                std::cout << "couldn't recieve message properly" << std::endl;
                                OnClientDisconnect(c_Queue.front());
                            }
                            c_Queue.pop();
                        }
                    }
                });
            }

            // adding client to queue and check if listeningForMessages function was not running if no it will call that function
            void AddClientToQueue(std::shared_ptr<Connection<T>> Connection){
                bool emptyQueue = c_Queue.empty();
                c_Queue.push(Connection);

                if(emptyQueue)
                    ListeningForMessages();
            }

            // text all clients connected to the server / there is an option to avoid one client to not sending him payloads
            void TextClients(Payload<T> msg, std::shared_ptr<Connection<T>> ClientToAvoide = nullptr){
                for(auto _Cn : Connections){
                    asio::error_code ec;
                    if(_Cn != ClientToAvoide){
                        _Cn->Async_Send(msg);
                    }
                }
            }

            // Disconnecting Client and removing it from Connctions container
            void Disconnect_Client(std::shared_ptr<Connection<T>> Cn){
                Connections.erase(std::remove(Connections.begin(), Connections.end(), Cn));
                Cn->Disconnect();
            }

            // Called once message recieved
            virtual void IncomingMessage(std::shared_ptr<Connection<T>> Cn){

            }

            // Called once Client is Disconnected
            virtual void OnClientDisconnect(std::shared_ptr<Connection<T>> Cn){
                
            }
            
            // Called once Client is Connected
            virtual void OnClientConnect(std::shared_ptr<Connection<T>> Cn){

            }


        protected:
            
            // Read Payload Header from a client (Async)
            void async_read_header(std::shared_ptr<Connection<T>> Cn){
                Cn->Socket->async_read_some(asio::buffer(&Cn->Data.Header, sizeof(Cn->Data.Header)),
                    [this, Cn](asio::error_code ec, size_t bytes){
                        if(Cn->Data.Header.size > 0 && bytes > 0 && !ec){
                            async_read_body(Cn);
                        }else{
                            OnClientDisconnect(Cn);
                        }
                    });
            }

            // Read Payload Body from a Client (Async)
            void async_read_body(std::shared_ptr<Connection<T>> Cn){
                Cn->Data.Body.resize(Cn->Data.Header.size);
                Cn->Socket->async_read_some(asio::buffer(Cn->Data.Body.data(), Cn->Data.Header.size),
                    [this, Cn](asio::error_code ec, size_t bytes){
                        if(bytes > 0 && !ec){
                            IncomingMessage(Cn);
                        }else{
                            OnClientDisconnect(Cn);
                        }
                    });
            }

        };

    }
}
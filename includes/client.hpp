#pragma once

#include <jy_jai.hpp>
#include <asio.hpp>

namespace jai{
    namespace net{
        
        template<typename T>
        struct Client_interface{
            asio::io_context Context;
            Connection<T> _Connection;
            Ownership Owner;
            asio::ip::tcp::endpoint ep;

            Client_interface() = default;
            Client_interface(std::string ip) : _Connection(Context, Ownership::Client), ep(asio::ip::make_address(ip), PORT){}

            bool Connect(){
                asio::error_code ec;
                _Connection.Socket->connect(ep, ec);
                if(ec){
                    return false;
                }else{
                    return true;
                }
            }

            void async_read(){
                async_read_header();
            }

            void Send(){
                asio::error_code ec;
                _Connection.Send(ec);
            }

            void Send(Payload<T>& msg){
                asio::error_code ec;
                _Connection.Send(msg, ec);
            }

        protected:

            void async_read_header(){
                _Connection.Socket->async_read_some(asio::buffer(&_Connection.Data.Header, sizeof(_Connection.Data.Header)),
                    [this](asio::error_code ec, size_t bytes){
                        if(!ec){
                            if(_Connection.Data.Header.size)
                                async_read_body();
                            else
                                OnMessageRecieve();
                        }else{
                            std::cout << "Couldn't Send Header" << std::endl;
                            _Connection.Disconnect();
                        }
                    });
            }

            void async_read_body(){
                _Connection.Socket->async_read_some(asio::buffer(_Connection.Data.Body.data(), _Connection.Data.Header.size),
                    [this](asio::error_code ec, size_t bytes){
                        if(!ec){
                            OnMessageRecieve();
                        }else{
                            std::cout << "Couldn't Send Header" << std::endl;
                            _Connection.Disconnect();
                        }
                    });
            }
        
        public:

            virtual void OnMessageRecieve(){
                
            }

            void run(){ Context.run(); }
            bool isConnected(){return _Connection.Socket->is_open();}

            template<typename DataType>
            friend Client_interface<T>& operator<<(Client_interface<T> &Client, DataType& Data){
                Client._Connection.Data << Data;
                return Client;
            }

            template<typename DataType>
            friend Client_interface<T>& operator>>(Client_interface<T> &Client, DataType& Data){
                Client._Connection.Data >> Data;
                return Client;
            }

        };

    }
}
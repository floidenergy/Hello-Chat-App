#pragma once

#include <jy_jai.hpp>

namespace jai{
    namespace net{

        struct User{
            char name[20];
        };

        enum class Ownership{
            Client,
            Server
        };

        template<typename T>
        struct Connection : std::enable_shared_from_this<Connection<T>>{
            Payload<T> Data;
            std::shared_ptr<asio::ip::tcp::socket> Socket;
            Ownership _Owner;
            User UserInfo;

            Connection() = default;

            Connection(asio::io_context &Contxt, Ownership owner)
                : Socket(std::make_shared<asio::ip::tcp::socket>(Contxt)), _Owner(owner){}

            Connection(std::shared_ptr<asio::ip::tcp::socket> _soc, Ownership owner){
                Socket = _soc;
                _Owner = owner;
            }

            void SetUserInfo(std::string _name){
                strcpy(UserInfo.name, _name.c_str());
            }

            bool Connect(asio::ip::tcp::endpoint ep){
                asio::error_code ec;
                Socket->connect(ep, ec);
                if(ec){
                    std::cout << "Couldn't Connect To " << ep.address() << std::endl;
                    return false;
                }
                    std::cout << "Connected~" << std::endl;
                    return true;
            }

            void Disconnect(){                
                Socket->cancel();
                Socket->close();
            }

            void Ban(){

            }

            void Recieve(){
                Read_Header();
            }

            void Send(asio::error_code &ec){
                Write_Header(this->Data, ec);
            }

            void Send(Payload<T> msg, asio::error_code &ec){
                Write_Header(msg, ec);
            }

            void Async_Send(){
                Async_Send(this->Data);
            }

            void Async_Send(Payload<T> msg){
                Async_Write_Header(msg);
            }
            

        private:
            void Read_Header(){
                try{
                    Socket->read_some(asio::buffer(&Data.Header, sizeof(Data.Header)));
                
                    if(Data.Header.size > 0)
                        Read_Body();
                }catch(asio::error_code ec){
                    std::cout << "Couldnt read from the socket" << std::endl;
                    this->Disconnect();
                }
            }

            void Read_Body(){
                size_t old_Size = Data.Body.size();
                try{
                    Data.Body.resize(Data.Header.size);
                    Socket->read_some(asio::buffer(Data.Body.data(), Data.Header.size));                
                }catch(asio::error_code ec){
                    std::cout << "Couldnt read from the socket" << std::endl;
                    this->Disconnect();
                    Data.Body.resize(old_Size);
                }
            }

            void Write_Header(Payload<T> &msg, asio::error_code &ec){
                size_t bytes = Socket->write_some(asio::buffer(&msg.Header, sizeof(msg.Header)), ec);

                if(ec){
                    std::cout << "Couldn't Send Header: " << ec.message() << std::endl;
                }else{
                        Write_Body(msg, ec);
                }
            }

            void Write_Body(Payload<T> &msg, asio::error_code &ec){
                size_t bytes = Socket->write_some(asio::buffer(msg.Body.data(), msg.Header.size), ec);

                if(ec){
                    std::cout << "Couldn't Send Body: " << ec.message() << std::endl;
                }
            }

            void Async_Write_Header(Payload<T> &msg){
                    Socket->async_write_some(asio::buffer(&msg.Header, sizeof(msg.Header)),
                        [this, msg](asio::error_code ec, size_t bytes){
                            if(!ec){
                                Async_Write_Body(msg);
                            }else{
                                std::cout << "Couldn't Send Header: " << ec.message() << std::endl;
                            }
                        });
                    
            }

            void Async_Write_Body(Payload<T> msg){
                Socket->async_write_some(asio::buffer(msg.Body.data(), msg.Header.size),
                    [this, msg](asio::error_code ec, size_t bytes){
                        if(ec){
                            std::cout << "Couldn't Send Body: " << ec.message() << std::endl;
                        }
                    });
            }
            
        };

    }
}
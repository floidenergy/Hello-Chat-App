#pragma once

#include <jy_jai.hpp>
#include <typeinfo>

namespace jai{
    namespace net{
        
        template<typename T>
        struct MessageHeader{
            T ID;
            size_t size;
            MessageHeader() : size(0), strSize(0){}
            size_t strSize;
        };

        /*
        *
        * Payload is a stuct which will be data container that will be sending btween clients and server
        * 
        * It has header whiche Contain (ID, Size, strSize);
        * @ID: to identify server what to do with that data
        * @Size: size of bytes that body contain
        * @strSize: made for strings it to resize std::strings
        * 
        * And Body whiche Contain vector of type uint8_t that will contain bytes of data
        * 
        */
        template<typename T>
        struct Payload{
            MessageHeader<T> Header;
            std::vector<uint8_t> Body;

            template<typename DataType>
            friend Payload& operator<<(Payload& obj, DataType& data){

                size_t i = obj.Body.size();

                obj.Body.resize(i + sizeof(data));

                std::memcpy(obj.Body.data() + i, &data, sizeof(data));

                obj.Header.size = obj.Body.size();

                return obj;
            }

            template<typename DataType>
            friend Payload& operator>>(Payload& obj, DataType& data){
                
                size_t i = obj.Body.size() - sizeof(data);

                std::memcpy(&data, obj.Body.data() + i, sizeof(data));

                obj.Body.resize(i);

                obj.Header.size = obj.Body.size();

                return obj;
            }


            //strings
            friend Payload& operator<<(Payload& obj, std::string& data){

                obj.Header.strSize = data.size();

                char _data[data.size()];

                strcpy(_data, data.data());

                size_t i = obj.Body.size();

                obj.Body.resize(i + data.size());

                std::memcpy(obj.Body.data() + i, _data, sizeof(_data));

                obj.Header.size = obj.Body.size();

                return obj;
            }

            friend Payload& operator>>(Payload& obj, std::string& data){

                char _data[obj.Header.strSize];

                size_t i = obj.Body.size() - sizeof(_data);

                std::memcpy(&_data, obj.Body.data() + i, sizeof(_data));

                data.resize(obj.Header.strSize);
                
                strcpy(data.data(), _data);

                obj.Body.resize(i);

                obj.Header.size = obj.Body.size();

                obj.Header.strSize = 0;

                return obj;
            }
       
            
        };
    }
}
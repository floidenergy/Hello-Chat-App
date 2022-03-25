#pragma once
#define PORT 6250
#define ASIO_STANDALONE

#ifdef _WIN32
    #define _WIN32_WINNT 0x0A00
#endif

#include <asio.hpp>

#include <iostream>

#include <queue>
#include <deque>
#include <algorithm>
#include <iomanip>
#include <fstream>

#include <payload.hpp>
#include <connection.hpp>
#include <server.hpp>
#include <client.hpp>


namespace jai{
    namespace net{
        #define PORT 6250
    }
}

enum class Message_id {
    ClientConnected,
    RecieveMessege,
    Disconnect,
    ServerShutDown,
    Init_NewClient
};
g++ "server\*.cpp" -o bin\server.exe -I includes -I "D:\API\asio-1.18.1\asio-1.18.1\include" --static -l ws2_32 -l wsock32 -l pthread
bin\server
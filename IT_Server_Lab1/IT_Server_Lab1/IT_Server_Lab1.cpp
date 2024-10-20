#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <unordered_map>
#include <fstream>
#include "Table.h"

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define BUFFER_SIZE 512

void saveToFile(const std::unordered_map<std::string, std::pair<Table, bool>>& DB, const std::string& filename);
void loadFromFile(std::unordered_map<std::string, std::pair<Table, bool>>& DB, const std::string& filename);

int main() 
{
    WSADATA wsaData;
    int iResult;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) 
    {
        std::cerr << "WSAStartup failed with error: " << iResult << std::endl;
        return 1;
    }

    struct addrinfo* result = nullptr, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) 
    {
        std::cerr << "getaddrinfo failed with error: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) 
    {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) 
    {
        std::cerr << "bind failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) 
    {
        std::cerr << "listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    std::unordered_map<std::string, std::pair<Table, bool>> DB;
    int CURRENT_ID = 0;
    std::string filePath = "C:\\Users\\dwarf\\OneDrive\\Documents\\DataBaseSaveFile.txt";

    SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) 
    {
        std::cerr << "accept failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    char recvbuf[BUFFER_SIZE];
    int recvbuflen = BUFFER_SIZE;
    Table currentTable;

    char messageSize[512];
    std::string ms;

    do {
        recv(ClientSocket, recvbuf, recvbuflen, 0);
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);

        if (iResult > 0) 
        {
            std::string command(recvbuf, iResult);
            std::cout << "Received: " << command << std::endl;
            int var = std::stoi(command);
            std::string tableName;
            int colAmount = 0;
            int update = 0;
            std::string hinput;
            std::string commonMessage = "1 - Add table, 2 - Delete table, 3 - Show tables list, 4 - Update table, 5 - Print table, 6 - Save database, 7 - Load database: ";

            switch (var) 
            {
            case 1:
                commonMessage = "Enter table name: ";
                send(ClientSocket, commonMessage.c_str(), commonMessage.length(), 0);
                recv(ClientSocket, recvbuf, recvbuflen, 0);
                recv(ClientSocket, recvbuf, recvbuflen, 0);
                tableName = std::string(recvbuf);

                if (DB.find(tableName) != DB.end() && DB[tableName].second) 
                {
                    send(ClientSocket, "Name must be unique. Try again", 31, 0);
                    break;
                }

                commonMessage = "Enter amount of columns: ";
                send(ClientSocket, commonMessage.c_str(), commonMessage.length(), 0);
                recv(ClientSocket, recvbuf, recvbuflen, 0);
                recv(ClientSocket, recvbuf, recvbuflen, 0);
                colAmount = std::stoi(recvbuf);

                DB[tableName] = std::pair<Table, bool>(Table(++CURRENT_ID, colAmount, tableName), true);
                DB[tableName].first.initiateTable(ClientSocket);
                send(ClientSocket, "Table created", 13, 0);
                break;

            case 2: 
                commonMessage = "Enter table name: ";
                send(ClientSocket, commonMessage.c_str(), commonMessage.length(), 0);
                recv(ClientSocket, recvbuf, recvbuflen, 0);
                recv(ClientSocket, recvbuf, recvbuflen, 0);
                tableName = std::string(recvbuf);
                if (DB.find(tableName) != DB.end())
                {
                    DB[tableName].second = false;
                    send(ClientSocket, "Table deleted", 13, 0);
                }
                else
                {
                    send(ClientSocket, "No such file.", 13, 0);
                } 
                break;

            case 3: 
                for (const auto& p : DB)
                    if (p.second.second)
                    {
                        commonMessage = "Name: " + p.first + " columns: " + std::to_string(p.second.first.getColumnSize()) + "\n";
                        send(ClientSocket, commonMessage.c_str(), commonMessage.length(), 0);
                    }
                send(ClientSocket, "Tables displayed", 16, 0);
                break;

            case 4: 
                commonMessage = "Enter table name: ";
                send(ClientSocket, commonMessage.c_str(), commonMessage.length(), 0);
                recv(ClientSocket, recvbuf, recvbuflen, 0);
                recv(ClientSocket, recvbuf, recvbuflen, 0);
                tableName = std::string(recvbuf);

                if (DB.find(tableName) == DB.end())
                {
                    send(ClientSocket, "No such file.", 13, 0);
                    break;
                }

                currentTable = DB[tableName].first;

                commonMessage = "1 - Add row, 2 - Update row: ";
                send(ClientSocket, commonMessage.c_str(), commonMessage.length(), 0);
                recv(ClientSocket, recvbuf, recvbuflen, 0);
                recv(ClientSocket, recvbuf, recvbuflen, 0);
                update = std::stoi(recvbuf);

                if (update == 1) 
                {
                    currentTable.addRowManual(ClientSocket);
                }
                else if (update == 2) 
                {
                    commonMessage = "Enter row number: ";
                    send(ClientSocket, commonMessage.c_str(), commonMessage.length(), 0);
                    recv(ClientSocket, recvbuf, recvbuflen, 0);
                    recv(ClientSocket, recvbuf, recvbuflen, 0);
                    var = std::stoi(recvbuf);
                    currentTable.updateRowManual(var, ClientSocket);
                }

                DB[tableName].first = currentTable;
                send(ClientSocket, "Row updated", 11, 0);
                break;

            case 5:  
                commonMessage = "Enter table name: ";
                hinput = "";
                send(ClientSocket, commonMessage.c_str(), commonMessage.length(), 0);
                recv(ClientSocket, messageSize, recvbuflen, 0);
                recv(ClientSocket, recvbuf, recvbuflen, 0);
                for (size_t k = 0; k < std::stoi(ms.assign(messageSize)); k++)
                    hinput += recvbuf[k];
                if(DB.find(hinput) == DB.end())
                {
                    send(ClientSocket, "No such file.", 13, 0);
                    break;
                }
                DB[hinput].first.print(ClientSocket);
                send(ClientSocket, "Table printed", 13, 0);
                break;

            case 6:
                saveToFile(DB, filePath);
                send(ClientSocket, "Database saved", 14, 0);
                break;

            case 7:
                loadFromFile(DB, filePath);
                send(ClientSocket, "Database loaded", 15, 0);
                break;

            default:
                send(ClientSocket, "Invalid command", 15, 0);
                break;
            }
        }
        else if (iResult == 0) 
        {
            std::cout << "Connection closing..." << std::endl;
        }
        else 
        {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    } while (iResult > 0);

    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) 
    {
        std::cerr << "shutdown failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}

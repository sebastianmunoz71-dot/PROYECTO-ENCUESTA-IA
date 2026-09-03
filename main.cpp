#include <iostream>
#include <fstream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

//#pragma comment(lib, "ws2_32.lib")

using namespace std;

void guardarEnBD(const string& datos) {
    ofstream bd("base_datos_ia.csv", ios::app);
    if (bd.is_open()) {
        bd << datos << "\n";
        bd.close();
        cout << "[C++] Registro guardado en base_datos_ia.csv" << endl;
    }
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);

    cout << "========================================" << endl;
    cout << " Servidor C++ Escuchando en puerto 8080 " << endl;
    cout << "========================================" << endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        char buffer[1024] = {0};
        recv(clientSocket, buffer, sizeof(buffer), 0);

        string request(buffer);
        
        // Manejo de la petición OPTIONS (CORS para permitir conexión desde el navegador)
        if (request.find("OPTIONS") != string::npos) {
            string response = "HTTP/1.1 204 No Content\r\n"
                              "Access-Control-Allow-Origin: *\r\n"
                              "Access-Control-Allow-Methods: POST, OPTIONS\r\n"
                              "Access-Control-Allow-Headers: Content-Type\r\n\r\n";
            send(clientSocket, response.c_str(), response.length(), 0);
        } 
        // Manejo de la petición POST con los datos
        else if (request.find("POST") != string::npos) {
            size_t bodyPos = request.find("\r\n\r\n");
            if (bodyPos != string::npos) {
                string body = request.substr(bodyPos + 4);
                if (!body.empty()) {
                    guardarEnBD(body);
                }
            }

            string response = "HTTP/1.1 200 OK\r\n"
                              "Access-Control-Allow-Origin: *\r\n"
                              "Content-Type: text/plain\r\n\r\n"
                              "Exito";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        closesocket(clientSocket);
    }

    WSACleanup();
    return 0;
}
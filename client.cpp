// client.cpp
// 2022.6.10
// author: boiled_fish
#include <iostream>
#include <winsock.h>
#include <string>
using namespace std;

#define BUF_LEN       100

#define PAN_DOWNLOAD  0
#define PAN_MOVE      1
#define PAN_COPY      2
#define PAN_PASTE     3
#define PAN_DELETE    4
#define PAN_SHOW      5
#define PAN_REGISTER  6
#define PAN_LOGIN     7

#define PAN_SUCCESS   8
#define PAN_FAILURE   9

struct data_package {
    int op;
    char username[BUF_LEN];
    char content[BUF_LEN];
};

bool client_request(int opt);
void fill_pkg(data_package* pkg);
void clean(SOCKET sock);

int main() {
	/*变量定义**********************************************************************/
	int ret;
	WSADATA wsaData;
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	char server_ip[20] = "35.220.130.168";
	SOCKADDR_IN sockAddr;
	int opt = -1;
	
    /*连接服务器*******************************************************************/   
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "DLL initialization failed" << WSAGetLastError() <<  endl;
        return -1;
    }
    
    cout << "server ip: " << server_ip << endl;
    //cin >> server_ip;
    
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.S_un.S_addr = inet_addr(server_ip);
	sockAddr.sin_port = htons(8080);
	if (connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		cout << "Failed to connect to server, error: " << WSAGetLastError() <<  endl;
        clean(sock);
        return -1;
	}
	cout << "server connected" << endl;
    cout << endl;
    
	/*****************************************************************************/
	/*注册、登录*/
	cout << "*****************************************************************************" << endl;
	cout << "PAN_DOWNLOAD  0" << endl;
	cout << "PAN_MOVE      1" << endl;
	cout << "PAN_COPY      2" << endl;
	cout << "PAN_PASTE     3" << endl;
	cout << "PAN_DELETE    4" << endl;
	cout << "PAN_SHOW      5" << endl;
	cout << "PAN_REGISTER  6" << endl;
	cout << "PAN_LOGIN     7" << endl;
	cout << "*****************************************************************************" << endl;
	
	while (1) {
		cout << "op: ";
		cin >> opt;
		if (opt >= PAN_DOWNLOAD && opt <= PAN_LOGIN && !client_request(opt)) {
			cout << "op " << opt << " failed" << endl;
		}
		else {
			cout << "succeed" << endl;
		}
	}

    clean(sock);
    return 0;
}

bool client_request(int opt) {
	data_package pkg;
	pkg.op = opt;
    if (!fill_pkg(&pkg)){
    	return false;
	}
    int ret = send(sock, (char*)&pkg, sizeof(pkg), 0);
    if (ret < 0) {
    	cout << "send failed, error: " << WSAGetLastError() <<  endl;
        return false;
	}
	/*阻塞等待server返回值，超时server断开连接，判断为请求失败*/
	char* recv_buf[BUF_LEN] = { 0 };
	int ret = recv(sock, recv_buf, 1, 0);
	if (ret <= 0) {
		cout << "recv failed, error: " << WSAGetLastError() <<  endl;
        return false;
	}
	/*临时的返回数据包解析*/
	if (recv_buf[0] != PAN_SUCCESS) {
		return false;
	}
	return true;
}

bool fill_pkg(data_package* pkg) {
	switch(pkg.op) {
		case PAN_REGISTER:
        case PAN_LOGIN:
        	char username[BUF_LEN] = { 0 };
			cout << " Username: ";
			cin >> username; 
			if (strlen(username) < BUF_LEN) {
				memcpy(pkg.username, username);
			}
			else {
				return false;
			}
    		char password[BUF_LEN] = { 0 };
			cout << " Password: ";
			cin >> password; 
			if (strlen(password) < BUF_LEN) {
				memcpy(pkg.content, password);
			}
			else {
				return false;
			}
        	break;
        case PAN_MOVE:
        	char username[BUF_LEN] = { 0 };
			cout << " Username: ";
			cin >> username; 
			if (strlen(username) < BUF_LEN) {
				memcpy(pkg.username, username);
			}
			else {
				return false;
			}
			memset(pkg.content, 0, BUF_LEN);
			char start_path[BUF_LEN] = { 0 };
			cout << " Start path: ";
			cin >> start_path; 
			if (strlen(start_path) < BUF_LEN) {
				memcpy(pkg.content, start_path);
			}
			
			pkg.content[strlen(start_path)] = '-';
			
			char destinaion_path[BUF_LEN] = { 0 };
			cout << " Destinaion path: ";
			cin >> destination_path;
			if (strlen(destination_path) + strlen(start_path) + 1 <= BUF_LEN) {
				memcpy(&pkg.content[strlen(start_path) + 1], destination_path);
			}
            break;
        case PAN_COPY:
        	char username[BUF_LEN] = { 0 };
			cout << " Username: ";
			cin >> username; 
			if (strlen(username) < BUF_LEN) {
				memcpy(pkg.username, username);
			}
			memset(pkg.content, 0, BUF_LEN);
			char copy_path[BUF_LEN] = { 0 };
			cout << " Copy path: ";
			cin >> copy_path; 
			if (strlen(copy_path) < BUF_LEN) {
				memcpy(pkg.content, copy_path);
			}
            break;
        case PAN_PASTE:
        	char username[BUF_LEN] = { 0 };
			cout << " Username: ";
			cin >> username; 
			if (strlen(username) < BUF_LEN) {
				memcpy(pkg.username, username);
			}
			memset(pkg.content, 0, BUF_LEN);
			char paste_path[BUF_LEN] = { 0 };
			cout << " Paste path: ";
			cin >> start_path; 
			if (strlen(paste_path) < BUF_LEN) {
				memcpy(pkg.content, paste_path);
			}
        	break;
        case PAN_DELETE:
        	char username[BUF_LEN] = { 0 };
			cout << " Username: ";
			cin >> username; 
			if (strlen(username) < BUF_LEN) {
				memcpy(pkg.username, username);
			}
			
			memset(pkg.content, 0, BUF_LEN);
			char file_path[BUF_LEN] = { 0 };
			cout << " Delete file path: ";
			cin >> file_path; 
			if (strlen(file_path) < BUF_LEN) {
				memcpy(pkg.content, file_path);
			}
            break;
        case PAN_SHOW:
        	char username[BUF_LEN] = { 0 };
			cout << " Username: ";
			cin >> username; 
			if (strlen(username) < BUF_LEN) {
				memcpy(pkg.username, username);
			}
			
			memset(pkg.content, 0, BUF_LEN);
			char path[BUF_LEN] = { 0 };
			cout << " Show path: ";
			cin >> path; 
			if (strlen(path) < BUF_LEN) {
				memcpy(pkg.content, path);
			}
        	break;
        case PAN_DOWNLOAD:
        	char username[BUF_LEN] = { 0 };
			cout << " Username: ";
			cin >> username; 
			if (strlen(username) < BUF_LEN) {
				memcpy(pkg.username, username);
			}
			
			memset(pkg.content, 0, BUF_LEN);
			char file_path[BUF_LEN] = { 0 };
			cout << " file path: ";
			cin >> file_path; 
			if (strlen(file_path) < BUF_LEN) {
				memcpy(pkg.content, file_path);
			}
			
			pkg.content[strlen(file_path)] = '-';
			
			char destinaion_path[BUF_LEN] = { 0 };
			cout << " Destinaion path: ";
			cin >> destination_path;
			if (strlen(destination_path) + strlen(file_path) + 1 <= BUF_LEN) {
				memcpy(&pkg.content[strlen(file_path) + 1], destination_path);
			}
            break;
	}
	return true;
}

void clean(SOCKET sock) {
	closesocket(sock);
	WSACleanup();
}

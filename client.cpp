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
    string username;
    string content;
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

	/*设置非阻塞模式*/
    ret = ioctlsocket(sock, FIONBIO, (unsigned long *)&ul);
	if(ret == SOCKET_ERROR) {
		cout << "非阻塞设置失败" << endl;
		clean(sock);
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
	cout << "注册(6)，登录(7): ";
	while (1) {
		cin >> opt;
		// register
		if (opt == PAN_REGISTER){
			cout << "Register:" << endl;
   			if (!client_request(PAN_REGISTER)) {
				cout << "register failed" << endl;
			}
			else {
				cout << "register succeed" << endl;
			}
   		}
   		//login
   		else if (opt == PAN_LOGIN) {
   			cout << "Login:" << endl;
   			if (!client_request(PAN_LOGIN)) {
   				cout << "login failed" << endl;	
			}
			else {
				cout << "login succeed" << endl;
			}
   		}
   		else {
   			break;	
		}
	}

    clean(sock);
    return 0;
}

bool client_request(int opt) {
	data_package pkg;
	pkg.op = opt;
    fill_pkg(&pkg);
    int ret = send(sock, (char*)&pkg, sizeof(pkg), 0);
    if (ret < 0) {
    	cout << "send failed, error: " << WSAGetLastError() <<  endl;
        return false;
	}
	/*超过10s无回应，判断为请求失败*/
	char* recv_buf[BUF_LEN] = { 0 };
	int ret = recv(sock, recv_buf, 1, 0);
	if (ret < 0) {
		cout << "recv failed, error: " << WSAGetLastError() <<  endl;
        return false;
	}
	if (recv_buf[0] != PAN_SUCCESS) {
		return false;
	}
	return true;
}

void fill_pkg(data_package* pkg) {
    cout << "  Username: ";
    cin >> pkg->username;      
    cout << "  Password: ";
    cin >> pkg->content;
}

void clean(SOCKET sock) {
	closesocket(sock);
	WSACleanup();
}

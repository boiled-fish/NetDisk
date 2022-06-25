// client.cpp
// 2022.6.10
// author: boiled_fish
#pragma comment(lib, "ws2_32.lib") 
#include "../../include/client.h"
using namespace std;

int main() {
	/*变量定义**********************************************************************/
	int ret, opt = -1;
	WSADATA wsaData;

	/*连接服务器*******************************************************************/
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cout << "DLL initialization failed" << WSAGetLastError() << endl;
		return -1;
	}
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		cout << "invalid" << endl;
		return 0;
	}

	char server_ip[20] = "35.220.130.168";
	cout << "server ip: " << server_ip << endl;
	int port;
	cout << "server port: ";
	cin >> port;
	//cin >> server_ip;

	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.S_un.S_addr = inet_addr(server_ip);
	sockAddr.sin_port = htons(port);

	if (connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		cout << "Failed to connect to server, error: " << WSAGetLastError() << endl;
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
	cout << "PAN_UPLOAD    10" << endl;
	cout << "*****************************************************************************" << endl;

	while (1) {
		cout << "op: ";
		cin >> opt;
		if ((opt >= PAN_DOWNLOAD && opt <= PAN_LOGIN) || (opt == PAN_UPLOAD) || (opt == PAN_DOWNLOAD)) {
			client_request(opt, sock);
		}
		else {
			cout << "op illegal" << endl;
		}
	}

	clean(sock);
	return 0;
}

bool client_request(int opt, SOCKET sock) {
	data_package pkg_snt;
	pkg_snt.op = opt;
	if (!fill_pkg(&pkg_snt, sock)) {
		return false;
	}
	if (pkg_snt.op != PAN_UPLOAD && pkg_snt.op != PAN_DOWNLOAD) {
		int ret = send(sock, (char*)&pkg_snt, sizeof(pkg_snt), 0);
		cout << "ret=" << ret << endl;
		if (ret < 0) {
			cout << "send failed, error: " << WSAGetLastError() << endl;
			return false;
		}
		/*阻塞等待server返回值，超时server断开连接，判断为请求失败*/
		data_package pkg_recv;
		ret = recv(sock, (char*)&pkg_recv, sizeof(data_package), 0);
		if (ret <= 0) {
			cout << "recv failed, error: " << WSAGetLastError() << endl;
			return false;
		}
		/*临时的返回数据包解析*/
		if (!parse_pkg(&pkg_snt, &pkg_recv)) {
			return false;
		}
	}
	return true;
}
bool parse_pkg(data_package* pkg_snt, data_package* pkg_recv) {
	switch (pkg_snt->op) {
		case PAN_REGISTER:
		case PAN_LOGIN:
		case PAN_MOVE:
		case PAN_COPY:
		case PAN_PASTE:
		case PAN_DELETE: {
			if (pkg_recv->op == PAN_SUCCESS) {
				cout << "success" << endl;
			}
			else {
				cout << "failed" << endl;
				cout << pkg_recv->content << endl;
			}
			break;
		}
		case PAN_SHOW: {
			if (pkg_recv->op == PAN_SUCCESS) {
				cout << "show success" << endl;
				cout << pkg_recv->content << endl;
			}
			else {
				cout << "show failed" << endl;
				cout << pkg_recv->content << endl;
			}
			break;
		}
	}
	return true;
}
bool fill_pkg(data_package* pkg, SOCKET sock) {
	char username[BUF_LEN] = { 0 };
	char content1[BUF_LEN] = { 0 };
	char content2[BUF_LEN] = { 0 };
	switch (pkg->op) {
		case PAN_REGISTER:
		case PAN_LOGIN: {
			cout << " Username: ";
			cin >> username;
			if (strlen(username) < BUF_LEN) {
				strcpy(pkg->username, username);
			}
			else {
				return false;
			}

			cout << " Password: ";
			cin >> content1;
			if (strlen(content1) < BUF_LEN) {
				strcpy(pkg->content, content1);
			}
			else {
				return false;
			}
			break;
		}
		case PAN_MOVE: {
			cout << " Username: ";
			cin >> username;
			if (strlen(username) < BUF_LEN) {
				strcpy(pkg->username, username);
			}
			else {
				return false;
			}

			memset(pkg->content, 0, BUF_LEN);
			cout << " Start path: ";
			cin >> content1;
			if (strlen(content1) < BUF_LEN) {
				strcpy(pkg->content, content1);
			}
			else {
				return false;
			}

			pkg->content[strlen(content1)] = '-';

			cout << " Destinaion path: ";
			cin >> content2;
			if (strlen(content1) + strlen(content2) + 1 <= BUF_LEN) {
				strcpy(&pkg->content[strlen(content1) + 1], content2);
			}
			else {
				return false;
			}
			break;
		}
		case PAN_COPY: {
			cout << " Username: ";
			cin >> username;
			if (strlen(username) < BUF_LEN) {
				strcpy(pkg->username, username);
			}
			else {
				return false;
			}

			memset(pkg->content, 0, BUF_LEN);
			cout << " Copy path: ";
			cin >> content1;
			if (strlen(content1) < BUF_LEN) {
				strcpy(pkg->content, content1);
			}
			else {
				return false;
			}
			break;
		}
		case PAN_PASTE: {
			cout << " Username: ";
			cin >> username;
			if (strlen(username) < BUF_LEN) {
				strcpy(pkg->username, username);
			}
			else {
				return false;
			}
			memset(pkg->content, 0, BUF_LEN);
			cout << " Paste path: ";
			cin >> content1;
			if (strlen(content1) < BUF_LEN) {
				strcpy(pkg->content, content1);
			}
			else {
				return false;
			}
			break;
		}
		case PAN_DELETE: {
			cout << " Username: ";
			cin >> username;
			if (strlen(username) < BUF_LEN) {
				strcpy(pkg->username, username);
			}
			else {
				return false;
			}
			memset(pkg->content, 0, BUF_LEN);
			cout << " Delete file path: ";
			cin >> content1;
			if (strlen(content1) < BUF_LEN) {
				strcpy(pkg->content, content1);
			}
			break;
		}
		case PAN_SHOW: {
			cout << " Username: ";
			cin >> username;
			if (strlen(username) < BUF_LEN) {
				strcpy(pkg->username, username);
			}
			else {
				return false;
			}

			memset(pkg->content, 0, BUF_LEN);
			cout << " Show path: ";
			cin >> content1;
			if (strlen(content1) < BUF_LEN) {
				strcpy(pkg->content, content1);
			}
			else {
				return false;
			}
			break;
		}
		case PAN_DOWNLOAD: {
			/*
			cout << "****************" << endl;
			cout << "PAN_DIR       14" << endl;
			cout << "PAN_FILE      15"
			cout << "****************" << endl;
			int type = 0;
			cout << " Download type: ";
			cin >> type;
			*/
			cout << " Local Download Path(directory): ";
			cin >> content1;
			if (strlen(content1) < BUF_LEN) {
				strcpy(pkg->username, content1);
			}
			else {
				return false;
			}
			cout << " From Pan Path: ";
			cin >> content2;
			if (strlen(content2) < BUF_LEN) {
				strcpy(pkg->content, content2);
				cout << "into download" << endl;
				if (!download(pkg, sock)) {
					return false;
				}
			}
			else {
				return false;
			}
			break;
		}
		case PAN_UPLOAD: {
			cout << " Local Path: ";
			cin >> content1;
			if (strlen(content1) < BUF_LEN) {
				strcpy(pkg->username, content1);
			}
			else {
				return false;
			}
			cout << " Pan Path: ";
			cin >> content2;
			if (strlen(content2) < BUF_LEN) {
				cout << "into upload" << endl;
				strcpy(pkg->content, content2);
				if (!upload(pkg, sock)) {
					return false;
				}
			}
			else {
				return false;
			}
			break;
		}
	}
	return true;
}

void clean(SOCKET sock) {
	closesocket(sock);
	WSACleanup();
}

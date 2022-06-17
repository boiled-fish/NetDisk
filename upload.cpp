// upload.cpp
// 2022.6.16
// author: boiled_fish
#pragma comment(lib, "ws2_32.lib")
#include "../../include/client.h"
using namespace std;

/*数组反转函数*/ 
void reverse(char* a, int len)
{
	int i = 0;
	int t;
	int j;

	for (i = 0; i < len / 2; i++) {
		t = a[i];
		a[i] = a[len - 1 - i];
		a[len - 1 - i] = t;
	}
}
/*int转换成字符串函数*/
void int2string(int b, char* c)
{
	char d[BUF_LEN];
	char temp[2] = { 0 };//转换过程中需要暂存数据	
	int i;

	while (b % 10) {
		char a = b % 10 + '0';
		temp[0] = a;
		b = b / 10;
		strcat(c, temp);
	}
	reverse(c, strlen(c));
}
/*字符串转换成int函数*/
int string2int(char b[])
{
	int i;
	int res = 0, temp_res;
	
	for (i = 0; i < strlen(b); i++) {
		temp_res = (b[i] - '0');
		res *= 10;
		res += temp_res;
	}
	return res;
}
/*求servre端路径*/
void pan_path(data_package* pkg, char* path) {
	int len1 = strlen(pkg->username);
	int len2 = strlen(pkg->content);
	int i, pivot = 0;
	for (i = 0; i < len1; ++i) {
		if (pkg->username[i] == '\\') {
			pivot = i;
		}
	}
	strcpy(path, pkg->content);
	strcpy(path + len2, pkg->username + pivot);
}
 
bool upload_file(data_package* pkg, SOCKET sock) {
	/*填充check包*/
	/*op：PAN_UPLOAD_FILE_CHECK*/ 
	/*username: md5*/
	/*content: s端文件路径-文件大小*/
	data_package check_pkg;
	check_pkg.op = PAN_UPLOAD_FILE_CHECK;
	if (!get_md5(pkg->username, check_pkg.username)) {
		return false;
	}
	pan_path(pkg, check_pkg.content);
	int content_len = strlen(check_pkg.content);
	check_pkg.content[content_len] = '-';
	/*计算文件大小*/ 
	char file_size[BUF_LEN] = { 0 };
	int size = 0;
	FILE* file = fopen(pkg->username, "r");
	if (file)
	{
		size = filelength(fileno(file));
		fclose(file);
	}
	int2string(size, file_size);
	strcpy(check_pkg.content + content_len + 1, file_size);
	
	/*发送check包*/ 
	int ret = send(sock, (char*)&check_pkg, sizeof(check_pkg), 0);
	if (ret < 0) {
		cout << "send check pkg failed, error: " << WSAGetLastError() << endl;
	}
	/*阻塞等待*/ 
	data_package pkg_recv;
	ret = recv(sock, (char*)&pkg_recv, sizeof(data_package), 0);
	if (ret <= 0) {
		cout << "recv failed, error: " << WSAGetLastError() << endl;
	}
	if (pkg_recv.op == PAN_SUCCESS) {
		cout << "upload success" << endl;
		cout << pkg_recv.content << endl;
	}
	else if (pkg_recv.op == PAN_UPLOAD_FILE_CHECK) {
		while (1) {
			int file_no = string2int(pkg_recv.content);
			int len = file_no * BUF_LEN;
			/*op：PAN_UPLOAD_FILE_CONTENT*/
			/*username：序号-md5*/
			/*content: 文件内容*/ 
			data_package pkg_snt;
			pkg_snt.op = PAN_UPLOAD_FILE_CONTENT;
			strcpy(pkg_snt.username, pkg_recv.content); //序号 
			pkg_snt.username[strlen(pkg_recv.content)] = '-'; 
			strcpy(pkg_snt.username + strlen(pkg_recv.content) + 1, check_pkg.username); //md5
			/*读文件*/
			ifstream infile;
			infile.open(pkg->username, ios::in);
			if (!infile.is_open()) {
				return false;
			}
			infile.seekg(len);
			int count = 0;
			char c;
			while ((c = infile.get()) != EOF && count < BUF_LEN) {
				pkg_snt.content[count++] = c;
			}
			/*发送check包*/ 
			int ret = send(sock, (char*)&pkg_snt, sizeof(pkg_snt), 0);
			if (ret < 0) {
				cout << "send data pkg failed, error: " << WSAGetLastError() << endl;
			}
			/*阻塞等待*/
			pkg_recv.op = -1;
			memset(pkg_recv.username, 0, BUF_LEN);
			memset(pkg_recv.content, 0, BUF_LEN);
			ret = recv(sock, (char*)&pkg_recv, sizeof(data_package), 0);
			if (ret <= 0) {
				cout << "recv failed, error: " << WSAGetLastError() << endl;
			}
			if (pkg_recv.op == PAN_SUCCESS) {
				cout << "upload success" << endl;
				cout << pkg_recv.content << endl;
				break;
			}
		}
	}
	return true;
}

void dfs_dir(data_package* pkg, SOCKET sock) {
	//文件句柄
	long hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string path = pkg->username;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
		do {
			//如果是目录,发送mkdir包之后迭代之
			//如果是文件，则调用upload_file 
			if ((fileinfo.attrib &  _A_SUBDIR)) {
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					string dir_path = p.assign(path).append("\\").append(fileinfo.name);
					strcpy(pkg->username, dir_path.c_str());
					pan_path(pkg, pkg->content);
					
					/*发送mkdir包*/
					data_package pkg_dir;
					pkg_dir.op = PAN_UPLOAD_DIR;
					strcpy(pkg_dir.content, pkg->content);
					int ret = send(sock, (char*)&pkg_dir, sizeof(pkg_dir), 0);
					if (ret < 0) {
						cout << "send mkdir pkg failed, error: " << WSAGetLastError() << endl;
						return;
					}
					/*阻塞等待*/ 
					data_package pkg_recv;
					ret = recv(sock, (char*)&pkg_recv, sizeof(data_package), 0);
					if (ret <= 0) {
						cout << "mkdir "<< dir_path << " failed, error: " << WSAGetLastError() << endl;
						return;
					}
					if (pkg_recv.op == PAN_SUCCESS) {
						cout << "mkdir " << dir_path << " success" << endl;
						cout << pkg_recv.content << endl;
					}
					else {
						return;
					}	
					dfs_dir(pkg, sock);
				}
			}
			else {
				string file_path = p.assign(path).append("\\").append(fileinfo.name);
				data_package file_pkg;
				file_pkg.op = PAN_UPLOAD;
				strcpy(file_pkg.username, file_path.c_str());
				strcpy(file_pkg.content, pkg->content);
				upload_file(&file_pkg, sock);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
/*upload文件夹*/
/*发送s端路径*/
/*op : PAN_UPLOAD_DIR*/
/*username: 无意义*/
/*content: s端路径*/
bool upload_dir(data_package* pkg, SOCKET sock) {
	data_package pkg_dir;
	pkg_dir.op = PAN_UPLOAD_DIR;
	pan_path(pkg, pkg_dir.content);
	
	/*发送mkdir包*/ 
	int ret = send(sock, (char*)&pkg_dir, sizeof(pkg_dir), 0);
	if (ret < 0) {
		cout << "send mkdir pkg failed, error: " << WSAGetLastError() << endl;
	}
	/*阻塞等待*/ 
	data_package pkg_recv;
	ret = recv(sock, (char*)&pkg_recv, sizeof(data_package), 0);
	if (ret <= 0) {
		cout << "mkdir failed, error: " << WSAGetLastError() << endl;
	}
	if (pkg_recv.op == PAN_SUCCESS) {
		cout << "mkdir success" << endl;
		cout << pkg_recv.content << endl;
	}
	else {
		return false;
	}
	dfs_dir(pkg, sock);
	return true;
}
/*查看文件类型*/ 
int file_type(char* path) {
	struct stat s;
	if (stat(path, &s) == 0) {
		if (s.st_mode & S_IFDIR) {
			return PAN_DIR;
		}
		else if (s.st_mode & S_IFREG) {
			return PAN_FILE;
		}
		else {
			return PAN_FAILURE;
		}
	}
	else {
		return PAN_FAILURE;
	}
}
bool upload(data_package* pkg, SOCKET sock) {
	switch (file_type(pkg->username)) {
		case PAN_DIR: {
			return upload_dir(pkg, sock);
		}
		case PAN_FILE: {
			return upload_file(pkg, sock);
		}
		default: {
			return false;
		}
	}
	return true;
}

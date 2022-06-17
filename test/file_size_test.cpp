#include <iostream>
#include <string>
#include <cstring>
#include <io.h>
using namespace std;

struct data_package {
    int op;
    char username[100];
    char content[100];
};

string getCmdResult(const string &strCmd)  
{
    char buf[10240] = {0};
    FILE *pf = NULL;
 
    if( (pf = popen(strCmd.c_str(), "r")) == NULL )
    {
        return "";
    }
 
    string strResult;
    while(fgets(buf, sizeof buf, pf))
    {
        strResult += buf;
    }
 
    pclose(pf);
 
    unsigned int iSize =  strResult.size();
    if(iSize > 0 && strResult[iSize - 1] == '\n')
    {
        strResult = strResult.substr(0, iSize - 1);
    }
 
    return strResult;
}

bool get_md5(char* file_name, char* md5) {
	string f = file_name;
	string cmd = "certutil -hashfile ";
	cmd += f;
	cmd += " MD5";
	string cmd_res = getCmdResult(cmd);
	/*
	MD5 的 filename 哈希:
	e1507aecc719c255660be912bcbc541c
	CertUtil: -hashfile 命令成功完成。
	*/
	if (cmd_res[0] == 'M') {
		strcpy(md5, cmd_res.substr(14 + f.size(), 32).c_str());
	}
	/*
	CertUtil: -hashfile 失败: 0x80070002 (WIN32: 2 ERROR_FILE_NOT_FOUND)
	CertUtil: 系统找不到指定的文件。
	*/
	else {
		return false;
	}
	return true;
}
//数组反转函数
int reverse(char* a, int len)
{
	int i = 0;
	int t;
	int j;

	for (i = 0; i < len / 2; i++)
	{
		t = a[i];
		a[i] = a[len - 1 - i];
		a[len - 1 - i] = t;
	}
	printf("\n");
}
void int2string(int b, char* c)
{
	char d[100];
	char temp[2] = { 0 };//转换过程中需要暂存数据	
	int i;

	while (b % 10)//到最高位的数值计算回来后就没有后续的数值可以计算，while循环结束
	{
		char a = b % 10 + '0';//将余数转换成字符
		temp[0] = a;//余数转换的字符暂存在temp数组中
		b = b / 10;//余数就是最低位个位的数值且以转换成字符，需要除以10将十位数上的数值放到最低位个位上
		strcat(c, temp);//将temp中暂存的追加到c中
	}
	reverse(c, strlen(c));//所有转换完成后反转数组	
}
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
	strcpy(path + len2, pkg->username + pivot + 1);
}
int main() {
	data_package pkg;
	cin >> pkg.username;
	cin >> pkg.content;
	
	data_package check_pkg;
	check_pkg.op = 1;
	if (!get_md5(pkg.username, check_pkg.username)) {
		return false;
	}
	pan_path(&pkg, check_pkg.content);
	int content_len = strlen(check_pkg.content);
	check_pkg.content[content_len] = '-';
	/*计算文件大小*/ 
	char file_size[100] = { 0 };
	int size = 0;
	FILE* file = fopen(pkg.username, "r");
	if (file)
	{
		size = filelength(fileno(file));
		fclose(file);
	}
	int2string(size, file_size);
	strcpy(check_pkg.content + content_len + 1, file_size);
	cout << check_pkg.content << endl;
}

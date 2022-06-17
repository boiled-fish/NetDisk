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
	MD5 �� filename ��ϣ:
	e1507aecc719c255660be912bcbc541c
	CertUtil: -hashfile ����ɹ���ɡ�
	*/
	if (cmd_res[0] == 'M') {
		strcpy(md5, cmd_res.substr(14 + f.size(), 32).c_str());
	}
	/*
	CertUtil: -hashfile ʧ��: 0x80070002 (WIN32: 2 ERROR_FILE_NOT_FOUND)
	CertUtil: ϵͳ�Ҳ���ָ�����ļ���
	*/
	else {
		return false;
	}
	return true;
}
//���鷴ת����
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
	char temp[2] = { 0 };//ת����������Ҫ�ݴ�����	
	int i;

	while (b % 10)//�����λ����ֵ����������û�к�������ֵ���Լ��㣬whileѭ������
	{
		char a = b % 10 + '0';//������ת�����ַ�
		temp[0] = a;//����ת�����ַ��ݴ���temp������
		b = b / 10;//�����������λ��λ����ֵ����ת�����ַ�����Ҫ����10��ʮλ���ϵ���ֵ�ŵ����λ��λ��
		strcat(c, temp);//��temp���ݴ��׷�ӵ�c��
	}
	reverse(c, strlen(c));//����ת����ɺ�ת����	
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
	/*�����ļ���С*/ 
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

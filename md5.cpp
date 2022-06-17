#pragma comment(lib, "ws2_32.lib") 
#include <iostream>
#include <string>
#include <cstring>
#include "../../include/client.h"
using namespace std;

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

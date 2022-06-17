#include <iostream>
#include <string>
#include <cstring>
#include <io.h>
#include <vector>
using namespace std;

void getFiles(string path, vector<string>& files)
{
	//�ļ����
	long hFile = 0;
	//�ļ���Ϣ
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮
			//�������,�����б�
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					cout << p.assign(path).append("\\").append(fileinfo.name) << endl;
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
			}
			else
			{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
 
		_findclose(hFile);
	}
}
int main() {
	string path;
	cin >> path;
	vector<string> files;
	getFiles(path, files);
	for (int i = 0; i < files.size(); ++ i) {
		cout << files[i] << endl;
	}
}

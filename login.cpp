// login.cpp
// 2022.6.10
// author: boiled_fish
#include <iostream>
#include <string>
#include <mysql/mysql.h>
using namespace std;

#define MAX_COMMAND_SIZE 256

class User
{
    private:
        string username;
        string rootdir;
        string paste_path;
    public:
        User(string user_name)
        {
            username = user_name;
            //rootdir = 
        }
        //ɾ��·��Ϊfile_path���ļ�
        void delete_file(string file_path)
        {
            char command[MAX_COMMAND_SIZE] = {0};

            sprintf(command,"rm -rf %s",file_path.c_str());
            system(command);
            return;
        }
        //չʾpath·���µ��ļ�
        void show(string path, vector<string>& files)
        {
            /*ֱ����ls���ȱ�����޷��ֳ���Щ���ļ�����Щ���ļ���*/
            // char command[MAX_COMMAND_SIZE] = {0};

            // sprintf(command,"ls %s",path.c_str());
            // system(command);
            // return;

            /*��C++�ļ��ķ���*/
            DIR *dir;
	        struct dirent *ptr;
            dir = opendir(path.c_str());
            while((ptr = readdir(dir)) != 0)
            {
                //������ǰĿ¼�͸�Ŀ¼
                if(strcmp(ptr->d_name,".") == 0 || strcmp(ptr->d_name,"..") == 0)
                {
                    continue;
                }
                //�ж��Ƿ����ļ���
                if(int(ptr->d_type)==4)     //�ļ���
                {
                    cout<<ptr->d_name<<"��һ���ļ���"<<endl;
                    files.push_back(string(ptr->d_name));
                }
                else                        //�ļ�
                {
                    cout<<ptr->d_name<<"��һ���ļ�"<<endl;
                    files.push_back(string(ptr->d_name));
                }
            }
            closedir(dir);
        }
        //��start path�ƶ���destination path
        void move(string start_path, string destinaion_path)
        {
            char command[MAX_COMMAND_SIZE] = {0};
            sprintf(command,"mv -f %s %s",start_path.c_str(),destinaion_path.c_str());
            system(command);
            return;
        }

        void copy(string path)
        {
            paste_path = path;
            return;
        }
        //��start path�ļ�ճ����destination path
        void paste(string start_path, string destinaion_path)
        {
            char command[MAX_COMMAND_SIZE] = {0};
            sprintf(command,"cp -rf %s %s",start_path.c_str(),destinaion_path.c_str());
            system(command);
            return;
        }
        void download(string file_path, string destinaion_path)
        {
            return;
        }
        void upload()
        {
            return;
        }
};

User* pan_login(MYSQL* mysql, string username, string pwd) {
	/*��������*/
	char msg[BUF_LEN] = { 0 };
	int is_login_correct = 0;
	User empty_user("");
	
	/*�����û�*/ 
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "SELECT*FROM user WHERE user_name='%s'", username);
	myquery(mysql, msg);
	MYSQL_RES* result_usr = mysql_store_result(mysql);
	row = mysql_fetch_row(result_usr);

	/*���ؿն��󣬱�ʾ�û������� */ 
	if (row == NULL) {
		mysql_free_result(result_usr);
		return empty_user; 
	}
	mysql_free_result(result_usr);

	/*��֤������ȷ��*/
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "SELECT*FROM user WHERE user_pwd=md5('%s')", pwd);
	myquery(mysql, msg);
	MYSQL_RES* result_pwd = mysql_store_result(mysql);
	while ((row = mysql_fetch_row(result_pwd)) != NULL) {
		//�������
		if (row == NULL) {
			mysql_free_result(result_pwd);
			return empty_user;
		}
		//������ȷ
		else if (!strcmp(row[0], username)) {
			mysql_free_result(result_pwd);
			islogincorrect = 1;
			break;
		}
	}
	if (!islogincorrect) {
		mysql_free_result(result_pwd);
		return empty_user;
	}
	
	/*���User����*/
	//string root_dir = "/home/net_disk/";
	//root_dir += username;
	
	return User(username);
}

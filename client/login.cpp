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
        //删除路径为file_path的文件
        void delete_file(string file_path)
        {
            char command[MAX_COMMAND_SIZE] = {0};

            sprintf(command,"rm -rf %s",file_path.c_str());
            system(command);
            return;
        }
        //展示path路径下的文件
        void show(string path, vector<string>& files)
        {
            /*直接用ls命令，缺点是无法分出哪些是文件，哪些是文件夹*/
            // char command[MAX_COMMAND_SIZE] = {0};

            // sprintf(command,"ls %s",path.c_str());
            // system(command);
            // return;

            /*用C++文件的方法*/
            DIR *dir;
	        struct dirent *ptr;
            dir = opendir(path.c_str());
            while((ptr = readdir(dir)) != 0)
            {
                //跳过当前目录和父目录
                if(strcmp(ptr->d_name,".") == 0 || strcmp(ptr->d_name,"..") == 0)
                {
                    continue;
                }
                //判断是否是文件夹
                if(int(ptr->d_type)==4)     //文件夹
                {
                    cout<<ptr->d_name<<"是一个文件夹"<<endl;
                    files.push_back(string(ptr->d_name));
                }
                else                        //文件
                {
                    cout<<ptr->d_name<<"是一个文件"<<endl;
                    files.push_back(string(ptr->d_name));
                }
            }
            closedir(dir);
        }
        //从start path移动到destination path
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
        //将start path文件粘贴到destination path
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
	/*变量定义*/
	char msg[BUF_LEN] = { 0 };
	int is_login_correct = 0;
	User empty_user("");
	
	/*查找用户*/ 
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "SELECT*FROM user WHERE user_name='%s'", username);
	myquery(mysql, msg);
	MYSQL_RES* result_usr = mysql_store_result(mysql);
	row = mysql_fetch_row(result_usr);

	/*返回空对象，表示用户不存在 */ 
	if (row == NULL) {
		mysql_free_result(result_usr);
		return empty_user; 
	}
	mysql_free_result(result_usr);

	/*验证密码正确性*/
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "SELECT*FROM user WHERE user_pwd=md5('%s')", pwd);
	myquery(mysql, msg);
	MYSQL_RES* result_pwd = mysql_store_result(mysql);
	while ((row = mysql_fetch_row(result_pwd)) != NULL) {
		//密码错误
		if (row == NULL) {
			mysql_free_result(result_pwd);
			return empty_user;
		}
		//密码正确
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
	
	/*填充User对象*/
	//string root_dir = "/home/net_disk/";
	//root_dir += username;
	
	return User(username);
}

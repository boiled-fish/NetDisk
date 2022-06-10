// login.cpp
// 2022.6.10
// author: boiled_fish
#include <iostream>
#include <string>
#include <mysql/mysql.h>
using namespace std;

class User{
private:
	string username;
	string root_dir;
public:
	User(string username, string root_dir);
	~User();

	string get_username();
	string get_rootdir();
};

User::User(string username, string root_dir) {
	this->username = username;
	this->root_dir = root_dir;
}
string User::get_username() {
	return this->username;
}
string User::get_rootdir() {
	return this->root_dir;
}

User* pan_login(MYSQL* mysql, string username, string pwd) {
	/*变量定义*/
	char msg[BUF_LEN] = { 0 };
	int is_login_correct = 0;
	User empty_user(NULL, NULL);
	
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
	string root_dir = "/home/net_disk/";
	root_dir += username;
	
	return User(username, root_dir);
}

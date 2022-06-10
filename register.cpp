// register.cpp
// 2022.6.10
// author: boiled_fish
#include <iostream>
#include <string>
#include <mysql/mysql.h>
using namespace std;

#define BUF_LEN 100

bool pan_register(MYSQL* mysql, string username, string pwd) {
	char msg[BUF_LEN] = { 0 };
	
	sprintf(msg, "INSERT INTO user VALUES('%s', '%s')", username.c_str(), pwd.c_str());
	if (mysql_query(mysql, msg)) {	
		//client端注册失败，用户已存在
		return false;
	}
	else {
		//创建用户文件表
		memset(msg, 0, sizeof(msg));
		sprintf(msg, "DROP TABLE IF EXISTS `%s`", username);
		myquery(mysql, msg);
		
		memset(msg, 0, sizeof(msg));
		sprintf(msg, "CREATE TABLE `%s`(`usrfile_id` INT(11) NOT NULL AUTO_INCREMENT,`usrfile_name` VARCHAR(128) NOT NULL,", username.c_str());
		sprintf(msg, "%s`usrfile_hashname` VARCHAR(128),`usrfile_fatherid` INT(11),`usrfile_isfile` TINYINT(1) NOT NULL,", msg);
		sprintf(msg, "%sPRIMARY KEY(`usrfile_id`)) ENGINE=InnoDB DEFAULT CHARSET=gbk AUTO_INCREMENT=1;", msg);
		myquery(mysql, msg);
		
		memset(msg, 0, sizeof(msg));
		sprintf(msg, "insert into %s values(NULL,'root',NULL,NULL,0)", username);
		myquery(mysql, msg);
		
		memset(msg, 0, sizeof(msg));
		sprintf(msg, "insert into %s values(NULL,'test',NULL,1,0)", username);
		myquery(mysql, msg);
	}
	//client端注册成功
	return true;
}

#include <iostream>
#include <sys/stat.h>
using namespace std;

struct data_package {
    int op;
    char username[100];
    char content[100];
};

int file_type(char *path) { 
	struct stat s;
	if (stat(path,&s)==0) {
  		if(s.st_mode & S_IFDIR) {
  			return 0;
  		}
		else if (s.st_mode & S_IFREG) {
    		return 1;
  		}
		else {
    		return 2;
  		}
	}	
	else {
  		return 2;	
	}
}

void test1(data_package* t) {
	cout << file_type(t->content);
}


bool upload_dir(data_package* pkg) {
	int len1 = strlen(pkg->username);
	int len2 = strlen(pkg->content);
	int i, pivot = 0;
	for (i = 0; i < len1; ++i) {
		if (pkg->username[i] == '\\') {
			pivot = i;
		} 
	}
	strcpy(pkg->content + len2, pkg->username + pivot + 1);
	cout << pkg->content;
}



int main() {
	data_package test;
	cin >> test.username;
	cin >> test.content;
	upload_dir(&test);
}

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <stdio.h>
using namespace std;

struct data_package {
    int op;
    char username[100];
    char content[100];
};

int main() {
	data_package pkg_snt;
	char path[100] = { 0 };
	cin >> path;
	ifstream infile;
	infile.open(path, ios::in);
	if (!infile.is_open()) {
		return -1;
	}
	infile.seekg(2);
	int count = 0;
	char c;
	while ((c = infile.get()) != EOF && count < 5) {
		pkg_snt.content[count++] = c;
	}
	cout << pkg_snt.content << endl;
}

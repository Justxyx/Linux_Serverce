//
// Created by xm on 2022/5/11.
//

#include <unistd.h>
#include <stdio.h>
#include <iostream>
using namespace std;

int main(){
    uid_t uid = getuid();
    uid_t euid = geteuid();
    cout << "uid:" << uid << endl;
    cout << "euid:" << euid << endl;
}
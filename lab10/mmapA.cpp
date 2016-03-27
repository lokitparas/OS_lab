//Part A
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits>
#include <string>
#include <string.h>
using namespace std;

void report(int arg){
  pid_t pid =getpid();
  string call1="cat /proc/"+to_string(pid)+"/status | grep VmSize";
  system(call1.c_str());
  string call2="cat /proc/"+to_string(pid)+"/status | grep VmRSS";
  system(call2.c_str());
  if(arg == 0)cout << "Initialized Execution, Press Enter to Continue";
  if(arg == 1)cout << "Memory Mapped, Press Enter to Continue";
  if(arg == 2)cout << "Char read, Press Enter to Continue";
  if(arg == 3)cout << "Char read at offset 10,000 , Press Enter to Continue";
  cin.ignore(std::numeric_limits<streamsize>::max(),'\n');
}

int main(){
  report(0);
  //Memory Map
  int fd = open("files/file0.txt", O_RDONLY);
  off_t f_size;
  f_size = lseek(fd, 0, SEEK_END);
  mmap(NULL, f_size, PROT_READ, MAP_PRIVATE, fd, 0);
  report(1);

  //Read first char
  char buff[10];
  bzero(buff, 10);
  lseek(fd, 0, SEEK_SET);
  int tmp = read(fd, buff, 1);
  cout <<"read char is " <<buff << endl;
  report(2);

  //Read 10,000th char
  bzero(buff, 10);
  lseek(fd, 10000, SEEK_SET);
  tmp =read(fd, buff, 1);
  cout <<"read char at 10000 offset is: " <<buff << endl;
  report(3);
  close(fd);
}
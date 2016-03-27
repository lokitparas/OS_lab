#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits>
#include <string>
#include <time.h>
#include <string.h>
#include <sys/time.h>
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

void mapped_read(int buffer_size){
  struct timeval start, end;
  gettimeofday(&start, NULL);  // start time for the experiment noted
  for(int  i = 0; i < 25; i++){
    string tmp = "files/file"+to_string(i)+".txt";
    int fd = open(tmp.c_str(), O_RDONLY);
    char buff[buffer_size];
    int nread;

    while(1)
        {
            /* First read file in chunks of 10 bytes */
            bzero(buff,buffer_size);
            nread = read(fd,buff,buffer_size);
            if (nread < buffer_size)
            {   
                break;
            }


        }
    close(fd);
  }
  gettimeofday(&end, NULL);  // start time for the experiment noted
  float throughput = 25*10.0*1000000/((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
  cout << "Throughput is: "<< throughput << endl;
}
void mapped_write(int buffer_size){
  struct timeval start, end;
  gettimeofday(&start, NULL);  // start time for the experiment noted
  for(int  i = 0; i < 25; i++){
    string tmp = "files/file"+to_string(i)+".txt";
    int fd = open(tmp.c_str(), O_RDWR);
    char buff[buffer_size];
    int nwrite;
    off_t f_size;
    f_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    int bytes_written = 0;
    for(int i = 0; i < buffer_size; i++){
      buff[i] = '0';
    }
    while(bytes_written < f_size){
      nwrite = write(fd, buff, buffer_size);
      bytes_written += buffer_size;
    }
    close(fd);
  }
  gettimeofday(&end, NULL);  // start time for the experiment noted
  float throughput = 25*10.0/((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
  cout << "Throughput is: "<< throughput << endl;
}



int main(){
  // report(0);
  // //Memory Map
  // FILE* fd = fopen("file.txt", "r");
  // off_t f_size;
  // f_size = lseek(fileno(fd), 0, SEEK_END);
  // mmap(NULL, f_size, PROT_READ, MAP_PRIVATE, fileno(fd), 0);
  // report(1);

  // //Read first char
  // char buff[10];
  // bzero(buff, 10);
  // fseek(fd, 0, SEEK_SET);
  // int tmp = fread(buff, 1, 1, fd);
  // cout <<"read char is " <<buff << endl;
  // report(2);

  // //Read 10,000th char
  // bzero(buff, 10);
  // fseek(fd, 10000, SEEK_SET);
  // tmp = fread(buff, 1, 1, fd);
  // cout <<"read char at 10000 offset is: " <<buff << endl;
  // report(3);

  int flag = 0;
  if(flag == 1){
    for(int  i = 0; i < 25; i++){
      string tmp = "file"+to_string(i)+".txt";
      int fd = open(tmp.c_str(), O_RDONLY);
      off_t f_size;
      f_size = lseek(fd, 0, SEEK_END);
      mmap(NULL, f_size, PROT_READ, MAP_PRIVATE, fd, 0);
      close(fd);
    }
    mapped_read(20);
  }
  else mapped_read(512);
  // mapped_write(20);
}
#include <sys/types.h>
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
char* array[25];
int sizes[25];
int size;
struct stat buf;
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

void mapped_read(){
  struct timeval start, end;
  gettimeofday(&start, NULL);  // start time for the experiment noted
  char tmp;
  
  // cout << f_size << endl;
  for(int  i = 0; i < 25; i++){
      for(int j=0;j<sizes[i];j++){
        tmp = array[i][j];
      }
      
  }
  gettimeofday(&end, NULL);  // start time for the experiment noted
  float throughput = 25*size/((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
  cout << "Throughput is: "<< throughput << endl;
}

void unmapped_read(int buffer_size){
  struct timeval start, end;
  gettimeofday(&start, NULL);  // start time for the experiment noted
  for(int  i = 0; i < 25; i++){
    string tmp = "files/file"+to_string(i)+".txt";
    int fd = open(tmp.c_str(), O_RDONLY);
    char buff[buffer_size];
    int nread;
    fstat(fd, &buf);
    size = buf.st_size;
      
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
  float throughput = 25*size/((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
  cout << "Throughput is: "<< throughput << endl;
}

void mapped_write(){
  struct timeval start, end;
  gettimeofday(&start, NULL);  // start time for the experiment noted
  for(int  i = 0; i < 25; i++){
    int bytes_written = 0;
    // cout << sizes[i] << endl; 
    while(bytes_written < sizes[i]){
      // nwrite = write(fd, buff, buffer_size);
      array[i][bytes_written]='0';
      bytes_written += 1;
    }
  }
  gettimeofday(&end, NULL);  // start time for the experiment noted
  float throughput = 25*size/((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
  cout << "Throughput is: "<< throughput << endl;
}

void unmapped_write(int buffer_size){
  struct timeval start, end;
  gettimeofday(&start, NULL);  // start time for the experiment noted
  for(int  i = 0; i < 25; i++){
    string tmp = "files/file"+to_string(i)+".txt";
    int fd = open(tmp.c_str(), O_RDWR);
    char buff[buffer_size];
    int nwrite;
    fstat(fd, &buf);
    size = buf.st_size;

    int bytes_written = 0;
    for(int i = 0; i < buffer_size; i++){
      buff[i] = '0';
    }
    while(bytes_written < size){
      nwrite = write(fd, buff, buffer_size);
      bytes_written += buffer_size;
    }
    close(fd);
  }
  gettimeofday(&end, NULL);  // start time for the experiment noted
  float throughput = 25*size/((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec));
  cout << "Throughput is: "<< throughput << endl;
}



int main(){

  int flag = 0;
  if(flag == 1){
    
    for(int  i = 0; i < 25; i++){
      string tmp = "./files/file"+to_string(i)+".txt";
      int fd = open(tmp.c_str(), O_RDWR);
      fstat(fd, &buf);
      sizes[i] = buf.st_size;
      size = sizes[i];
      // f_size = lseek(fd, 0, SEEK_END);
      // cout << sizes[i] << endl;
      array[i] = (char*)mmap(NULL, sizes[i], PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
      // cout << "poop" << endl;
      close(fd);
    }
    // mapped_read();
    mapped_write();

    for(int i=0; i<25; i++){
      msync(array[i],sizes[i],MS_SYNC);
      munmap(array[i],sizes[i]);
    }
  }
  else {
        // unmapped_read(512);
        unmapped_write(512);
      }
}
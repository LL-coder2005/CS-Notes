#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define BROADCAST_PORT 8888
#define BROADCAST_IP "192.168.255.255"

using namespace std;
int main(){
    
    int cfd = socket(AF_INET,SOCK_DGRAM,0);
    if(cfd==-1){
        perror("socket error : ");
        return -1;
    }
    
    sockaddr_in cin;
    cin.sin_family = AF_INET;
    cin.sin_port = htons(BROADCAST_PORT);
    cin.sin_addr.s_addr = inet_addr(BROADCAST_IP);
    if(bind(cfd,(sockaddr*)&cin,sizeof(cin))==-1){
        perror("bind error:");
        return -1;
    }

    char rbuf[128]="";
    while(1){
        cout << "1" << endl;
        bzero(rbuf,sizeof(rbuf));
        int res = recvfrom(cfd,rbuf,sizeof(rbuf),0,nullptr,nullptr);
        cout << "广播通知: " << rbuf << endl;
    }

    close(cfd);
    
    return 0;
    
}
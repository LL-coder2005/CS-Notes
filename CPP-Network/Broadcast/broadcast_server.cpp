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
    int sfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sfd==-1){
        perror("socket error: ");
        return -1;
    }
    //设置套接字为允许广播
    int broad = 1;
    if(setsockopt(sfd,SOL_SOCKET,SO_BROADCAST,&broad,sizeof(broad))==-1){
        perror("setsockopt error : ");
        return -1;
    }
    cout << "套接字设置成功！" << endl;

    sockaddr_in rin;
    rin.sin_family = AF_INET;
    rin.sin_port = htons(BROADCAST_PORT);
    rin.sin_addr.s_addr = inet_addr(BROADCAST_IP);

    char buf[128]="";

    while(1){
        bzero(buf,sizeof(buf));
        fgets(buf,sizeof(buf),stdin);
        buf[strlen(buf)-1]=0;
        if(sendto(sfd,buf,sizeof(buf),0,(sockaddr*)&rin,sizeof(rin))==-1){
            perror("send error : ");
            return -1;
        }
        cout << "send success" << endl;
    }

    close(sfd);
    return 0;
        
}

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

using namespace std;

int main(){
    int sfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sfd==-1){
        perror("socket error");
        return -1;
    }
    //绑定ip地址和端口号(可选)


    //将组播地址填入结构体中
    sockaddr_in rin;
    rin.sin_family = AF_INET;
    rin.sin_port = htons(8888);
    rin.sin_addr.s_addr = inet_addr("224.1.2.3");

    char wbuf[128]="";
    while(1){
        bzero(wbuf,sizeof(wbuf));
        fgets(wbuf,sizeof(wbuf),stdin);
        wbuf[strlen(wbuf)-1]=0;
        if(sendto(sfd,wbuf,sizeof(wbuf),0,(sockaddr*)&rin,sizeof(rin))==-1){
            perror("sendto error : ");
            return -1;
        }
    }

    close(sfd);
    return 0;
}

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

using namespace std;
int main(){
    int rfd = socket(AF_INET,SOCK_DGRAM,0);
    if(rfd==-1){
        perror("socket error:");
        return -1;
    }

    //将套接字加入多播组

    struct ip_mreqn ipr;
    //组播地址
    ipr.imr_multiaddr.s_addr = inet_addr("224.1.2.3");
    //本机地址
    ipr.imr_address.s_addr = inet_addr("192.168.255.128");
    //网卡编号，使用ip ad查看，一般为2
    ipr.imr_ifindex = 2;
    if(setsockopt(rfd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&ipr,sizeof(ipr))==-1){
        perror("setsockopt error:");
        return -1;
    }

    //绑定ip地址(组播地址)和端口号
    struct sockaddr_in rin;
    rin.sin_family = AF_INET;
    rin.sin_port = htons(8888);
    rin.sin_addr.s_addr = inet_addr("224.1.2.3");
    if(bind(rfd,(sockaddr*)&rin,sizeof(rin))==-1){
        perror("bind error");
        return -1;
    }

    char rbuf[128]="";
    while(1){
        bzero(rbuf,sizeof(rbuf));
        if(recvfrom(rfd,rbuf,sizeof(rbuf),0,nullptr,nullptr)==-1){
            perror("recvfrom error:");
            return -1;
        }
        cout << "multicast issue >>" << rbuf << endl;
    }

    close(rfd);
    return 0;
}
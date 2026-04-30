//新加 #include <sys/un.h> 头文件

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>

using namespace std;
int main(){
    int sndfd = socket(AF_UNIX,SOCK_STREAM,0);
    if(sndfd==-1){
        perror("socket error : ");
        return -1;
    }

    if(access("./unix",F_OK)==0){
        if(unlink("./unix")==-1){
            perror("unlink error : ");
            return -1;
        }
    }

    struct sockaddr_un sun;
    sun.sun_family = AF_UNIX;
    strcpy(sun.sun_path,"./unix");
    if(bind(sndfd,(sockaddr*)&sun,sizeof(sun))==-1){
        perror("bind error ");
        return -1;
    }

    if(listen(sndfd,128)==-1){
        perror("listen error");
        return -1;
    }

    sockaddr_un cin;
    socklen_t socklen = sizeof(cin);

    int newfd = accept(sndfd,(sockaddr*)&cin,&socklen);
    if(newfd==-1){
        perror("accept error:");
        return -1;
    }

    char buf[128]="";
    while(1){
        bzero(buf,sizeof(buf));
        int res = recv(newfd,buf,sizeof(buf),0);
        if(res==0){
            cout << "对端已下线！"<< endl;
            close(newfd);
            break;
        }
        cout <<"["<< cin.sun_path <<"]"<<">>"<<buf<<endl;
        strcat(buf,"***v***");
        if(send(newfd,buf,sizeof(buf),0)==-1){
            perror("send error:");
            return -1;
        }
        cout << "send succes" << endl;
    }

    close(sndfd);

    return 0;

}
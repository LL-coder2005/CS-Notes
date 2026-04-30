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
    int cfd = socket(AF_UNIX,SOCK_STREAM,0);
    if(cfd==-1){
        perror("socket error:");
        return -1;
    }

    if(access("./linux",F_OK)==0){
        if(unlink("./linux")==-1){
            perror("unlink error : ");
            return -1;
        }
    }
    sockaddr_un cun;
    cun.sun_family = AF_UNIX;
    strcpy(cun.sun_path,"./linux");
    if(bind(cfd,(sockaddr*)&cun,sizeof(cun))==-1){
        perror("bind error:");
        return -1;
    }

    sockaddr_un sun;
    sun.sun_family = AF_UNIX;
    strcpy(sun.sun_path,"./unix");

    if(connect(cfd,(sockaddr*)&sun,sizeof(sun))==-1){
        perror("connect error:");
        return -1;
    }
    char wbuf[128]="";
    while(1){

        bzero(wbuf,sizeof(wbuf));
        fgets(wbuf,sizeof(wbuf),stdin);
        wbuf[strlen(wbuf)-1]=0;
        if(send(cfd,wbuf,sizeof(wbuf),0)==-1){
            perror("send error:");
            return -1;
        }
        int res = recv(cfd,wbuf,sizeof(wbuf),0);
        cout << "服务器来信>>>" << wbuf << endl;

    }

    close(cfd);

    return 0;
}
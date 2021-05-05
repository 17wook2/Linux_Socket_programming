#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>

int tcp_listen(int port, int backlog);
void error_handling(char *msg);

int main(int argc, char* argv[]){

  FILE *file;
  int file_size;
  int chunk;
  char buffer[128000];
  int trasfer_count;
  int transfered_size;
  transfered_size = 0;
  char *file_name = argv[2]; // 파일명
	if((file = fopen(file_name, "rb"))==NULL)
    error_handling("[-]File Opening Error");
  printf("\n[+]File Opened\n");
  
	fseek (file , 0 , SEEK_END); // EOF 찾기
  file_size = ftell (file);
  fseek(file,0,SEEK_SET);

  int serverfd;
  int clientfd;

  char answer;
  char download_message[] = "press Y if you want to Download";
  
  struct sockaddr_in client_address;
  socklen_t client_address_size;
  /*
  server_socket 연결
  */
  serverfd = tcp_listen(atoi(argv[1]), 5);

  client_address_size = sizeof(client_address);
  clientfd = accept(serverfd, (struct sockaddr *)&client_address, &client_address_size);
  if(clientfd == -1)
    error_handling("[-] Accept Error");
  printf("[+]accepted.\n\n");

  send(clientfd, download_message, sizeof(download_message),0);

  recv(clientfd, &answer, sizeof(answer), 0);

  if(answer == 'Y' || answer == 'y'){
    send(clientfd, (char*)&file_size, sizeof(int),0);
  double c;
  while(1){
    
    chunk = fread(buffer, sizeof(char), 128000, file);
    send(clientfd,buffer,chunk,0);

    trasfer_count += 1;
    transfered_size += chunk;
    c = (double)transfered_size * 100 / (double) file_size;
    printf("(#%d)has been sent. %f percent has been transfered\r", trasfer_count, c);

    if(feof(file))
      break;
  }
  printf("\n %dbytes has been transferred\n",file_size);
  fclose(file);
  }
  else
    printf("download Failed");

  close(serverfd);
  close(clientfd);

  return 0;
}
  

// listen 소켓 생성 및 listen
int tcp_listen(int port, int backlog)
{
  int listen_socket;
  struct sockaddr_in servaddr;

  listen_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_socket == -1)
    error_handling("socket creation fail");
  printf("[+]listen_socket created\n");

  // servaddr 구조체의 내용 세팅
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  // binding
  if (bind(listen_socket, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
  {
    error_handling("binding fail");
  }
  printf("[+]binding successful\n");
  printf("[+]listening....\n");
  // 클라이언트로부터 연결요청을 기다림
  listen(listen_socket, backlog);
  return listen_socket;
}

void error_handling(char *msg)
{
  printf("error: %s", msg);
  exit(1);
}

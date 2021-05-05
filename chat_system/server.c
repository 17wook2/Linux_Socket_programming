#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define MAXLINE 1000

int tcp_listen(int address, int port, int k);
void error_handling(char *msg);
int main(int argc, char *argv[])
{
  int listen_sock; // 서버의 리슨 소켓
  struct sockaddr_in accp_sock_address;
  char bufmsg[MAXLINE]; // 메시지부분
  char fullbuffer[MAXLINE + 30];
  int accp_sock;
  socklen_t size;
  fd_set read_fds; //읽기를 감지할 fd_set 구조체
  time_t ct;
  struct tm tm;

  if (argc != 2)
  {
    printf("사용법 :%s port\n", argv[0]);
    exit(0);
  }
  size = sizeof(struct sockaddr_in);
  listen_sock = tcp_listen(INADDR_ANY, atoi(argv[1]), 5);
  accp_sock = accept(listen_sock, (struct sockaddr *)&accp_sock_address, &size);
  if (accp_sock != -1)
  {
    printf("connection successful\n");
    printf("Client Info : IP %s, Port %d\n", inet_ntoa(accp_sock_address.sin_addr), ntohs(accp_sock_address.sin_port));
  }

  while (1)
  {
    FD_ZERO(&read_fds);
    FD_SET(0, &read_fds);
    FD_SET(accp_sock, &read_fds);
    // if (accp_sock == -1)
    //   errquit("accept fail");
    fprintf(stderr, "\033[1;33m"); //글자색을 노란색으로 변경
    if (select(accp_sock + 1, &read_fds, NULL, NULL, NULL) < 0)
      printf("select Fail");
    if (FD_ISSET(accp_sock, &read_fds))
    {
      int nbyte;
      if ((nbyte = recv(accp_sock, bufmsg, MAXLINE, 0)) > 0)
      {
        bufmsg[nbyte] = 0;
        // write(1, "\033[0G", 4);        //커서의 X좌표를 0으로 이동
        fprintf(stderr, "\033[1;32m"); //글자색을 녹색으로 변경
        printf("%s", bufmsg);          //메시지 출력
        fprintf(stderr, "\033[1;33m"); //글자색을 노란색으로 변경
      }
      fprintf(stderr, "Server>"); //내 닉네임 출력
    }
    if (FD_ISSET(0, &read_fds))
    {
      fprintf(stderr, "Server>"); //내 닉네임 출력
      if (fgets(bufmsg, MAXLINE, stdin))
      {
        ct = time(NULL); //현재 시간을 받아옴
        tm = *localtime(&ct);
        sprintf(fullbuffer, "[%02d:%02d:%02d]From Server : %s", tm.tm_hour, tm.tm_min, tm.tm_sec, bufmsg); //메시지에 현재시간 추가
        if (send(accp_sock, fullbuffer, strlen(fullbuffer), 0) < 0)
          puts("Error : Write error on socket.");
      }
    }
  }
  close(accp_sock);
  close(listen_sock);
  return 0;
}

// listen 소켓 생성 및 listen
int tcp_listen(int address, int port, int k)
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
  servaddr.sin_addr.s_addr = htonl(address);
  servaddr.sin_port = htons(port);

  // binding
  if (bind(listen_socket, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
  {
    error_handling("binding fail");
  }
  printf("[+]binding successful\n");
  printf("[+]listening....\n");
  // 클라이언트로부터 연결요청을 기다림
  listen(listen_socket, k);
  return listen_socket;
}
void error_handling(char *msg)
{
  printf("error: %s", msg);
  exit(1);
}
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
#include <pthread.h>

#define MAXLINE 1000
#define MAX_SOCK 1024 // 솔라리스의 경우 64

int tcp_listen(int address, int port, int backlog);
int main(int argc, char *argv[])
{
  int listen_sock; // 서버의 리슨 소켓
  struct sockaddr_in cliaddr;
  char bufmsg[MAXLINE]; // 메시지부분
  char bufall[MAXLINE];
  // char buf[MAXLINE + 1]; //클라이언트에서 받은 메시지
  int nbyte, accp_sock, addrlen = sizeof(struct sockaddr_in);
  fd_set read_fds; //읽기를 감지할 fd_set 구조체
  pthread_t a_thread;

  if (argc != 2)
  {
    printf("사용법 :%s port\n", argv[0]);
    exit(0);
  }

  // init_listenSocket(address, port, backlog) 함수 호출
  listen_sock = tcp_listen(INADDR_ANY, atoi(argv[1]), 5);
  accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen);
  //스레드 생성
  // pthread_create(&a_thread, NULL, thread_function, (void *)NULL);

  while (1)
  {
    FD_ZERO(&read_fds);
    FD_SET(0, &read_fds);
    FD_SET(accp_sock, &read_fds);
    // if (accp_sock == -1)
    //   errquit("accept fail");

    if (select(accp_sock + 1, &read_fds, NULL, NULL, NULL) < 0)
      printf("select Fail");
    if (FD_ISSET(accp_sock, &read_fds))
    {
      int nbyte;
      if ((nbyte = recv(accp_sock, bufmsg, MAXLINE, 0)) > 0)
      {
        bufmsg[nbyte] = 0;
        write(1, "\033[0G", 4);        //커서의 X좌표를 0으로 이동
        printf("%s", bufmsg);          //메시지 출력
        fprintf(stderr, "\033[1;32m"); //글자색을 녹색으로 변경
      }
    }
    if (FD_ISSET(0, &read_fds))
    {
      if (fgets(bufall, MAXLINE, stdin))
      {
        // fprintf(stderr, "\033[1;33m"); //글자색을 노란색으로 변경
        // fprintf(stderr, "\033[1A");    //Y좌표를 현재 위치로부터 -1만큼 이동
        // printf("%s", bufmsg); //메시지에 현재시간 추가
        if (send(accp_sock, bufall, strlen(bufall), 0) < 0)
          puts("Error : Write error on socket.");
      }
    }
  }

  return 0;
}

// listen 소켓 생성 및 listen
int tcp_listen(int address, int port, int backlog)
{
  int listen_socket;
  struct sockaddr_in servaddr;

  listen_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_socket == -1)
  {
    perror("socket creation fail");
    exit(1);
  }
  // servaddr 구조체의 내용 세팅
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(address);
  servaddr.sin_port = htons(port);

  // binding
  if (bind(listen_socket, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
  {
    perror("binding fail");
    exit(1);
  }
  // 클라이언트로부터 연결요청을 기다림
  listen(listen_socket, backlog);
  return listen_socket;
}
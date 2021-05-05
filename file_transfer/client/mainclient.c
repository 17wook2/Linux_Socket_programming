#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
int tcp_connect(char* address, int port);
void error_handling(char *msg);

int main(int argc, char *argv[])
{

  int clientfd;
  struct sockaddr_in serv_addr;

  char* gretting = "client connected"; 
  char chat[100];                  
  char buffer[128000];                  

  int n;             // recv 리턴 값 저장
  int cnt = 0;       // 바이트 전달 횟수 저장
  int true_file_size; // 원본 동영상 파일 크기
  int file_size;     // 받은 동영상 파일 크기
  char answer;    

  clock_t begin;
  clock_t end;
  double time_spent;

  clientfd = tcp_connect(argv[1],atoi(argv[2]));

  recv(clientfd, chat, sizeof(chat), 0);
  printf("Download File from server: %s. [Y/N]\n", chat);

  scanf("%c", &answer);
  send(clientfd, &answer, sizeof(answer), 0);

  printf("\n");

  /* Step 5. 파일 사이즈 받기(파일을 다 쓰고 난 후 비교용)*/
  recv(clientfd, &true_file_size, sizeof(int), 0);

  /* 파일을 받겠다고 했을 때, 저장 및 쓰기 시작 */
  if (answer == 'Y' || answer == 'y')
  {
    /*--- 핵심: 파일 받고 쓰는 부분 ---*/
    FILE *file = fopen("downloaded.mp4", "wb");
    begin = clock();
    while ((n = recv(clientfd, buffer, 128000, 0)) != 0)
    {
      fwrite(buffer, sizeof(char), n, file);
      cnt += 1;
      printf("from server : %4dbytes is downloading(:#%d)...\r", n, cnt);
    }
    end = clock();
    /* 새로 쓴 파일 사이즈 측정 */
    file_size = ftell(file);

    fclose(file);
    /*--- // 파일 받고 쓰기 완료 ---*/
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("\ndownload time : %fs", time_spent);

    /* 원본 파일의 사이즈와 비교해서 크기가 같으면 성공 메시지 출력, 아니라면 실패 메시지 출력 */
    
    if (file_size == true_file_size)
      printf("\n\ncomplete: Total size is %d bytes.\n", file_size);
    else
      printf("\n\nfailed: size is different.\n");
  }
  /* 파일을 받지 않는다면 종료 */
  else
    printf("termination.\n");

  /* 소켓 닫기 */
  close(clientfd);
  return 0;
}

// connect 소켓 생성 및 connect
int tcp_connect(char* address, int port)
{
  printf("%s",address);
  printf("%d",port);
  int connect_socket;
  struct sockaddr_in servaddr;

  connect_socket = socket(PF_INET, SOCK_STREAM, 0);
  if (connect_socket == -1)
    error_handling("socket creation fail");
  printf("\n[+]connect_socket created\n");

  // servaddr 구조체의 내용 세팅
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(address);
  servaddr.sin_port = htons(port);

  if (connect(connect_socket, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    return -1;
  printf("\n[+]connection success\n");
  return connect_socket;
}

void error_handling(char *msg)
{
  printf("error: %s", msg);
  exit(1);
}

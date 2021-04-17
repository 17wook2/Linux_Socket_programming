#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // close function

#define BUF_SIZE 1024
int main(int argc, char *argv[])
{
  int client_socket;
  struct sockaddr_in server_address;
  char message[1024] = {
      0x00,
  };
  client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket == -1)
  {
    printf("Could not create socket");
  }
  memset(&server_address, 0, sizeof(server_address)); // 메모리에 주소 0으로 초기화
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(argv[1]);
  server_address.sin_port = htons(atoi(argv[2]));

  if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
  {
    printf("Could not connect");
  }
  if (read(client_socket, message, sizeof(message) - 1) == -1)
  {
    printf("Could not read");
  }
  printf("%s\n", message);

  close(client_socket);
  return 0;
}
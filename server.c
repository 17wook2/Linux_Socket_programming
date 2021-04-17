#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // for close
#define BUF_SIZE 1024
int main(int argc, char *argv[])
{
  int server_socket, client_socket;
  struct sockaddr_in server_address, client_address;
  socklen_t size;
  int recv_length;
  char buffer[BUF_SIZE];
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1)
  {
    printf("Could not create socket");
  }
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = 0;
  server_address.sin_port = htons(atoi(argv[1]));
  memset(&(server_address.sin_zero), 0, 8); // 메모리에 주소 0으로 초기화

  bind(server_socket, (struct sockaddr *)&server_address, sizeof(struct sockaddr));

  listen(server_socket, 3);
  printf("server opened, waiting for client");
  while (1)
  {
    size = sizeof(struct sockaddr_in);
    client_socket = accept(server_socket, (struct sockaddr *)&client_address, &size);

    send(client_socket, "Hello Client", 20, 0);
    printf("Client Info : IP %s, Port %d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    recv_length = recv(client_socket, &buffer, BUF_SIZE, 0);
    while (recv_length > 0)
    {
      printf("From Client : %s\n", buffer);
      recv_length = recv(client_socket, &buffer, BUF_SIZE, 0);
    }
    close(client_socket);
  }

  return 0;
}
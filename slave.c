#include <stdio.h> 
#include <math.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 

#define THIS_PORT 8888
#define SERVER_PORT 8080 
#define SERVER_ADDR "127.0.0.1"

// Function to calculate the integral
double integral(double gap, double discretization)
{

  // Trapeze Rule:
  // h = (b - a) / N

  double total = 0;
  double xa = 0;
  double xb = 0;

  xa = sqrt((100 * 100) - (gap * gap));
  xb = sqrt((100 * 100) - ((gap + discretization) * (gap + discretization)));

  total = ((xa + xb) * discretization) / 2;

  return total;
}

int main(int argc, char const *argv[]) 
{   
    // Socket-Slave variables
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    char *hello = "Hello from client"; 
    char buffer[1024] = {0}; 

    double gap = 0;
    double discretization = 0;
    double result = 0;

    fprintf(stdout, "Starting Client ...\n");

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 

    /* Defines the connection properties */
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(SERVER_PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, SERVER_ADDR, &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    while(1) {
      if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
      { 
          printf("\nConnection Failed \n"); 
          return -1; 
      } 

      char* message;

      // Handshacking
      if(result == 0){
        message = "Hello, i'm your slave!";
      }
      else {
        message = "20";
      }

      send(sock , message , strlen(message) , 0 ); 
      printf("[CLIENT] -> Datagrama enviado: %s\n",hello); 

      valread = read( sock , buffer, 1024); 
      printf("[CLIENT] <- Datagrama recebido: %s\n",buffer ); 
    }
    return 0; 
} 
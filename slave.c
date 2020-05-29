#include <stdio.h> 
#include <math.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 

#define masterPort 8080

int main(int argc, char const *argv[]) {   

  // Criação do sokcet
  int networkSocket;
  networkSocket = socket(AF_INET, SOCK_STREAM, 0);

  // Especificações do socket de conexão 
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(masterPort);  // Converte pra endereço de porta
  serverAddress.sin_addr.s_addr = INADDR_ANY;  // INADDR_NAY = 0.0.0.0

  //Função de coneção com o socket
  // @Param 'int': Identificação do socket de comunicação
  // @Param 'struck pointer': ponteiro para o endereço(estrutura) do socket
  // @Param 'integer': tamanho do endereço 
  int connectionStatus = connect(networkSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

  // Verifica possiveis erros de conexão
  if(connectionStatus == -1){
    printf("Erro ao conectar com o socket do servidor MASTER \n");
  }

  // Armazena a mensagem recebida MASTER
  char serverResponse[256];

  // Armazena a mensagem de envio para o MASTER
  char slaveMessage[256] = "Hello, i'm your SLAVE!";

  // Envia mensagem para o Master
  send(networkSocket, slaveMessage, sizeof(slaveMessage), 0);
  printf("[SLAVE] -> Enviou: %s\n", slaveMessage);

  //Recebendo dados do MASTER após conexão
  // @Param: 'int'   : Identificação do socket de comunicação
  // @Param: 'char*' : Buffer de armazenamento de resposta
  // @Param: 'int'   : Tamanho da msg de resposta
  // @Param: 'int'   : Opcional
  recv(networkSocket, &serverResponse, sizeof(serverResponse), 0);

  // Envia a mensagem para o servidor como "resposta"
  printf("[SLAVE] <- Recebeu: %s\n", serverResponse);


  // Finzaliza a conexão
  close(networkSocket);

  return 0;
}
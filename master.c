#include <stdio.h> 
#include <math.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 

#define masterPort 8080

int main(int argc, char const *argv[]) {  

  // Mensagem enviado quando estabelece uma nova conexão
  char helloMessage[256] = "Conexão estabelecida!";

  // Usado para identificar conexões no socket
  int conectionBacklog = 5;

  // Criação do sokcet
  int serverSocket;
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);

  // Especificações do socket de conexão 
  struct sockaddr_in serverAdress;
  serverAdress.sin_family = AF_INET;
  serverAdress.sin_port = htons(8080);        // Converte pra endereço de porta
  serverAdress.sin_addr.s_addr = INADDR_ANY;  // INADDR_NAY = 0.0.0.0

  // Bind(Conexão/Ligação) com um socket de IP e PORTA especifico 
  // Similar aos parametros de conexão com do SLAVE para o MASTER
  bind(serverSocket, (struct sockaddr *) &serverAdress, sizeof(serverAdress));

  // Escuta conexãoes na rede
  // @Param: 'struct' : Struct do socket utilizado
  // @Param: 'int' : Backlog de numero de conexões
  listen(serverSocket, conectionBacklog);

  // Armazena a identificação do socket do Client(SLAVE)
  int clientSocket; 

  // Quando aceita uma nova conexão com o MASTER
  // @Param: 'int' : Socket de conexão principal
  // @Param: 'struct' : Armazena a informação de conexão do Client(SLAVE) em uma struct
  // @Param: 'int' : Tamanho da estrutura declarada a cima
  clientSocket = accept(serverSocket, NULL, NULL);

  // Envia mensagem para o Client(SLAVE) conectado e aceito
  // @Param: 'int' : Socket referente ao client em questão
  // @Param: 'char*' : Mensagem a ser enviada
  // @Param: 'int' : Tamanho da msg a ser enviada
  // @Param: 'int' : Flag opcional
  send(clientSocket, helloMessage, sizeof(helloMessage), 0);

  // Armazena a mensagem do SLAVE
  char slaveMessage[256];

  //Recebendo dados do SLAVE
  // @Param: 'int'   : Identificação do socket de comunicação
  // @Param: 'char*' : Buffer de armazenamento de resposta
  // @Param: 'int'   : Tamanho da msg de resposta
  // @Param: 'int'   : Opcional
  recv(clientSocket, &slaveMessage, sizeof(slaveMessage), 0);

  // Finalização conexão do MASTER
  close(serverSocket);
  
  return 0;
}
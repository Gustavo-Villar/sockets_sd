// Alunos:
// Gabriel Henrique da Silva Gava
// Gustavo Henrique de Oliveira Villar
// Vitoria Zanon

#include <stdio.h>  
#include <string.h>     
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>    
#include <arpa/inet.h>    
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h> 

// Constantes da aplicação
#define masterPort 8080
#define slavesLimit 10
#define PATH_MAX 4096 

// Estrutura do server global (MASTER) 
struct sockaddr_in serverAddress;
struct sockaddr_in slaveAddress;  
int totalSlavesConnected = 0;
int slavesArray[slavesLimit];
int flag = 1;

// Variaveis da Integral
double discrization = 0.0001;
double gap = 0.0;
double total = 0.0;
double slaveReturn = 0.0;

// Buffer de Leitura / Escrita de dados
char inputBuffer[64];
char Buffer[64];

// Verifica erros nos passos do Socket
int check(int bool, const char *msg){
  if(bool == -1) {
    perror(msg);
    exit(1);
  }
}

// Inicia o servidor e fica escutando em uma porta em especifico
int setupServer(short serverPort, int serverBacklog) {
  // @Param serverPort: Porta que ficara escutando conexões
  // @Param serverBacklog: Backlog de conexões do servidor

  int serverSocket, clientSocket, addressSize;

  // Especificações do socket de conexão do MASTER
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(masterPort);  // Converte pra endereço de porta
  serverAddress.sin_addr.s_addr = INADDR_ANY;  // INADDR_NAY = 0.0.0.0
  
  // Armazena o "ids" do socket
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  check(serverSocket, "Falha ao criar o socket");

  // Bind(Conexão/Ligação) com um socket de IP e PORTA especifico 
  // Similar aos parametros de conexão com do SLAVE para o MASTER
  int bindFlag = bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
  check(bindFlag, "Erro ao realizar o Bind()");

  // Escuta conexãoes na rede
  // @Param: 'struct' : Struct do socket utilizado
  // @Param: 'int' : Backlog de numero de conexões
  int listenFlag = listen(serverSocket, serverBacklog);
  check(listenFlag, "Erro ao realizar Listen()");

  return serverSocket;
}

// Adiciona o novo socket do slave ao array
void addNewSlaveSocket(int slaveSokcet){
  int i;
  for(i = 0; i < slavesLimit; i++){
    if(slavesArray[i] == 0){
      slavesArray[i] = slaveSokcet;
      break;
    }
  }
}

// Inicializa o array com 0s
void initializeSlavesArray(){
  for(int i = 0; i < slavesLimit; i++){
    slavesArray[i] = 0;
  }
}

// Aceita uma nova conexão com um novo host(Slave)
int acceptNewConnection(int serverSokcet) {
  // @Param serverSocket : Socket que estão escutando as conexões

  // Tamanho do estrutura 
  int serverAddressSize = sizeof(serverAddress);

  // Armazena a identificação do socket do Client(SLAVE)
  int clientSocket;

  // Estrutura de endereço do Client
  struct sockaddr_in clientAddress;
  int clientAddrSize = sizeof(serverSokcet);

  // Quando aceita uma nova conexão com o MASTER
  // @Param: 'int' : Socket de conexão principal
  // @Param: 'struct' : Armazena a informação de conexão do Client(SLAVE) em uma struct
  // @Param: 'int' : Tamanho da estrutura declarada acima
  clientSocket = accept(serverSokcet,  (struct sockaddr *)&clientAddress, (socklen_t*)&clientAddrSize);
  check(clientSocket, "Erro ao estabelecer nova conexão!");
  printf("\n> Nova conexão estabelecida! FD[%d]-> %s:%d\n\n",
    clientSocket, 
    inet_ntoa(clientAddress.sin_addr), 
    ntohs(clientAddress.sin_port)
  );

  //Incrementa o total de slaves
  totalSlavesConnected++;
  addNewSlaveSocket(clientSocket);

  // Primeira mensagem enviada -> Envia o valor de discretização
  char helloMessage[256];
  sprintf(helloMessage, "%lf", discrization);
  if( send(clientSocket, helloMessage, strlen(helloMessage), 0) != strlen(helloMessage)){   
    perror("send");   
  }   
  printf("[MASTER] -> Enviou: (%s) -> SLAVE[%d]\n", helloMessage, clientSocket);
  
  return clientSocket;
}

// Troca de mensagens entre MASTER e SLAVE
void * reciveAndSendData(int serverSokcet, int clientSocket){
  // @Param serverSocket : Socket que estão escutando as conexões
  // @Param clientSocket : Socket que está transmitindo dados

  // Armazena a mensagem do SLAVE
  char slaveMessage[256];
  // Mensagem enviada quando estabelece uma nova conexão
  char helloMessage[256];

  // Envia mensagem para o Client(SLAVE) conectado e aceito
  // @Param: 'int' : Socket referente ao client em questão
  // @Param: 'char*' : Mensagem a ser enviada
  // @Param: 'int' : Tamanho da msg a ser enviada
  // @Param: 'int' : Flag opcional
  sprintf(helloMessage, "%lf", discrization);
  if( send(clientSocket, helloMessage, strlen(helloMessage), 0) != strlen(helloMessage)){   
      perror("send");   
  }   
  printf("[MASTER] -> Enviou: (%s) -> SLAVE[%d]\n", helloMessage, clientSocket);
    
  //Recebendo dados do SLAVE
  // @Param: 'int'   : Identificação do socket de comunicação
  // @Param: 'char*' : Buffer de armazenamento de resposta
  // @Param: 'int'   : Tamanho da msg de resposta
  // @Param: 'int'   : Opcional
  recv(clientSocket, &slaveMessage, sizeof(slaveMessage), 0);
  printf("[MASTER] <- Recebeu: (%s) <- SLAVE[%d]\n", slaveMessage, clientSocket);
  
}

int main(int argc, char const *argv[]) {  

  // Usado para identificar backlog de conexões no socket
  int conectionBacklog = 10;
  int slaveSockets[slavesLimit];

  // Usado para armazenar todos os Sockets de Slaves
  initializeSlavesArray();

  // Inicia a configuração do socket principal (MASTER)
  int masterSocket = setupServer(masterPort, conectionBacklog);
  printf("Servidor master criado com fd %d\n", masterSocket);    
  printf("Aguardando conexões ...\n");

  // Armazena o "maior" socket registrado
  int lastSocket = masterSocket;

  // Seleção dos descritores de arquivo (FD -> File Descriptor)
  fd_set readSockets;

  // Repetição infinita
  while(1){

    // Inicialização do "set" atual
    FD_ZERO(&readSockets);
    FD_SET(masterSocket, &readSockets);

    // Como select() é destrutivo, é necessario
    // atualizar o fd_set atualizar a cada loop
    for(int i = 0; i < slavesLimit; i++){

      // Caso seja um sd válido, adiciona para a readSockets
      if(slavesArray[i] > 0 ) FD_SET(slavesArray[i], &readSockets);
    
      // Atualiza o maior numero de socket para usar no select()
      if(slavesArray[i] > lastSocket) lastSocket = slavesArray[i];
      
    }

    // Select() :
    // @Param(1): Número máximo de FDs em fd_Set
    // @Param(2): FDs que verifica leitura
    // @Param(3): FDs que verifica escrita
    // @Param(2): Erros
    // @Param(5): Timeout
    
    check(select(10, &readSockets, NULL, NULL, NULL), "Erro de seleção de FD");

    // Verifica um nova conexão com o masterSocket
    if(FD_ISSET(masterSocket, &readSockets)){

      // Neste caso, significa que existe uma nova conexão
      int clientSocket = acceptNewConnection(masterSocket);
      FD_SET(clientSocket, &readSockets);
      if(clientSocket > lastSocket) { lastSocket = clientSocket; }
      addNewSlaveSocket(clientSocket);

    } 

    // Neste caso, como não é uma nova conexão, se trata de 
    // uma operação de I/O
    for(int i = 0; i < slavesLimit; i++){
          
      memset(inputBuffer, 0x0, 64);
      memset(Buffer, 0x0, 64);

      int slaveSocket = slavesArray[i];

      // Verificamos se este socket faz parte do "set"
      if(FD_ISSET(slaveSocket, &readSockets)){
        // Return : -1 = Error / 0 = EOF
        int readFlag = read(slaveSocket, Buffer, 1024);
        if( readFlag == 0) {
          printf("Resultado: %f\n", total);
          getpeername(
            slaveSocket, 
            (struct sockaddr*)&slaveAddress , 
            (socklen_t*)&slaveAddress
          );
          printf(
            "> Host %s:%d desconectado \n", 
            inet_ntoa(slaveAddress.sin_addr), 
            ntohs(slaveAddress.sin_port)
          );  

          
          close(slaveSocket);
          slavesArray[i] = 0;

        }
        
        // Recebeu uma mensagem do Slave
        else {
          
          // Verificamos  se o intervalo é maior que 100, caso for, finaliza
          if( gap + discrization >= 100 ){
            strcpy(Buffer, "finalizado");
            send(slaveSocket, Buffer, strlen(Buffer), 0);
            printf("[MASTER] -> (%s) -> Slave[%d] \n", Buffer, slaveSocket);
          }
          
          // Caso contrario, envia dados de calculo
          else{

            // Caso esteja pronto, envia o intervalo da integral
            if(strcmp(Buffer, "pronto") == 0) {
              printf("[MASTER] <- (%s) <- Slave[%d] \n", Buffer, slaveSocket);
              sprintf(Buffer, "%lf", gap);
              send(slaveSocket, Buffer, strlen(Buffer), 0);
              printf("[MASTER] -> (%f) -> Slave[%d] \n", gap, slaveSocket);
            }
          
            // Caso contrario, o Slave finalizou o calculo
            else {
              sscanf(Buffer, "%lf", &slaveReturn);
              printf("[MASTER] <- (%f) <- Slave[%d] \n", slaveReturn, slaveSocket);
              total = total + slaveReturn;
              sprintf(Buffer, "%lf", gap);
              send(slaveSocket, Buffer, strlen(Buffer), 0);
              printf("[MASTER] -> (%f) -> Slave[%d] \n", gap, slaveSocket);
            }

            gap = gap + discrization;
          }

        } // Else recebimento de msg

      } // If FD_ISSET

    } // For

  } // While

  // Finalização conexão do MASTER
  close(masterSocket);
  
  return EXIT_SUCCESS;
}

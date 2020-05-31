#include <stdio.h> 
#include <math.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 

#define masterPort 8080
#define bufferSize 64

// Especificações do socket de conexão 
struct sockaddr_in serverAddress;

// Variaveis da Integral
double discretization = 0.0;
double gap = 0.0;
double result = 0.0;

// Buffer de Leitura/Escrita
char inputBuffer[bufferSize];
char outputBuffer[bufferSize];

// Função para calculo da integral
double integral(double gap, double discretization){
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

// Inicializa o socket e a conexão com Master
int startConnection(){
  
  // Criação do sokcet
  int networkSocket;
  networkSocket = socket(AF_INET, SOCK_STREAM, 0);

  // Especificações do socket de conexão 
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(masterPort);  // Converte pra endereço de porta
  serverAddress.sin_addr.s_addr = INADDR_ANY;  // INADDR_NAY = 0.0.0.0
  memset(serverAddress.sin_zero, 0x0, 8);

  //Função de coneção com o socket
  // @Param 'int': Identificação do socket de comunicação
  // @Param 'struck pointer': ponteiro para o endereço(estrutura) do socket
  // @Param 'integer': tamanho do endereço 
  int connectionStatus = connect(networkSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

  // Verifica possiveis erros de conexão
  if(connectionStatus == -1){
    printf("Erro ao conectar com o socket do servidor MASTER \n");
  }

  return networkSocket;
}

// Faz o handShake com o Master para dizer que está "pronto"
void handShake(int networkSocket){
  // Armazena a mensagem recebida MASTER
  char serverResponse[256];

  //Recebendo dados do MASTER após conexão
  // @Param: 'int'   : Identificação do socket de comunicação
  // @Param: 'char*' : Buffer de armazenamento de resposta
  // @Param: 'int'   : Tamanho da msg de resposta
  // @Param: 'int'   : Opcional
  int status = recv(networkSocket, &serverResponse, sizeof(serverResponse), 0);
  if(status > 0) { sscanf(serverResponse, "%lf", &discretization); }

  // Envia a mensagem para o servidor como "resposta"
  printf("[SLAVE] <- Recebeu: %f\n", discretization);

  // Armazena a mensagem de envio para o MASTER
  char slaveMessage[256];

  strcpy(slaveMessage, "pronto");

  // Envia mensagem para o Master
  send(networkSocket, slaveMessage, strlen(slaveMessage), 0);
  printf("[SLAVE] -> Enviou: %s\n", slaveMessage);
}

// Função principal
int main(int argc, char const *argv[]) {   

  // Inicializa e armezena socket de conexão com Master
  int networkSocket = startConnection();

  // Realização do HandShake
  handShake(networkSocket);
  
  // Comunica-se com o Master até que a o Master encerre a conexão
  while(1) {

    // Limpa(Zera) os Buffers
    memset(inputBuffer, 0x0, bufferSize);
    memset(outputBuffer, 0x0, bufferSize);

    // Recebe uma resposta do master
    int response = recv(networkSocket, outputBuffer, bufferSize, 0);
    if( response == -1 ) { perror("Erro ao receber resposta do Master\n"); } 

    // Faz a leitura do intervalo e realiza a integral
    sscanf(outputBuffer, "%lf", &gap);
    printf("[SLAVE] <- (%f) <- Master \n", gap);

    result = integral(gap, discretization);
    printf("[SLAVE] <> Valor da integral de intervao [%f, %f] = %f\n",gap, gap+discretization, result);

    // Caso o Master tenha finalizado a integral encerrra conexão
    if(strcmp(outputBuffer, "finalizado") == 0) break;

    // Envia o resultado da integral para o Master
    sprintf(inputBuffer, "%lf", result);
    send(networkSocket, inputBuffer, strlen(inputBuffer), 0);
    printf("[SLAVE] -> (%f) -> Master \n", result);

  }
  

  // Finzaliza a conexão
  close(networkSocket);

  return 0;
}
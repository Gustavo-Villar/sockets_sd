# Sistemas distribuidos - Sockets em C.

<p>Escrever um programa para o cálculo da integral da função abaixo, usando a regra do Trapézio:<p>
<img src="https://www.dcce.ibilce.unesp.br/~aleardo/cursos/sd/function.png"/>

<p>Seu programa deve calcular a integral no intervalo [0,100], sendo que o mesmo deverá usar uma estrutura mestre-escravo (o inverso de cliente-servidor) usando sockets para a comunicação entre mestre e escravos. O programa mestre deve requisitar a cada escravo o cálculo de parte do intervalo, esperando que os mesmos retornem a parte deles, que serão acumuladas para o resultado final pelo mestre.<p>

<p>Seu programa deverá ser executado para as seguintes condições:<p>

1. Número de escravos variando entre 1, 2, 4 e 10 escravos.
2. Intervalo de discretivação sendo 0.0001, 0.00001 e 0.000001

## Como usar

<p> Existem duas versões, a com log de acontecimentos e uma versão com limpa, após a escolher a versão execute os seguintes comandos no terminal.<p>

 1. make Makefile compile - compila os arquivos do diretorio selecionado
 2. Escolher quantos slaves deseja utilizar e executar o comando respectivo:
    1. make Makefile slave2 - executa com 2 slaves
    2. make Makefile slave4 - executa com 4 slaves
    3. make Makefile slave10 - executa com 10 slaves

 	

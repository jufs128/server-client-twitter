# Simplificação do Twitter
  O propósito do código é criar algo similar ao Twitter, em que múltiplos clientes podem se conectar a um servidor, inscrever-se e desinscrever-se em certas tags, mandar mensagens e receber mensagens com as tags em que está inscrito, tendo as mensagens tamanho máximo de 500 bytes.
  
## Implementação
  Para que o servidor se conecte a e receba mensagens de múltiplos clientes ao mesmo tempo, foi utilizado um sistema multi-threads com a biblioteca pthreads em C. Já para o cliente ser capaz de enviar uma mensagem e receber mensagens simultâneamente, foi implementado um select, que checa todos os I/O para verificar se há atividade.
  
## Entrada
  Para o server, é necessário o port em que será conectado, e para o cliente, o endereço IPv4 e o port do servidor a que irá se conectar. Fora isso, o cliente deve mandar as mensagens, tanto normais como de inscrever ou desinscrever de tags.
  
## Compilação
  Na pasta code está incluso um Makefile, basta digitar make na linha de comando da pasta onde se encontra.

## Execução
  Em uma janela do prompt de comando deve-se, primeiramente, executar o server (ex.: ./servidor 51511) e, em uma janela diferente para cada cliente, executa-se o client (ex.: ./cliente 127.0.0.1 51511).

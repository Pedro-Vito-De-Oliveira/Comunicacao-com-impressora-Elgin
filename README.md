# Comunicacao-com-impressora-Elgin

Grupo formado por: Andrey Trolezi, Bruno Juvenal, Pedro Vito e William Pimentel

Projeto em C para comunicação com impressora de cupom fiscal.

Como usar:

1- Baixe e extraia os arquivos;
2- Conecte a impressora em seu computador;
3- Execute o arquivo "projeto.c";
4- Faça a configuração e selecione o modelo da sua impressora;
5- Escolha a opção desejada para impressão.

Explicação das funções: 

O funcionamento se deve ao uso da biblioteca para importar os arquivos e ter comunicação com as impressoras elgin e genéricas

Tem as seguintes funções:

exibirMenu - exibe os printf mostrando as opções disponiveis

configurarConexao - segue os passos para fazer a configuração da impressora

abrirConfiguraçoes - mostra as configurações que colocou na configurações e mostra conexão efetuada

fecharConexao - fecha a conexão e da um feedback se a conexão foi fechada mesmo ou deu erro

imprimirTexto - entra na opção de escrever um texto qualquer e imprimir ele

imprimirQRCode - deixa você colocar algum texto ou link em um QRcode

imprimirCodigoBarras - imprime um codigo generico

imprimirXMLSAT - imprime um arquivo XML

imprimirXMLCancelamentoSAT - imprime o XML de 
cancelamento

abrirGavetaElginOpc - abre a gaveta da impressora Elgin

abrirGavetaOpc - abre a gaveta de uma impressora genérica

emitirSinalSonoro - emite um bip

no int main ele só executa um switch case para selecionar as funções gerais citadas acima

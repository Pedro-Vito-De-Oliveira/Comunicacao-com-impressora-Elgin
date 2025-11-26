#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/* ======================= Config DLL ======================= */
// Handle da DLL carregada dinamicamente
static HMODULE g_hDll = NULL;

/* ConvenÃ§Ã£o de chamada (Windows): __stdcall */
// Isso garante que as funÃ§Ãµes da DLL usem o mesmo padrÃ£o de empilhamento do Windows
#ifndef CALLCONV
#  define CALLCONV WINAPI
#endif

/* ======================= Assinaturas da DLL ======================= */
// DefiniÃ§Ã£o dos tipos de funÃ§Ã£o que serÃ£o importados da DLL
// Cada typedef corresponde a uma funÃ§Ã£o real da DLL
typedef int (CALLCONV *AbreConexaoImpressora_t)(int, const char *, const char *, int);
typedef int (CALLCONV *FechaConexaoImpressora_t)(void);
typedef int (CALLCONV *ImpressaoTexto_t)(const char *, int, int, int);
typedef int (CALLCONV *Corte_t)(int);
typedef int (CALLCONV *ImpressaoQRCode_t)(const char *, int, int);
typedef int (CALLCONV *ImpressaoCodigoBarras_t)(int, const char *, int, int, int);
typedef int (CALLCONV *AvancaPapel_t)(int);
typedef int (CALLCONV *AbreGavetaElgin_t)(int, int, int);
typedef int (CALLCONV *AbreGaveta_t)(int, int, int);
typedef int (CALLCONV *SinalSonoro_t)(int, int, int);
typedef int (CALLCONV *ImprimeXMLSAT_t)(const char *, int);
typedef int (CALLCONV *ImprimeXMLCancelamentoSAT_t)(const char *, const char *, int);
typedef int (CALLCONV *InicializaImpressora_t)(void);

/* ======================= Ponteiros ======================= */
// Ponteiros para funÃ§Ãµes que serÃ£o carregadas dinamicamente
static AbreConexaoImpressora_t        AbreConexaoImpressora        = NULL;
static FechaConexaoImpressora_t       FechaConexaoImpressora       = NULL;
static ImpressaoTexto_t               ImpressaoTexto               = NULL;
static Corte_t                        Corte                        = NULL;
static ImpressaoQRCode_t              ImpressaoQRCode              = NULL;
static ImpressaoCodigoBarras_t        ImpressaoCodigoBarras        = NULL;
static AvancaPapel_t                  AvancaPapel                  = NULL;
static AbreGavetaElgin_t              AbreGavetaElgin              = NULL;
static AbreGaveta_t                   AbreGaveta                   = NULL;
static SinalSonoro_t                  SinalSonoro                  = NULL;
static ImprimeXMLSAT_t                ImprimeXMLSAT                = NULL;
static ImprimeXMLCancelamentoSAT_t    ImprimeXMLCancelamentoSAT    = NULL;
static InicializaImpressora_t         InicializaImpressora         = NULL;

/* ======================= ConfiguraÃ§Ã£o ======================= */
// VariÃ¡veis globais que armazenam as configuraÃ§Ãµes da impressora
static int   g_tipo      = 1;           // Tipo de conexÃ£o (1=USB, 2=Serial, 3=Rede)
static char  g_modelo[64] = "i9";       // Modelo padrÃ£o da impressora
static char  g_conexao[128] = "USB";    // ConexÃ£o padrÃ£o
static int   g_parametro = 0;           // ParÃ¢metro adicional
static int   g_conectada = 0;           // Flag para saber se a impressora estÃ¡ conectada

/* ======================= Utilidades ======================= */
// Macro para carregar funÃ§Ãµes da DLL de forma segura
#define LOAD_FN(h, name)                                                         \
    do {                                                                         \
        name = (name##_t)GetProcAddress((HMODULE)(h), #name);                    \
        if (!(name)) {                                                           \
            fprintf(stderr, "Falha ao resolver sÃ­mbolo %s (erro=%lu)\n",         \
                    #name, GetLastError());                                      \
            return 0;                                                            \
        }                                                                        \
    } while (0)

// FunÃ§Ã£o auxiliar para limpar o buffer do teclado (usada apÃ³s scanf)
static void flush_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* ======================= DLL ======================= */
// Carrega a DLL e obtÃ©m os endereÃ§os das funÃ§Ãµes
static int carregarFuncoes(void)
{
    g_hDll = LoadLibraryA("E1_Impressora01.dll"); // Tenta carregar a DLL
    if (!g_hDll) {
        fprintf(stderr, "Erro ao carregar E1_Impressora01.dll (erro=%lu)\n", GetLastError());
        return;
    }

    // Carrega os ponteiros das funÃ§Ãµes exportadas pela DLL
    LOAD_FN(g_hDll, AbreConexaoImpressora);
    LOAD_FN(g_hDll, FechaConexaoImpressora);
    LOAD_FN(g_hDll, ImpressaoTexto);
    LOAD_FN(g_hDll, Corte);
    LOAD_FN(g_hDll, ImpressaoQRCode);
    LOAD_FN(g_hDll, ImpressaoCodigoBarras);
    LOAD_FN(g_hDll, AvancaPapel);
    LOAD_FN(g_hDll, AbreGavetaElgin);
    LOAD_FN(g_hDll, AbreGaveta);
    LOAD_FN(g_hDll, SinalSonoro);
    LOAD_FN(g_hDll, ImprimeXMLSAT);
    LOAD_FN(g_hDll, ImprimeXMLCancelamentoSAT);
    LOAD_FN(g_hDll, InicializaImpressora);

    return 1; // DLL carregada com sucesso
}

// Libera a DLL da memoria quando nao for mais usada
static void liberarBiblioteca(void)
{
    if (g_hDll) {
        FreeLibrary(g_hDll);
        g_hDll = NULL;
    }
}

/* ======================= Funcoes do menu ======================= */


static void exibirMenu(void)
{
	//Menu inicial do terminal
	
    printf("\n===================== MENU IMPRESSORA ELGIN =====================\n");
    printf("1 - Configurar conexao\n");
    printf("2 - Abrir conexao\n");
    printf("3 - Fechar conexao\n");
    printf("4 - Imprimir texto\n");
    printf("5 - Imprimir QRCode\n");
    printf("6 - Imprimir codigo de barras\n");
    printf("7 - Imprimir XML SAT\n");
    printf("8 - Imprimir XML Cancelamento SAT\n");
    printf("9 - Abrir gaveta (Elgin)\n");
    printf("10 - Abrir gaveta (generico)\n");
    printf("11 - Emitir sinal sonoro\n");
    printf("0 - Sair\n");
    printf("================================================================\n");
    printf("Escolha uma opcao: ");
}


static void configurarConexao(void)
{
	// Configuracao da conexao com a impressora
    printf("Informe o tipo \n1-USB\n2-Serial\n3-Rede\n");
    scanf("%d", &g_tipo);
    // Valida o tipo de conexao
    if (g_tipo < 1 || g_tipo > 3) {
        printf("ERRO, TENTE NOVAMENTE");
        Sleep(2000);
        return;
    }
    flush_entrada();

    int modelo_opcao;
    printf("Informe o modelo \n1-i7\n2-i8\n3-i9\n");
    scanf("%d", &modelo_opcao);
    flush_entrada();
    // Seleciona o modelo baseado na opcao escolhida
    switch(modelo_opcao) {
        case 1: strcpy(g_modelo, "i7"); break;
        case 2: strcpy(g_modelo, "i8"); break;
        case 3: strcpy(g_modelo, "i9"); break;
        default:
            printf("ERRO, TENTE NOVAMENTE");
            Sleep(2000);
            return;
    }
	
	//Informar a conexao aqui
    printf("Informe a conexao (ex: USB, COM3, 192.168.0.100): ");
    fgets(g_conexao, sizeof(g_conexao), stdin);
    g_conexao[strcspn(g_conexao, "\n")] = 0; 

	// Parametro da impressora
    printf("Informe o parametro (geralmente 0): ");
    scanf("%d", &g_parametro);
    flush_entrada();

    printf("Configuracao salva!\n");
}


static void abrirConexao(void)
{
    int ret = AbreConexaoImpressora(g_tipo, g_modelo, g_conexao, g_parametro); 
    if (ret == 0) {
        printf("Conexao aberta com sucesso!\n"); // Abre a conexao
        g_conectada = 1;
    } else {
        printf("Erro ao abrir conexao (codigo=%d)\n", ret); 
    }
}

// Fecha a conexao
static void fecharConexao(void)
{
    if (g_conectada == 0) { // Puxa para ver se tem alguma conexao sendo feita
        printf("Nenhuma conexao aberta.\n");
        return;
    }
    int ret = FechaConexaoImpressora();
    if (ret == 0) {
        printf("Conexao fechada com sucesso.\n"); // Fecha a conexao
        g_conectada = 0;
    } else {
        printf("Erro ao fechar conexao (codigo=%d)\n", ret); // Mensagem de erro se nao conseguir fechar
    }
}

// Envia texto para a impressora
static void imprimirTexto(void)
{
	if (g_conectada == 0) { // Puxa para ver se tem alguma conexao sendo feita
        printf("Erro, impressora desligada\n");
        return;
    }
    char texto[256];
    printf("Digite o texto para imprimir: "); // Escrever qualquer coisa aqui que vai ser impresso
    fgets(texto, sizeof(texto), stdin); 
    texto[strcspn(texto, "\n")] = 0;

    // Impressao de texto simples
    ImpressaoTexto(texto, 0, 0, 0);
    AvancaPapel(10); // Avanca o papel
    Corte(0);        // Faz o corte total
}

// Imprime um QRCode com o conteudo digitado
static void imprimirQRCode(void)
{
	if (g_conectada == 0) { // Puxa para ver se tem alguma conexao sendo feita
        printf("Erro, impressora desligada\n");
        return;
    }
    char conteudo[256];
    printf("Digite o conteudo do QRCode: "); //Escrever qualquer coisa aqui que vai ser exibido no QRCode
    fgets(conteudo, sizeof(conteudo), stdin);
    conteudo[strcspn(conteudo, "\n")] = 0;

    ImpressaoQRCode(conteudo, 6, 4); // Tamanho e correcao do QR
    AvancaPapel(10);
    Corte(0);
}

// Imprime um codigo de barras fixo aleatorio
static void imprimirCodigoBarras(void)
{
	if (g_conectada == 0) { // Puxa para ver se tem alguma conexao sendo feita
        printf("Erro, impressora desligada\n");
        return;
    }
    ImpressaoCodigoBarras(8, "{A012345678912", 100, 2, 3);
    AvancaPapel(10);
    Corte(0);
}

// Imprime o arquivo XML SAT 
static void imprimirXMLSAT(void)
{
	if (g_conectada == 0) { // Puxa para ver se tem alguma conexao sendo feita
        printf("Erro, impressora desligada\n");
        return;
    }
    FILE *f = fopen("./XMLSAT.xml", "rb"); // Abre o arquivo XML em modo binario
    if (!f) {
        // Erro ao abrir o arquivo XML 
        printf("Erro ao abrir XMLSAT.xml\n");
        return;
    }

    // LÃª o arquivo XML
    fseek(f, 0, SEEK_END); // Vai para o final do arquivo
    long len = ftell(f); // Pega o tamanho do arquivo
    rewind(f); // Volta para o inÃ­cio do arquivo
    char *xml = (char *)malloc(len + 1); // Aloca memoria para o conteudo do XML
    fread(xml, 1, len, f); // Le o conteudo do arquivo
    xml[len] = 0; // Adiciona o terminador de string
    fclose(f); // Fecha o arquivo

    // Manda o XML para a impressora
    ImprimeXMLSAT(xml, 0);
    AvancaPapel(10);
    Corte(0);
    free(xml);
}

// Imprime o XML de cancelamento do SAT (com assinatura)
static void imprimirXMLCancelamentoSAT(void)
{
	if (g_conectada == 0) { // Puxa para ver se tem alguma conexao sendo feita
        printf("Erro, impressora desligada\n");
        return;
    }
    // Abre o arquivo XML de cancelamento
    FILE *f = fopen("./CANC_SAT.xml", "rb"); // Abre o arquivo em modo binario
    if (!f) {
        printf("Erro ao abrir CANC_SAT.xml\n"); // Erro ao abrir o arquivo
        return;
    }

    // Le o arquivo XML de cancelamento
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);
    char *xml = (char *)malloc(len + 1);
    fread(xml, 1, len, f);
    xml[len] = 0;
    fclose(f);

    // Assinatura digital obrigatoria
    const char *assinatura =
        "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZ"
        "jbh3btsZhF+sjHqEMR159i4agru9x6KsepK/q0E2e5xlU5cv3m1woYfgHyOkWDNc"
        "SdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQE"
        "Vd7u+TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6"
        "p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+ZfO5b/+vkyAHS6C"
        "YVFCDtYR9Hi5qgdk31v23w==";

    // Chamada da funcao da DLL que imprime o XML de cancelamento
    ImprimeXMLCancelamentoSAT(xml, assinatura, 0);
    AvancaPapel(10);
    Corte(0);
    free(xml); // Libera a memoria alocada
}

// Abre a gaveta da impressora Elgin
static void abrirGavetaElginOpc(void)
{
	if (g_conectada == 0) { // Puxa para ver se tem alguma conexÃ£o sendo feita
        printf("Erro, impressora desligada\n");
        return;
    }
    AbreGavetaElgin(1, 50, 50);
}

// Abre a gaveta de uma impressora qualquer
static void abrirGavetaOpc(void)
{
	if (g_conectada == 0) { // Puxa para ver se tem alguma conexao£o sendo feita
        printf("Erro, impressora desligada\n");
        return;
    }
    AbreGaveta(1, 5, 10); 
}

// Faz um bip 
static void emitirSinalSonoro(void) 
{
	if (g_conectada == 0) { // Puxa para ver se tem alguma conexao sendo feita
        printf("Erro, impressora desligada\n");
        return;
    }
    SinalSonoro(4, 50, 5);
}

/* ======================= Funcao principal ======================= */
int main(void)
{
    // Tenta carregar a DLL da impressora
    if (!carregarFuncoes()) {
        return 1;
    }

    int opcao = -1;
    // Loop principal do menu
    while (opcao != 0) {
        exibirMenu();        // Mostra o menu
        scanf("%d", &opcao); // Le a opcao do usuario
        flush_entrada();     

        // Chama a funcao que voce escolhe na funcao exibirMenu
        switch (opcao) {
            case 1: configurarConexao(); break;
            case 2: abrirConexao(); break;
            case 3: fecharConexao(); break;
            case 4: imprimirTexto(); break;
            case 5: imprimirQRCode(); break;
            case 6: imprimirCodigoBarras(); break;
            case 7: imprimirXMLSAT(); break;
            case 8: imprimirXMLCancelamentoSAT(); break;
            case 9: abrirGavetaElginOpc(); break;
            case 10: abrirGavetaOpc(); break;
            case 11: emitirSinalSonoro(); break;
            case 0: printf("Saindo...\n"); break;
            default: printf("Opcao invalida!\n"); break;
        }
    }

    // Libera a DLL antes de encerrar o programa
    liberarBiblioteca();
    return 0;
}

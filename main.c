#include <stdio.h>
#include <curl/curl.h>
#include <gtk/gtk.h>
#include <string.h>
#include <locale.h>
#define SITE "https://www.noticiasagricolas.com.br/cotacoes/soja"

/*
Este programa obtem a cotacao diaria da saca de soja de acordo com o site definido, junto com a data da cotacao e a variacao diaria

*/

int construir_ui();
void obter_dados();
size_t escrever_dados();

//definindo a estrutura
typedef struct estrutura_memoria
{
    char *memoria;
    size_t tamanho;
}estrutura_etiqueta;

typedef struct dados_tratados
{
    GtkLabel *label_data_atual;
    GtkLabel *label_cotacao_atual;
    GtkLabel *label_varicao_atual;
}dados_tratados_etiqueta;

int main(int argc, char **argv)
{
    printf("Setando a codificacao e linguagem\n", setlocale(LC_ALL, "pt_BR"));
    gtk_init(&argc, &argv);

    if(construir_ui()==TRUE)
    {
        printf("Erro! Erro nº %i, do tipo: %s", errno, strerror(errno));
        return TRUE;
    }

    gtk_main();
    printf("Fim do programa!\n");
    return 0;
}

int construir_ui()
{
    struct dados_tratados *dados_exibidos;
    GtkBuilder *construtor = NULL;

    GtkWidget *janela_soja = NULL;
    GtkWidget *box_soja = NULL;
    GtkWidget *label_nome = NULL;
    GtkWidget *label_cotacao = NULL;
    GtkWidget *botao_atualizar = NULL;
    GtkWidget *grid_soja = NULL;
    GtkWidget *label_data_cotacao = NULL;
    GtkWidget *label_variacao = NULL;

    printf("Inicializando a estrutura global de labels...\n");
    dados_exibidos = malloc(sizeof(char[1024]));

    construtor = gtk_builder_new_from_file("glade.glade");
    if(construtor==NULL)
    {
        printf("Erro! Erro nº %i, do tipo: %s", errno, strerror(errno));
        return TRUE;
    }

    janela_soja = GTK_WIDGET(gtk_builder_get_object(construtor, "janela_soja"));
    if(janela_soja==NULL)
    {
        printf("Erro! Erro nº %i, do tipo: %s", errno, strerror(errno));
        return TRUE;
    }
    g_signal_connect(janela_soja, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect_swapped(janela_soja, "destroy", G_CALLBACK(free), dados_exibidos);

    box_soja = GTK_WIDGET(gtk_builder_get_object(construtor, "box_soja"));
    if(box_soja==NULL)
    {
        printf("Erro! Erro nº %i, do tipo: %s", errno, strerror(errno));
        return TRUE;
    }

    label_nome = GTK_WIDGET(gtk_builder_get_object(construtor, "label_nome"));
    if(label_nome==NULL)
    {
        printf("Erro! Erro nº %i, do tipo: %s", errno, strerror(errno));
        return TRUE;
    }

    label_cotacao = GTK_WIDGET(gtk_builder_get_object(construtor, "label_cotacao"));
    if(label_cotacao==NULL)
    {
        printf("Erro! Erro nº %i, do tipo: %s", errno, strerror(errno));
        return TRUE;
    }

    botao_atualizar = GTK_WIDGET(gtk_builder_get_object(construtor, "botao_atualizar"));
    if(botao_atualizar==NULL)
    {
        printf("Erro! Erro nº %i, do tipo: %s", errno, strerror(errno));
        return TRUE;
    }
    g_signal_connect_swapped(botao_atualizar, "clicked", G_CALLBACK(obter_dados), dados_exibidos);

    grid_soja = GTK_WIDGET(gtk_builder_get_object(construtor, "grid_soja"));
    if(grid_soja==NULL)
    {
        printf("Erro! Erro nº %i, do tipo: %s", errno, strerror(errno));
        return TRUE;
    }

    label_data_cotacao = GTK_WIDGET(gtk_builder_get_object(construtor, "label_data_cotacao"));
    if(label_data_cotacao==NULL)
    {
        printf("Erro! Erro nº %i, do tipo: %s", errno, strerror(errno));
        return TRUE;
    }

    dados_exibidos->label_data_atual = GTK_LABEL(gtk_builder_get_object(construtor, "label_data_atual"));
    if(dados_exibidos->label_data_atual==NULL)
    {
        printf("Erro! Erro nº %i, do tipo: %s", errno, strerror(errno));
        return TRUE;
    }
    gtk_label_set_text(dados_exibidos->label_data_atual, "0");

    dados_exibidos->label_cotacao_atual = GTK_LABEL(gtk_builder_get_object(construtor, "label_cotacao_atual"));
    if(dados_exibidos->label_cotacao_atual==NULL)
    {
        printf("Erro! Erro nº %i, do tipo: %s", errno, strerror(errno));
        return TRUE;
    }
    gtk_label_set_text(dados_exibidos->label_cotacao_atual, "0");

    label_variacao = GTK_WIDGET(gtk_builder_get_object(construtor, "label_variacao"));
    if(label_variacao==NULL)
    {
        printf("Erro! Erro nº %i, do tipo: %s", errno, strerror(errno));
        return TRUE;
    }

    dados_exibidos->label_varicao_atual = GTK_LABEL(gtk_builder_get_object(construtor, "label_varicao_atual"));
    if(dados_exibidos->label_varicao_atual==NULL)
    {
        printf("Erro! Erro nº %i, do tipo: %s", errno, strerror(errno));
        return TRUE;
    }
    gtk_label_set_text(dados_exibidos->label_varicao_atual, "0");

    gtk_widget_show_all(janela_soja);

    g_object_unref(construtor);

    return FALSE;
}

void obter_dados(struct dados_tratados *dados_exibidos)
{
    char *erro = NULL;
    CURL *Handle;
    CURLcode resultado;
    char *posicao;
    char *buffer;
    char *data_atual = "00/00/0000";
    char *cotacao = "000,00";
    char *variacao = "0,00%";
    int cortador;

    buffer = malloc(sizeof(char[500000]));

    data_atual = malloc(sizeof(char[20]));

    cotacao = malloc(sizeof(char[20]));

    variacao = malloc(sizeof(char[20]));

    struct estrutura_memoria alocador;

    alocador.memoria = malloc(1); //aloca a memoria inicialmente para que seja realocada depois quando a funcao escrever_dados for chamada
    alocador.tamanho = 0; //somente inicializando os valores

    //inicializa o curl
    curl_global_init(CURL_GLOBAL_ALL);

    char *url = SITE;

    //inicializar o handle
    Handle = curl_easy_init();

    //conjunto de comandos que definem o modo de conexao do programa ao site
    curl_easy_setopt(Handle, CURLOPT_WRITEFUNCTION, escrever_dados);
    curl_easy_setopt(Handle, CURLOPT_WRITEDATA, (void*)&alocador);
    curl_easy_setopt(Handle, CURLOPT_URL, url);
    curl_easy_setopt(Handle, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(Handle, CURLOPT_ERRORBUFFER, erro);
    curl_easy_setopt(Handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    resultado = curl_easy_perform(Handle);

    if(resultado!=CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(resultado));
        printf("Encerrando conexao...\n");
        curl_easy_cleanup(Handle);
        return;
    }else
        {
            //agora o alocador aponta para a parte da memoria aonde foi alocada a pagina de internet

            printf("%lu bytes recuperados\n", (unsigned long)alocador.tamanho);

            printf("Analisando os bytes recuperados...\n");
            //Esta funcao procura no espaço alocado a parte sobre a cotacao na pagina
            posicao = strstr(alocador.memoria, "cot-fisicas");

            printf("Copiando os bytes importantes....\n");
            //Aqui copia o que foi encontrado para o buffer
            strcpy(buffer, posicao);

            //apos copiar a memoria para o buffer, recortar o buffer com as informacoes importantes
            //Neste caso, delimitadas no codigo xml que foi buscado, por td
            //Desta forma obteremos a data da cotacao, o valor, e a variacao diaria

            printf("\n\nRecuperando a data atual...\n\n");
            //Copiando a data atual
            buffer = strstr(buffer, "td>");
            strncpy(data_atual, buffer, 13);

            printf("\n\nRecuperando a cotacao atual...\n\n");
            //Copiando o valor da cotacao
            buffer = strstr(buffer, "/td>");
            printf("copiando o buffer para a cotacao...\n\n");
            strncpy(cotacao, buffer, 15);

            printf("\n\nRecuperando a variacao atual...\n\n");
            //copiando a variacao diaria
            buffer = strstr(buffer, "0");
            printf("copiando o buffer para a variacao...\n\n");
            strncpy(variacao, buffer, 5);

            printf("Os dados recuperados foram: \n Data Atual: %s \n Cotacao: %s \n Variacao: %s \n", data_atual, cotacao, variacao);

            strcpy(buffer, "\0");

            //agora os dados serao limpos dos pedacos extras de memoria para serem disponibilizados
            for(cortador = 0;cortador<12;cortador++)
            {
                buffer[cortador] = data_atual[cortador+3];
            }
            strcpy(data_atual, buffer);
            printf("A data_atual tratada e: %s\n", data_atual);

            strcpy(buffer, "\0");

            for(cortador = 0;cortador<14;cortador++)
            {
                buffer[cortador] = cotacao[cortador+9];
            }
            strcpy(cotacao, buffer);
            printf("A cotacao tratada e: %s\n", cotacao);

            strcpy(buffer, "\0");

            printf("Os dados tratados sao: \n Data Atual: %s \n Cotacao: %s \n Variacao: %s \n", data_atual, cotacao, variacao);

            printf("Repassando os dados tratados para a estrutura...\n");
            gtk_label_set_text(dados_exibidos->label_cotacao_atual, cotacao);
            gtk_label_set_text(dados_exibidos->label_data_atual, data_atual);
            gtk_label_set_text(dados_exibidos->label_varicao_atual, variacao);
        }

    printf("A conexao foi completada\n\n");

    //apos ser feita a conexao, deve se passar o buffer do site para ser analisado o transformando em char


    printf("Limpando a conexao...\n");
    //limpando o curl e finalizando
    curl_easy_cleanup(Handle);

    printf("Limpando a memoria...\n");
    free(alocador.memoria);

    printf("Limpando a variavel buffer...\n");
    //preparando o buffer para ser limpo, neste ponto do programa ele esta vazio, mas alocado, e caso seja desalocado vazio, dara erro.
    buffer = '\0';
    free(buffer);

    printf("Limpando a variavel cotacao...\n");
    free(cotacao);

    printf("Limpando a variavel variacao...\n");
    free(variacao);

    printf("Limpando a variavel data_atual...\n");
    free(data_atual);

    printf("Encerrando a conexao...\n");
    curl_global_cleanup();

    printf("Apertou o botao!\n");
    return;
}

//esta funcao repassa os dados obtidos pelo curl para a estrutura global
size_t escrever_dados(void *buffer, size_t size, size_t nmemb, void *userp)
{
    size_t tamanho_real = size * nmemb;

    struct estrutura_memoria *memoria = (struct estrutura_memoria *)userp;

    char *ponteiro = realloc(memoria->memoria, memoria->tamanho + tamanho_real +1);
    if(!ponteiro)
    {
        //nao conseguiu realocar!
        printf("Nao foi possivel alocar a memoria para o ponteiro!\n");
        return 0;
    }

    memoria->memoria = ponteiro;
    //Este comando copia para dentro do endereço da estrutura, alocada pelo tamanho da estrutura, o buffer de acordo com o tamanho real
    memcpy(&(memoria->memoria[memoria->tamanho]), buffer, tamanho_real);
    //este comando adiciono o tamanho real ao tamanho da estrutura
    memoria->tamanho += tamanho_real;
    //Este comando limpa a estrutura
    memoria->memoria[memoria->tamanho] = 0;

    return tamanho_real;
}

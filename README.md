# AplicativoSoja
Um aplicativo que busca informações na Internet sobre o valor diário da Saca de Soja

//-------------------------------------------------------------------------------------------------------------------------------

# Descrição

Este aplicativo foi feito em C e GTK3, e tem como função buscar os valores diarios da saca de soja nacional de acordo com o
site "https://www.noticiasagricolas.com.br/cotacoes/soja".

//-------------------------------------------------------------------------------------------------------------------------------

# Estrutura:

Foram utilizadas  as bibliotecas padrão da linguagem C e da API GTK, além da API GLADE para montar os arquivos .XML de estrutura,
e a biblioteca CURL para obter as informações na internet.

//-------------------------------------------------------------------------------------------------------------------------------

# Como compilar:

cc pkg-config --cflags main.c -o AplicativoSoja `pkg-config --libs libcurl`

//-------------------------------------------------------------------------------------------------------------------------------

# Considerações Finais:

O arquivo .Glade é essencial para a estrutura do programa!

Caso encontre algum bug, por favor entre em contato!

//-------------------------------------------------------------------------------------------------------------------------------


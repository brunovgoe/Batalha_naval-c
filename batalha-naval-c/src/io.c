#include "io.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

bool entrada_ler_linha(char *buf, int maxlen){
    if(!fgets(buf, maxlen, stdin)) return false;
    size_t n=strlen(buf);
    if(n && buf[n-1]=='\n') buf[n-1]='\0';
    return true;
}

bool entrada_converter_coordenada(const char *s, int *linha, int *coluna, int linhas, int colunas){
    if(!s || !isalpha((unsigned char)s[0])) return false;
    int c = toupper((unsigned char)s[0]) - 'A';
    if(c<0 || c>=colunas) return false;
    int l=0;
    int i=1;
    if(!isdigit((unsigned char)s[i])) return false;
    while(s[i] && isdigit((unsigned char)s[i])){
        l = l*10 + (s[i]-'0');
        ++i;
    }
    if(s[i] != '\0') return false;
    l -= 1;
    if(l<0 || l>=linhas) return false;
    *linha=l;
    *coluna=c;
    return true;
}

int entrada_menu_principal(void){
    int opcao=0;
    printf("\n=== BATALHA NAVAL ===\n");
    printf("1) Jogador vs Jogador (mesmo PC)\n");
    printf("2) Jogador vs CPU\n");
    printf("3) Configuracoes\n");
    printf("4) Estatisticas (ultimo jogo)\n");
    printf("5) Sair\n> ");
    if (scanf("%d", &opcao) != 1){
        while(getchar()!='\n');
        return 0;
    }
    while(getchar()!='\n');
    return opcao;
}

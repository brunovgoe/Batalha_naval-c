#include <stdio.h>
#include "game.h"
#include "io.h"
#include "rnd.h"

int main(void){
    aleatorio_inicializar();

    int linhas = 10, colunas = 10;
    int posicionamento_automatico = 1;
    const char *cfg = "config.ini";

    jogo_carregar_configuracao(cfg, &linhas, &colunas, &posicionamento_automatico);

    Jogo jogo;

    for(;;){
        int opcao = entrada_menu_principal();

        if(opcao == 1){
            jogo_inicializar(&jogo, linhas, colunas, posicionamento_automatico);
            jogo_executar_pvp(&jogo);
            jogo_liberar(&jogo);

        } else if(opcao == 2){
            jogo_inicializar(&jogo, linhas, colunas, posicionamento_automatico);
            jogo_executar(&jogo);
            jogo_liberar(&jogo);

        } else if(opcao == 3){
            printf("Tamanho do tabuleiro (6-26): ");
            int n;
            if (scanf("%d", &n) != 1){
                while(getchar()!='\n');
                continue;
            }
            while(getchar()!='\n');
            if(n < 6)  n = 6;
            if(n > 26) n = 26;
            linhas = colunas = n;

            printf("Posicionamento humano (M)anual ou (A)utomatico? ");
            char ch = (char)getchar();
            while(getchar()!='\n');
            posicionamento_automatico = (ch == 'M' || ch == 'm') ? 0 : 1;

            printf("OK. Tamanho=%d, modo=%s\n", linhas, posicionamento_automatico ? "Auto" : "Manual");
            printf("Deseja salvar estas configuracoes agora? (S/N): ");
            int s = getchar();
            while(getchar()!='\n');
            if(s == 'S' || s == 's'){
                if(jogo_salvar_configuracao(cfg, linhas, colunas, posicionamento_automatico))
                    printf("Configuracao salva em %s\n", cfg);
                else
                    printf("Falha ao salvar %s\n", cfg);
            }

        } else if(opcao == 4){
            printf("\n-- Estatisticas (ultimo jogo) --\n");
            printf("As estatisticas detalhadas aparecem ao final de cada partida.\n");

        } else if(opcao == 5){
            printf("Saindo do jogo... Ate logo!\n");
            break;

        } else {
            printf("Opcao invalida.\n");
        }
    }

    return 0;
}

#include "game.h"
#include "io.h"
#include "rnd.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "stats.h"

static void jogador_inicializar(Jogador *j, int linhas, int colunas){
    tabuleiro_inicializar(&j->tabuleiro, linhas, colunas);
    tabuleiro_inicializar(&j->tiros, linhas, colunas);
    tabuleiro_preencher(&j->tiros, CELULA_AGUA);
    frota_inicializar_padrao(&j->frota);
    j->apelido[0] = '\0';
    j->tiros_disparados = 0;
    j->acertos = 0;
    j->erros = 0;
    j->navios_afundados = 0;
}

static void jogador_liberar(Jogador *j){
    tabuleiro_liberar(&j->tabuleiro);
    tabuleiro_liberar(&j->tiros);
    frota_liberar(&j->frota);
}

static void imprimir_coordenada(int linha, int coluna) {
    printf("%c%d", 'A' + coluna, linha + 1);
}

static void cpu_esperar(double segundos) {
    clock_t fim = clock() + (clock_t)(segundos * CLOCKS_PER_SEC);
    while (clock() < fim) {}
}

bool jogo_inicializar(Jogo *j, int linhas, int colunas, int posicionamento_automatico_humano){
    if(linhas<6) linhas=6;
    if(linhas>26) linhas=26;
    if(colunas<6) colunas=6;
    if(colunas>26) colunas=26;
    j->linhas=linhas;
    j->colunas=colunas;
    j->jogo_finalizado=0;
    j->posicionamento_automatico_humano=posicionamento_automatico_humano;
    jogador_inicializar(&j->humano, linhas, colunas);
    jogador_inicializar(&j->cpu, linhas, colunas);
    strcpy(j->humano.apelido, "Voce");
    strcpy(j->cpu.apelido, "CPU");
    j->ia_tem_alvo=0;
    j->cabeca_fila_ia=0;
    j->cauda_fila_ia=0;
    j->ultimo_jogo_valido=0;
    return true;
}

void jogo_liberar(Jogo *j){
    jogador_liberar(&j->humano);
    jogador_liberar(&j->cpu);
}

bool jogo_posicionar_frota_auto(Jogador *jogador){
    for(int id=0; id<jogador->frota.quantidade; ++id){
        int tentativas=0;
        const int max_tentativas=10000;
        while(tentativas++<max_tentativas){
            Orientacao o = (aleatorio_intervalo(0,1)==0 ? ORIENTACAO_H : ORIENTACAO_V);
            int linha = aleatorio_intervalo(0, jogador->tabuleiro.linhas-1);
            int coluna = aleatorio_intervalo(0, jogador->tabuleiro.colunas-1);
            if(frota_posicionar_navio(&jogador->tabuleiro, &jogador->frota, id, linha, coluna, o)) break;
        }
    }
    return true;
}

static void ler_coordenada(const char *mensagem, int *linha, int *coluna, int linhas, int colunas){
    char buf[32];
    for(;;){
        printf("%s", mensagem);
        if(!entrada_ler_linha(buf, sizeof(buf))) continue;
        if(entrada_converter_coordenada(buf, linha, coluna, linhas, colunas)) return;
        printf("Entrada invalida. Use algo como E5.\n");
    }
}

bool jogo_posicionar_frota_manual(Jogador *jogador){
    for(int id=0; id<jogador->frota.quantidade; ++id){
        Navio *n = &jogador->frota.navios[id];
        printf("\nPosicione: %s (tam=%d). Orientacao H/V? ", n->nome, n->tamanho);
        char buf[8];
        entrada_ler_linha(buf, sizeof(buf));
        Orientacao o = (buf[0]=='V' || buf[0]=='v') ? ORIENTACAO_V : ORIENTACAO_H;
        int linha,coluna;
        for(;;){
            ler_coordenada("Coordenada inicial (ex.: B3): ", &linha,&coluna, jogador->tabuleiro.linhas, jogador->tabuleiro.colunas);
            if(frota_posicionar_navio(&jogador->tabuleiro, &jogador->frota, id, linha, coluna, o)) break;
            printf("Nao cabe aqui ou conflita. Tente outra posicao.\n");
        }
        tabuleiro_imprimir(&jogador->tabuleiro, true);
    }
    return true;
}

static void ia_fila_inserir(Jogo *j, int linha, int coluna){
    if(linha<0||coluna<0||linha>=j->linhas||coluna>=j->colunas) return;
    int proximo=(j->cauda_fila_ia+1)%64;
    if(proximo==j->cabeca_fila_ia) return;
    j->fila_ia[j->cauda_fila_ia][0]=linha;
    j->fila_ia[j->cauda_fila_ia][1]=coluna;
    j->cauda_fila_ia=proximo;
}

static int ia_fila_remover(Jogo *j, int *linha, int *coluna){
    if(j->cabeca_fila_ia==j->cauda_fila_ia) return 0;
    *linha=j->fila_ia[j->cabeca_fila_ia][0];
    *coluna=j->fila_ia[j->cabeca_fila_ia][1];
    j->cabeca_fila_ia=(j->cabeca_fila_ia+1)%64;
    return 1;
}

static void cpu_adicionar_alvos(Jogo *j, int linha, int coluna){
    ia_fila_inserir(j, linha-1, coluna);
    ia_fila_inserir(j, linha+1, coluna);
    ia_fila_inserir(j, linha, coluna-1);
    ia_fila_inserir(j, linha, coluna+1);
}

static void cpu_escolher_tiro(Jogo *j, int *linha, int *coluna, Jogador *cpu){
    while(ia_fila_remover(j, linha, coluna)){
        Celula *vista = tabuleiro_obter(&cpu->tiros, *linha, *coluna);
        if(vista->estado!=CELULA_ACERTO && vista->estado!=CELULA_ERRO) return;
    }
    for(;;){
        *linha = aleatorio_intervalo(0, j->linhas-1);
        *coluna = aleatorio_intervalo(0, j->colunas-1);
        Celula *vista = tabuleiro_obter(&cpu->tiros, *linha, *coluna);
        if(vista->estado!=CELULA_ACERTO && vista->estado!=CELULA_ERRO) return;
    }
}

static void imprimir_estatisticas(const Jogador *j){
    int total = j->tiros_disparados;
    double precisao = total ? (100.0 * j->acertos / (double)total) : 0.0;
    printf("Jogador: %s\n", j->apelido);
    printf("  Tiros: %d | Acertos: %d | Erros: %d | Precisao: %.1f%% | Navios afundados: %d\n",
           total, j->acertos, j->erros, precisao, j->navios_afundados);
}

void jogo_executar(Jogo *j){
    time_t inicio = time(NULL);
    printf("Tamanho do tabuleiro: %dx%d\n", j->linhas, j->colunas);
    if(j->posicionamento_automatico_humano){
        printf("Posicionando sua frota automaticamente...\n");
        jogo_posicionar_frota_auto(&j->humano);
    } else {
        printf("-- Posicionamento Manual --\n");
        tabuleiro_imprimir(&j->humano.tabuleiro, true);
        jogo_posicionar_frota_manual(&j->humano);
    }
    printf("CPU posicionando frota...\n");
    jogo_posicionar_frota_auto(&j->cpu);

    int atual = 0;
    while(!j->jogo_finalizado){
        Jogador *atacante = atual==0 ? &j->humano : &j->cpu;
        Jogador *defensor = atual==0 ? &j->cpu   : &j->humano;

        int linha,coluna;
        if(atual==0){
            printf("\n-- Seu turno --\n");
            tabuleiro_imprimir(&atacante->tiros, false);
            ler_coordenada("Digite coordenada do tiro (ex.: E5): ", &linha,&coluna, j->linhas, j->colunas);
        } else {
            cpu_escolher_tiro(j, &linha,&coluna, &j->cpu);
            printf("\n-- Turno da CPU --\n");
            cpu_esperar(0.8);
        }

        Celula *vista = tabuleiro_obter(&atacante->tiros, linha, coluna);
        if(vista->estado==CELULA_ACERTO || vista->estado==CELULA_ERRO){
            if(atual==0) printf("Ja tentou aqui. Jogada perdida.\n");
        } else {
            bool afundou=false;
            int acerto = frota_aplicar_tiro(&defensor->tabuleiro, &defensor->frota, linha, coluna, &afundou);
            atacante->tiros_disparados++;

            if (acerto >= 0) {
                vista->estado = CELULA_ACERTO;
                atacante->acertos++;

                if (atual == 0) {
                    printf("ACERTOU!%s\n", afundou ? " AFUNDOU!" : "");
                } else {
                    printf("CPU atirou em ");
                    imprimir_coordenada(linha, coluna);
                    if (afundou)
                        printf(": ACERTOU e AFUNDOU %s!\n", defensor->frota.navios[acerto].nome);
                    else
                        printf(": ACERTOU!\n");
                }

                if (afundou) atacante->navios_afundados++;
                if (atual == 1) cpu_adicionar_alvos(j, linha, coluna);

            } else {
                vista->estado = CELULA_ERRO;
                atacante->erros++;

                if (atual == 0) {
                    printf("AGUA.\n");
                } else {
                    printf("CPU atirou em ");
                    imprimir_coordenada(linha, coluna);
                    printf(": AGUA.\n");
                }
            }
        }

        if(frota_celulas_restantes(&defensor->frota)==0){
            printf("\n*** FIM DE JOGO ***\nVencedor: %s\n", atacante->apelido);
            printf("\n-- Estatisticas --\n");
            imprimir_estatisticas(&j->humano);
            imprimir_estatisticas(&j->cpu);
            printf("\n-- Tabuleiro HUMANO (real) --\n");
            tabuleiro_imprimir(&j->humano.tabuleiro, true);
            printf("\n-- Tabuleiro CPU (real) --\n");
            tabuleiro_imprimir(&j->cpu.tabuleiro, true);

            int duracao = (int) difftime(time(NULL), inicio);
            estatisticas_registrar_partida("stats.csv",
                   "contra_cpu",
                   atacante->apelido,
                   j->linhas, j->colunas,
                   j->humano.tiros_disparados, j->humano.acertos, j->humano.erros, j->humano.navios_afundados,
                   j->cpu.tiros_disparados,   j->cpu.acertos,   j->cpu.erros,   j->cpu.navios_afundados,
                   duracao);

            j->jogo_finalizado=1;
            j->ultimo_jogo_valido=1;
            break;
        }

        atual = 1-atual;
    }
}

void jogo_executar_pvp(Jogo *j){
    time_t inicio = time(NULL);
    strcpy(j->humano.apelido, "Jogador 1");
    strcpy(j->cpu.apelido,   "Jogador 2");

    printf("Tamanho do tabuleiro: %dx%d\n", j->linhas, j->colunas);

    if(j->posicionamento_automatico_humano){
        printf("Posicionando frota do Jogador 1 automaticamente...\n");
        jogo_posicionar_frota_auto(&j->humano);
        printf("Posicionando frota do Jogador 2 automaticamente...\n");
        jogo_posicionar_frota_auto(&j->cpu);
    } else {
        printf("-- Posicionamento Manual (Jogador 1) --\n");
        tabuleiro_imprimir(&j->humano.tabuleiro, true);
        jogo_posicionar_frota_manual(&j->humano);

        printf("\nTroque para o Jogador 2. Pressione ENTER para continuar...");
        while(getchar()!='\n');

        printf("\n-- Posicionamento Manual (Jogador 2) --\n");
        tabuleiro_imprimir(&j->cpu.tabuleiro, true);
        jogo_posicionar_frota_manual(&j->cpu);

        printf("\nIniciando partida...\n");
    }

    int atual = 0;
    while(!j->jogo_finalizado){
        Jogador *atacante = atual==0? &j->humano : &j->cpu;
        Jogador *defensor = atual==0? &j->cpu   : &j->humano;

        int linha,coluna;
        printf("\n-- Turno do %s --\n", atacante->apelido);
        tabuleiro_imprimir(&atacante->tiros, false);
        ler_coordenada("Digite coordenada do tiro (ex.: E5): ", &linha,&coluna, j->linhas, j->colunas);

        Celula *vista = tabuleiro_obter(&atacante->tiros, linha, coluna);
        if(vista->estado==CELULA_ACERTO || vista->estado==CELULA_ERRO){
            printf("Ja tentou aqui. Jogada perdida.\n");
        } else {
            bool afundou=false;
            int acerto = frota_aplicar_tiro(&defensor->tabuleiro, &defensor->frota, linha, coluna, &afundou);
            atacante->tiros_disparados++;

            if (acerto >= 0) {
                vista->estado = CELULA_ACERTO;
                atacante->acertos++;
                printf("%s atirou em %c%d: ACERTOU", atacante->apelido, 'A'+coluna, linha+1);
                if (afundou){
                    atacante->navios_afundados++;
                    printf(" e AFUNDOU %s!", defensor->frota.navios[acerto].nome);
                }
                printf("\n");
            } else {
                vista->estado = CELULA_ERRO;
                atacante->erros++;
                printf("%s atirou em %c%d: AGUA.\n", atacante->apelido, 'A'+coluna, linha+1);
            }
        }

        if(frota_celulas_restantes(&defensor->frota)==0){
            printf("\n*** FIM DE JOGO ***\nVencedor: %s\n", atacante->apelido);
            printf("\n-- Estatisticas --\n");
            imprimir_estatisticas(&j->humano);
            imprimir_estatisticas(&j->cpu);

            printf("\n-- Tabuleiro %s (real) --\n", j->humano.apelido);
            tabuleiro_imprimir(&j->humano.tabuleiro, true);
            printf("\n-- Tabuleiro %s (real) --\n", j->cpu.apelido);
            tabuleiro_imprimir(&j->cpu.tabuleiro, true);

            int duracao = (int) difftime(time(NULL), inicio);
            estatisticas_registrar_partida("stats.csv",
                   "pvp",
                   atacante->apelido,
                   j->linhas, j->colunas,
                   j->humano.tiros_disparados, j->humano.acertos, j->humano.erros, j->humano.navios_afundados,
                   j->cpu.tiros_disparados,   j->cpu.acertos,   j->cpu.erros,   j->cpu.navios_afundados,
                   duracao);
            j->jogo_finalizado=1;
            j->ultimo_jogo_valido=1;
            break;
        }

        printf("\nPasse o teclado para o proximo jogador. Pressione ENTER para continuar...");
        while(getchar()!='\n');

        atual = 1-atual;
    }
}

bool jogo_carregar_configuracao(const char *caminho, int *linhas, int *colunas, int *posicionamento_automatico){
    FILE *f = fopen(caminho, "r");
    if(!f) return false;
    int l=*linhas, c=*colunas, a=*posicionamento_automatico;
    char chave[32];
    int valor;
    while(fscanf(f, " %31[^=]=%d", chave, &valor)==2){
        if(strcmp(chave,"rows")==0) l=valor;
        else if(strcmp(chave,"cols")==0) c=valor;
        else if(strcmp(chave,"auto_place")==0) a=valor;
        int ch=fgetc(f);
        if(ch=='\n') ;
        else ungetc(ch,f);
    }
    fclose(f);
    *linhas=l; *colunas=c; *posicionamento_automatico=a;
    return true;
}

bool jogo_salvar_configuracao(const char *caminho, int linhas, int colunas, int posicionamento_automatico){
    FILE *f=fopen(caminho,"w");
    if(!f) return false;
    fprintf(f, "rows=%d\ncols=%d\nauto_place=%d\n", linhas, colunas, posicionamento_automatico);
    fclose(f);
    return true;
}

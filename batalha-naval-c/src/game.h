#ifndef GAME_H
#define GAME_H
#include <stdbool.h>
#include "board.h"
#include "fleet.h"

typedef struct {
    Tabuleiro tabuleiro;
    Tabuleiro tiros;
    Frota frota;
    char apelido[32];
    int tiros_disparados;
    int acertos;
    int erros;
    int navios_afundados;
} Jogador;

typedef struct {
    Jogador humano, cpu;
    int linhas, colunas;
    int posicionamento_automatico_humano;
    int jogo_finalizado;
    int ia_tem_alvo;
    int ia_alvo_linha, ia_alvo_coluna;
    int fila_ia[64][2];
    int cabeca_fila_ia, cauda_fila_ia;
    int ultimo_jogo_valido;
} Jogo;

bool jogo_inicializar(Jogo *j, int linhas, int colunas, int posicionamento_automatico_humano);
void jogo_liberar(Jogo *j);

bool jogo_posicionar_frota_auto(Jogador *jogador);
bool jogo_posicionar_frota_manual(Jogador *jogador);

void jogo_executar(Jogo *j);
void jogo_executar_pvp(Jogo *j);

bool jogo_carregar_configuracao(const char *caminho, int *linhas, int *colunas, int *posicionamento_automatico);
bool jogo_salvar_configuracao(const char *caminho, int linhas, int colunas, int posicionamento_automatico);

#endif

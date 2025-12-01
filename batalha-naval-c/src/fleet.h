#ifndef FLEET_H
#define FLEET_H
#include <stdbool.h>
#include "board.h"

typedef enum { ORIENTACAO_H, ORIENTACAO_V } Orientacao;

typedef struct { char nome[20]; int tamanho; int acertos; int posicionado; } Navio;
typedef struct { Navio *navios; int quantidade; } Frota;

bool frota_inicializar_padrao(Frota *f);
void frota_liberar(Frota *f);
int  frota_celulas_restantes(const Frota *f);
bool frota_posicionar_navio(Tabuleiro *t, Frota *f, int id, int linha, int coluna, Orientacao o);
int  frota_aplicar_tiro(Tabuleiro *t, Frota *f, int linha, int coluna, bool *afundou);

#endif

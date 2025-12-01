#ifndef BOARD_H
#define BOARD_H
#include <stdbool.h>

typedef enum { CELULA_AGUA, CELULA_NAVIO, CELULA_ACERTO, CELULA_ERRO } EstadoCelula;

typedef struct {
    EstadoCelula estado;
    int id_navio;
} Celula;

typedef struct {
    int linhas, colunas;
    Celula *celulas;
} Tabuleiro;

bool tabuleiro_inicializar(Tabuleiro *t, int linhas, int colunas);
void tabuleiro_liberar(Tabuleiro *t);
void tabuleiro_preencher(Tabuleiro *t, EstadoCelula estado);

static inline bool tabuleiro_dentro_limites(const Tabuleiro *t, int linha, int coluna){
    return linha>=0 && linha<t->linhas && coluna>=0 && coluna<t->colunas;
}

static inline Celula* tabuleiro_obter(Tabuleiro *t, int linha, int coluna){
    return &t->celulas[linha*t->colunas + coluna];
}

static inline const Celula* tabuleiro_obter_const(const Tabuleiro *t, int linha, int coluna){
    return &t->celulas[linha*t->colunas + coluna];
}

void tabuleiro_imprimir(const Tabuleiro *t, bool revelar_navios);

#endif

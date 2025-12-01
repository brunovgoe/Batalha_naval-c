#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DISABLE_COLOR
#define C_RESET "\x1b[0m"
#define C_BLUE  "\x1b[34m"
#define C_RED   "\x1b[31m"
#define C_YEL   "\x1b[33m"
#define C_CYAN  "\x1b[36m"
#else
#define C_RESET ""
#define C_BLUE  ""
#define C_RED   ""
#define C_YEL   ""
#define C_CYAN  ""
#endif

bool tabuleiro_inicializar(Tabuleiro *t, int linhas, int colunas){
    t->linhas = linhas;
    t->colunas = colunas;
    t->celulas = malloc(linhas * colunas * sizeof(Celula));
    if(!t->celulas){
        fprintf(stderr,"Erro: memoria insuficiente para tabuleiro.\n");
        return false;
    }
    tabuleiro_preencher(t, CELULA_AGUA);
    return true;
}

void tabuleiro_liberar(Tabuleiro *t){
    free(t->celulas);
    t->celulas = NULL;
    t->linhas = 0;
    t->colunas = 0;
}

void tabuleiro_preencher(Tabuleiro *t, EstadoCelula estado){
    for(int i = 0; i < t->linhas * t->colunas; i++){
        t->celulas[i].estado = estado;
        t->celulas[i].id_navio = -1;
    }
}

void tabuleiro_imprimir(const Tabuleiro *t, bool revelar_navios){
    printf("\n     ");
    for(int c = 0; c < t->colunas; ++c)
        printf("%c ", 'A' + c);
    printf("\n");

    for(int r = 0; r < t->linhas; ++r){
        printf("%3d  ", r + 1);

        for(int c = 0; c < t->colunas; ++c){
            const Celula *celula = &t->celulas[r * t->colunas + c];
            char ch = '~';
            const char *cor = C_BLUE;

            if(revelar_navios && celula->id_navio != -1 && celula->estado == CELULA_NAVIO){
                ch  = '#';
                cor = C_CYAN;
            }

            if(celula->estado == CELULA_ERRO){
                ch  = 'o';
                cor = C_YEL;
            } else if(celula->estado == CELULA_ACERTO){
                ch  = 'X';
                cor = C_RED;
            }

            printf("%s%c%s ", cor, ch, C_RESET);
        }
        printf("\n");
    }
    printf("\n");
}

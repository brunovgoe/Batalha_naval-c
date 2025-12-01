#include "fleet.h"
#include <stdlib.h>
#include <string.h>

static bool pode_posicionar(Tabuleiro *t, int linha, int coluna, int tamanho, Orientacao o){
    if(o == ORIENTACAO_H){
        if(coluna + tamanho > t->colunas) return false;
    } else {
        if(linha + tamanho > t->linhas) return false;
    }
    for(int i = 0; i < tamanho; ++i){
        int l = linha + (o == ORIENTACAO_V ? i : 0);
        int c = coluna + (o == ORIENTACAO_H ? i : 0);
        if(!tabuleiro_dentro_limites(t, l, c)) return false;
        if(tabuleiro_obter_const(t, l, c)->estado != CELULA_AGUA) return false;
    }
    return true;
}

bool frota_inicializar_padrao(Frota *f){
    f->quantidade = 6;
    f->navios = malloc(sizeof(Navio)*f->quantidade);
    if(!f->navios) return false;
    strcpy(f->navios[0].nome, "Porta-avioes"); f->navios[0].tamanho = 5;
    strcpy(f->navios[1].nome, "Encouracado");  f->navios[1].tamanho = 4;
    strcpy(f->navios[2].nome, "Cruzador A");   f->navios[2].tamanho = 3;
    strcpy(f->navios[3].nome, "Cruzador B");   f->navios[3].tamanho = 3;
    strcpy(f->navios[4].nome, "Destroyer A");  f->navios[4].tamanho = 2;
    strcpy(f->navios[5].nome, "Destroyer B");  f->navios[5].tamanho = 2;
    for(int i = 0; i < f->quantidade; ++i){
        f->navios[i].acertos = 0;
        f->navios[i].posicionado = 0;
    }
    return true;
}

void frota_liberar(Frota *f){
    free(f->navios);
    f->navios = NULL;
    f->quantidade = 0;
}

int frota_celulas_restantes(const Frota *f){
    int restante = 0;
    for(int i = 0; i < f->quantidade; ++i){
        restante += f->navios[i].tamanho - f->navios[i].acertos;
    }
    return restante;
}

bool frota_posicionar_navio(Tabuleiro *t, Frota *f, int id, int linha, int coluna, Orientacao o){
    if(id < 0 || id >= f->quantidade || f->navios[id].posicionado) return false;
    int tamanho = f->navios[id].tamanho;
    if(!pode_posicionar(t, linha, coluna, tamanho, o)) return false;
    for(int i = 0; i < tamanho; ++i){
        int l = linha + (o == ORIENTACAO_V ? i : 0);
        int c = coluna + (o == ORIENTACAO_H ? i : 0);
        Celula *celula = tabuleiro_obter(t, l, c);
        celula->estado = CELULA_NAVIO;
        celula->id_navio = id;
    }
    f->navios[id].posicionado = 1;
    return true;
}

int frota_aplicar_tiro(Tabuleiro *t, Frota *f, int linha, int coluna, bool *afundou){
    *afundou = false;
    Celula *celula = tabuleiro_obter(t, linha, coluna);
    if(celula->estado == CELULA_ACERTO || celula->estado == CELULA_ERRO) return -1;
    if(celula->estado == CELULA_NAVIO){
        celula->estado = CELULA_ACERTO;
        int id = celula->id_navio;
        f->navios[id].acertos++;
        if(f->navios[id].acertos == f->navios[id].tamanho) *afundou = true;
        return id;
    } else {
        celula->estado = CELULA_ERRO;
        return -1;
    }
}

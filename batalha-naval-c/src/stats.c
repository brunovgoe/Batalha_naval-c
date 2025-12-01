#include "stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void talvez_escrever_cabecalho(FILE *f){
    long pos = ftell(f);
    fseek(f, 0, SEEK_END);
    long fim = ftell(f);
    fseek(f, pos, SEEK_SET);
    if(fim == 0){
        fprintf(f,
            "timestamp_iso,modo,vencedor,linhas,colunas,"
            "j1_tiros,j1_acertos,j1_erros,j1_afundados,"
            "j2_tiros,j2_acertos,j2_erros,j2_afundados,duracao_segundos\n");
    }
}

bool estatisticas_registrar_partida(const char *caminho,
                    const char *modo,
                    const char *vencedor,
                    int linhas, int colunas,
                    int j1_tiros, int j1_acertos, int j1_erros, int j1_afundados,
                    int j2_tiros, int j2_acertos, int j2_erros, int j2_afundados,
                    int duracao_segundos
){

    FILE *f = fopen(caminho, "ab");
    if(!f) return false;

    talvez_escrever_cabecalho(f);

    char ts[32];
    time_t agora = time(NULL);
    struct tm *lt = localtime(&agora);
    if(lt){
        strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", lt);
    } else {
        strcpy(ts, "1970-01-01 00:00:00");
    }

    fprintf(f,
        "%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
        ts, modo, vencedor, linhas, colunas,
        j1_tiros, j1_acertos, j1_erros, j1_afundados,
        j2_tiros, j2_acertos, j2_erros, j2_afundados,
        duracao_segundos
    );

    fclose(f);
    return true;
}

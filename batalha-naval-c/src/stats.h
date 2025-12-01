#ifndef STATS_H
#define STATS_H

#include <stdbool.h>

bool estatisticas_registrar_partida(const char *caminho,
                    const char *modo,
                    const char *vencedor,
                    int linhas, int colunas,
                    int j1_tiros, int j1_acertos, int j1_erros, int j1_afundados,
                    int j2_tiros, int j2_acertos, int j2_erros, int j2_afundados,
                    int duracao_segundos
);

#endif

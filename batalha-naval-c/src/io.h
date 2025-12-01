#ifndef IO_H
#define IO_H
#include <stdbool.h>

bool entrada_ler_linha(char *buf, int maxlen);
bool entrada_converter_coordenada(const char *s, int *linha, int *coluna, int linhas, int colunas);
int  entrada_menu_principal(void);

#endif

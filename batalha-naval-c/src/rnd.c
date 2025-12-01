#include "rnd.h"
#include <time.h>

void aleatorio_inicializar(void){
    srand((unsigned)time(NULL));
}

int aleatorio_intervalo(int minimo, int maximo){
    return minimo + rand() % (maximo - minimo + 1);
}

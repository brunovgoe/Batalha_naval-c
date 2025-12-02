Um jogo de Batalha Naval desenvolvido em C, com posicionamento manual ou automático, modo Jogador vs CPU, modo PvP no mesmo computador, IA simples e registro automático de estatísticas em CSV.


Como Compilar

Use este comando no terminal:

Windows (cmd ou PowerShell) gcc -std=c99 -Wall -Wextra -O2 -o batalha_naval.exe src\board.c src\fleet.c src\game.c src\io.c src\rnd.c src\main.c src\stats.c

Para executar(WINDOWNS): .\batalha_naval.exe

Ao iniciar o jogo, o menu principal aparece:

Jogador vs Jogador
Jogador vs CPU
Configurações
Estatísticas (último jogo)
Sair
Modos disponíveis Jogador vs CPU

Você posiciona sua frota manualmente ou automaticamente.

A CPU usa uma IA simples: após acertar um navio, tenta atingir as casas ao redor.

Jogador vs Jogador (PvP)

Dois jogadores revezam no mesmo computador.

Cada turno pede para passar o teclado ao outro jogador.

Estatísticas Automáticas

Ao final de cada partida, o jogo salva automaticamente:

Data e hora

Tipo de partida (CPU ou PvP)

Vencedor

Tamanho do tabuleiro

Tiros, acertos, erros e navios afundados de cada jogador

Duração da partida

Tudo é registrado no arquivo:

stats.csv

O arquivo é criado automaticamente se não existir.

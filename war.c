/*
gcc war_missoes.c -o war_missoes
./war_missoes
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ----- Definições ----- */
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

typedef struct {
    char nome[30];
    char cor[10];
    char* missao;   // alocada dinamicamente com malloc
} Jogador;

/* ----- Protótipos ----- */
Territorio* criarMapa(int n);
void cadastrarTerritorios(Territorio* mapa, int n);
void exibirMapa(Territorio* mapa, int n);
void atacar(Territorio* atacante, Territorio* defensor);
void liberarMemoriaMapa(Territorio* mapa);
void liberarMemoriaJogadores(Jogador* jogadores, int nJogadores);

void atribuirMissao(char* destino, char* missoes[], int totalMissoes, int tipoEscolhidoIndicador[]);
void exibirMissao(char* missao);
int verificarMissao(char* missao, Territorio* mapa, int tamanho, Jogador* jogador);

void lerLinhaLimpa(char* buffer, int tamanho);

/* ----- Implementações ----- */

/* Ler linha com fgets e remover newline */
void lerLinhaLimpa(char* buffer, int tamanho) {
    if (fgets(buffer, tamanho, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
    }
}

/* Aloca dinamicamente vetor de territórios */
Territorio* criarMapa(int n) {
    Territorio* mapa = (Territorio*) calloc(n, sizeof(Territorio));
    if (mapa == NULL) {
        printf("Erro: falha ao alocar memória para o mapa.\n");
        exit(EXIT_FAILURE);
    }
    return mapa;
}

/* Cadastro interativo dos territórios */
void cadastrarTerritorios(Territorio* mapa, int n) {
    printf("\n=== Cadastro de Territórios ===\n");
    for (int i = 0; i < n; i++) {
        printf("\nTerritório %d\n", i);
        printf("Nome: ");
        lerLinhaLimpa(mapa[i].nome, sizeof(mapa[i].nome));

        printf("Cor/do dono do exército: ");
        lerLinhaLimpa(mapa[i].cor, sizeof(mapa[i].cor));

        int tropas;
        do {
            printf("Número de tropas (>=0): ");
            if (scanf("%d", &tropas) != 1) {
                while (getchar() != '\n');
                tropas = -1;
                printf("Entrada inválida. Digite um inteiro.\n");
            } else if (tropas < 0) {
                printf("Tropas não podem ser negativas.\n");
            }
        } while (tropas < 0);

        mapa[i].tropas = tropas;
        while (getchar() != '\n'); // limpar buffer
    }
}

/* Exibe mapa com índices */
void exibirMapa(Territorio* mapa, int n) {
    printf("\n=== Mapa de Territórios ===\n");
    for (int i = 0; i < n; i++) {
        printf("[%d] %s | Dono: %s | Tropas: %d\n",
               i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
    printf("===========================\n");
}

/* Função de ataque entre dois territórios (usando ponteiros) */
void atacar(Territorio* atacante, Territorio* defensor) {
    if (atacante == NULL || defensor == NULL) return;

    if (atacante->tropas <= 0) {
        printf("Ataque inválido: atacante não tem tropas.\n");
        return;
    }

    printf("\n--- Batalha ---\n");
    printf("Atacante: %s (Dono: %s, Tropas: %d)\n",
           atacante->nome, atacante->cor, atacante->tropas);
    printf("Defensor: %s (Dono: %s, Tropas: %d)\n",
           defensor->nome, defensor->cor, defensor->tropas);

    int dadoA = (rand() % 6) + 1;
    int dadoD = (rand() % 6) + 1;
    printf("Rolagens: Atacante=%d | Defensor=%d\n", dadoA, dadoD);

    if (dadoA > dadoD) {
        int transferencia = atacante->tropas / 2;
        if (transferencia <= 0) transferencia = 1;
        atacante->tropas -= transferencia;
        defensor->tropas = transferencia;
        // copiar cor do atacante para defensor
        strncpy(defensor->cor, atacante->cor, sizeof(defensor->cor) - 1);
        defensor->cor[sizeof(defensor->cor) - 1] = '\0';
        printf("Atacante venceu! Território '%s' agora é de '%s' com %d tropas.\n",
               defensor->nome, defensor->cor, defensor->tropas);
        printf("Atacante agora tem %d tropas.\n", atacante->tropas);
    } else {
        atacante->tropas -= 1;
        if (atacante->tropas < 0) atacante->tropas = 0;
        printf("Atacante perdeu ou empatou. Atacante perde 1 tropa (agora %d).\n", atacante->tropas);
    }
}

/* Libera mapa */
void liberarMemoriaMapa(Territorio* mapa) {
    if (mapa != NULL) free(mapa);
}

/* Libera missões dos jogadores */
void liberarMemoriaJogadores(Jogador* jogadores, int nJogadores) {
    for (int i = 0; i < nJogadores; i++) {
        if (jogadores[i].missao != NULL) free(jogadores[i].missao);
    }
}

/*
  Atribuir missão:
  - destino: buffer onde será copiada a missão (deve ser alocado/ser espaço suficiente)
  - missoes[]: vetor de strings com prefixos TXX:descrição
  - totalMissoes: total no vetor
  - tipoEscolhidoIndicador: array de um inteiro (opcional) para armazenar o índice sorteado (passagem por referência)
*/
void atribuirMissao(char* destino, char* missoes[], int totalMissoes, int tipoEscolhidoIndicador[]) {
    if (totalMissoes <= 0) return;
    int idx = rand() % totalMissoes;
    // copia a missão completa (incluindo o prefixo) para destino
    strcpy(destino, missoes[idx]);
    if (tipoEscolhidoIndicador != NULL) {
        tipoEscolhidoIndicador[0] = idx; // retorna índice sorteado
    }
}

/* Exibe apenas a descrição legível da missão (sem o prefixo TXX:) */
void exibirMissao(char* missao) {
    if (missao == NULL) return;
    // procurar primeiro ':' e exibir o que vem após ele
    char* p = strchr(missao, ':');
    if (p != NULL) {
        printf("%s\n", p + 1); // descrição após o ':'
    } else {
        printf("%s\n", missao); // fallback
    }
}

/*
  verificarMissao:
  - função simples inicial que identifica o tipo pela tag TXX no início da string.
  - Implementa regras básicas para alguns tipos:
      T01: "Conquistar N territórios"  (procura 'N=' na string)
      T02: "Eliminar todas as tropas de cor X"  (procura cor após 'COR=')
      T03: "Controlar território nomeY" (procura 'ALVO=')
      T04: "Ter ao menos N tropas no total" (procura 'TOT=')
      T05: "Controlar 3 territórios seguidos" (verifica 3 territórios consecutivos do mesmo dono)
  - Retorna 1 se cumprida, 0 caso contrário.
*/
int verificarMissao(char* missao, Territorio* mapa, int tamanho, Jogador* jogador) {
    if (missao == NULL || mapa == NULL || jogador == NULL) return 0;

    // extrair código do tipo "TXX"
    if (strlen(missao) < 3) return 0;
    char tipoCod[4] = {0};
    strncpy(tipoCod, missao, 3); // "T01", etc.

    if (strcmp(tipoCod, "T01") == 0) {
        // Ex: "T01:N=3:Conquistar 3 territórios quaisquer"
        char* ptrN = strstr(missao, "N=");
        if (ptrN) {
            int N = atoi(ptrN + 2);
            int cont = 0;
            for (int i = 0; i < tamanho; i++) {
                if (strcmp(mapa[i].cor, jogador->cor) == 0) cont++;
            }
            return (cont >= N) ? 1 : 0;
        }
    } else if (strcmp(tipoCod, "T02") == 0) {
        // Ex: "T02:COR=vermelha:Eliminar todas as tropas da cor vermelha"
        char* ptrC = strstr(missao, "COR=");
        if (ptrC) {
            char corAlvo[20];
            int j = 0;
            char* p = ptrC + 4;
            while (*p != ':' && *p != '\0' && j < (int)sizeof(corAlvo)-1) {
                corAlvo[j++] = *p++;
            }
            corAlvo[j] = '\0';
            // verificar se existe alguma tropa dessa cor
            int soma = 0;
            for (int i = 0; i < tamanho; i++) {
                if (strcmp(mapa[i].cor, corAlvo) == 0) soma += mapa[i].tropas;
            }
            return (soma == 0) ? 1 : 0;
        }
    } else if (strcmp(tipoCod, "T03") == 0) {
        // Ex: "T03:ALVO=Fortaleza:Controlar o território Fortaleza"
        char* ptrA = strstr(missao, "ALVO=");
        if (ptrA) {
            char alvo[30];
            int j = 0;
            char* p = ptrA + 5;
            while (*p != ':' && *p != '\0' && j < (int)sizeof(alvo)-1) {
                alvo[j++] = *p++;
            }
            alvo[j] = '\0';
            for (int i = 0; i < tamanho; i++) {
                if (strcmp(mapa[i].nome, alvo) == 0) {
                    // verifica dono
                    if (strcmp(mapa[i].cor, jogador->cor) == 0) return 1;
                    else return 0;
                }
            }
            return 0;
        }
    } else if (strcmp(tipoCod, "T04") == 0) {
        // Ex: "T04:TOT=15:Ter ao menos 15 tropas no total"
        char* ptrT = strstr(missao, "TOT=");
        if (ptrT) {
            int TOT = atoi(ptrT + 4);
            int soma = 0;
            for (int i = 0; i < tamanho; i++) {
                if (strcmp(mapa[i].cor, jogador->cor) == 0) soma += mapa[i].tropas;
            }
            return (soma >= TOT) ? 1 : 0;
        }
    } else if (strcmp(tipoCod, "T05") == 0) {
        // Ex: "T05:SEQ=3:Controlar 3 territórios seguidos"
        char* ptrS = strstr(missao, "SEQ=");
        int SEQ = 3;
        if (ptrS) SEQ = atoi(ptrS + 4);
        // procura sequência de SEQ territórios consecutivos do jogador
        int maxSeq = 0, curSeq = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, jogador->cor) == 0) {
                curSeq++;
                if (curSeq > maxSeq) maxSeq = curSeq;
            } else {
                curSeq = 0;
            }
        }
        return (maxSeq >= SEQ) ? 1 : 0;
    }

    // caso nenhum tipo identificado ou sem parâmetros válidos, retorna 0
    return 0;
}

/* ----- MAIN: fluxo do jogo com 2 jogadores ----- */
int main() {
    srand((unsigned int) time(NULL));

    printf("=== War Estruturado: Missões Estratégicas ===\n");

    int nTerritorios;
    do {
        printf("Quantos territórios deseja cadastrar? (mínimo 3): ");
        if (scanf("%d", &nTerritorios) != 1) {
            while (getchar() != '\n');
            nTerritorios = 0;
            printf("Entrada inválida.\n");
        } else if (nTerritorios < 3) {
            printf("Escolha ao menos 3 territórios.\n");
        }
    } while (nTerritorios < 3);
    while (getchar() != '\n'); // limpar buffer

    Territorio* mapa = criarMapa(nTerritorios);
    cadastrarTerritorios(mapa, nTerritorios);

    // Vetor de missões (cada string contém prefixo tipo e depois descrição:
    // formato: "Txx:PARAMS:descrição")
    // Exemplos:
    // T01:N=3:Conquistar 3 territórios quaisquer
    // T02:COR=vermelha:Eliminar todas as tropas da cor vermelha
    // T03:ALVO=Fortaleza:Controlar o território Fortaleza
    // T04:TOT=15:Ter ao menos 15 tropas no total
    // T05:SEQ=3:Controlar 3 territórios seguidos
    char* missoes[] = {
        "T01:N=3:Conquistar 3 territórios quaisquer",
        "T02:COR=vermelha:Eliminar todas as tropas da cor vermelha",
        "T03:ALVO=Fortaleza:Controlar o território Fortaleza",
        "T04:TOT=15:Ter ao menos 15 tropas no total",
        "T05:SEQ=3:Controlar 3 territórios seguidos"
    };
    int totalMissoes = sizeof(missoes) / sizeof(missoes[0]);

    // Criar 2 jogadores (exemplo): jogador 0 e jogador 1
    int nJogadores = 2;
    Jogador* jogadores = (Jogador*) calloc(nJogadores, sizeof(Jogador));
    if (jogadores == NULL) {
        printf("Erro: memória insuficiente para jogadores.\n");
        liberarMemoriaMapa(mapa);
        exit(EXIT_FAILURE);
    }

    // Cadastro simples de nomes e cores dos jogadores
    for (int i = 0; i < nJogadores; i++) {
        printf("\nCadastro do Jogador %d\n", i + 1);
        printf("Nome: ");
        lerLinhaLimpa(jogadores[i].nome, sizeof(jogadores[i].nome));
        printf("Cor (dono): ");
        lerLinhaLimpa(jogadores[i].cor, sizeof(jogadores[i].cor));
        // Aloca espaço para a missão (tamanho suficiente)
        jogadores[i].missao = (char*) malloc(200 * sizeof(char)); // 200 bytes para descrição + prefixo
        if (jogadores[i].missao == NULL) {
            printf("Erro ao alocar missão para jogador %d.\n", i + 1);
            // liberar já alocado
            liberarMemoriaJogadores(jogadores, nJogadores);
            liberarMemoriaMapa(mapa);
            free(jogadores);
            exit(EXIT_FAILURE);
        }
    }

    // Atribuir missões aleatórias (armazenadas dinamicamente)
    int tipoIndicador[1];
    for (int i = 0; i < nJogadores; i++) {
        atribuirMissao(jogadores[i].missao, missoes, totalMissoes, tipoIndicador);
        printf("\nMissão do jogador '%s' (apenas exibida uma vez):\n", jogadores[i].nome);
        exibirMissao(jogadores[i].missao);
    }

    // Loop de jogo com menu simples
    int turno = 0; // índice do jogador atual
    int vencedor = -1;
    int opcao;
    do {
        printf("\n=== Turno do jogador: %s (cor: %s) ===\n", jogadores[turno].nome, jogadores[turno].cor);
        printf("1 - Exibir mapa\n");
        printf("2 - Atacar\n");
        printf("3 - Mostrar minha missão novamente\n");
        printf("4 - Passar turno\n");
        printf("5 - Sair\n");
        printf("Escolha: ");
        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n');
            opcao = 0;
            printf("Entrada inválida.\n");
            continue;
        }
        while (getchar() != '\n'); // limpar buffer

        if (opcao == 1) {
            exibirMapa(mapa, nTerritorios);
        } else if (opcao == 2) {
            exibirMapa(mapa, nTerritorios);
            int idxA, idxD;
            printf("Informe índice do território atacante: ");
            if (scanf("%d", &idxA) != 1) {
                while (getchar() != '\n');
                printf("Entrada inválida.\n");
                continue;
            }
            printf("Informe índice do território defensor: ");
            if (scanf("%d", &idxD) != 1) {
                while (getchar() != '\n');
                printf("Entrada inválida.\n");
                continue;
            }
            while (getchar() != '\n');

            if (idxA < 0 || idxA >= nTerritorios || idxD < 0 || idxD >= nTerritorios) {
                printf("Índices inválidos.\n");
                continue;
            }
            if (idxA == idxD) {
                printf("Um território não pode atacar a si mesmo.\n");
                continue;
            }
            // valida que atacante pertence ao jogador atual (mesma cor)
            if (strcmp(mapa[idxA].cor, jogadores[turno].cor) != 0) {
                printf("Ataque inválido: território atacante não é seu.\n");
                continue;
            }
            // não permitir atacar território do mesmo dono
            if (strcmp(mapa[idxA].cor, mapa[idxD].cor) == 0) {
                printf("Ataque inválido: defensor tem o mesmo dono.\n");
                continue;
            }
            // executar ataque (usa ponteiros)
            atacar(&mapa[idxA], &mapa[idxD]);

            // após ataque, verificar missão silenciosamente para o jogador atual
            if (verificarMissao(jogadores[turno].missao, mapa, nTerritorios, &jogadores[turno])) {
                vencedor = turno;
                printf("\nParabéns! O jogador '%s' cumpriu a missão!\n", jogadores[turno].nome);
                break;
            }

        } else if (opcao == 3) {
            printf("\nSua missão:\n");
            exibirMissao(jogadores[turno].missao);
        } else if (opcao == 4) {
            // antes de passar, verificar missão silenciosamente (caso tenha sido cumprida por outra ação)
            if (verificarMissao(jogadores[turno].missao, mapa, nTerritorios, &jogadores[turno])) {
                vencedor = turno;
                printf("\nParabéns! O jogador '%s' cumpriu a missão!\n", jogadores[turno].nome);
                break;
            }
            turno = (turno + 1) % nJogadores;
        } else if (opcao == 5) {
            printf("Saindo do jogo...\n");
            break;
        } else {
            printf("Opção inválida.\n");
        }

        // também verificar missão ao fim de cada ação (por segurança)
        if (verificarMissao(jogadores[turno].missao, mapa, nTerritorios, &jogadores[turno])) {
            vencedor = turno;
            printf("\nParabéns! O jogador '%s' cumpriu a missão!\n", jogadores[turno].nome);
            break;
        }

    } while (1);

    if (vencedor != -1) {
        printf("\n*** RESULTADO: Jogador vencedor -> %s (missão cumprida) ***\n", jogadores[vencedor].nome);
    } else {
        printf("\nJogo finalizado sem vencedor (ou saída antecipada).\n");
    }

    // liberar memória
    liberarMemoriaJogadores(jogadores, nJogadores);
    free(jogadores);
    liberarMemoriaMapa(mapa);

    printf("Memória liberada. Fim do programa.\n");
    return 0;
}

// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO
// ============================================================================
//        
// ============================================================================
//
// OBJETIVOS:
// - Modularizar completamente o código em funções especializadas.
// - Implementar um sistema de missões para um jogador.
// - Criar uma função para verificar se a missão foi cumprida.
// - Utilizar passagem por referência (ponteiros) para modificar dados e
//   passagem por valor/referência constante (const) para apenas ler.
// - Foco em: Design de software, modularização, const correctness, lógica de jogo.
//
// ============================================================================

// Inclusão das bibliotecas padrão necessárias para entrada/saída, alocação de memória, manipulação de strings e tempo.

// --- Constantes Globais ---
// Definem valores fixos para o número de territórios, missões e tamanho máximo de strings, facilitando a manutenção.

// --- Estrutura de Dados ---
// Define a estrutura para um território, contendo seu nome, a cor do exército que o domina e o número de tropas.

// --- Protótipos das Funções ---
// Declarações antecipadas de todas as funções que serão usadas no programa, organizadas por categoria.
// Funções de setup e gerenciamento de memória:
// Funções de interface com o usuário:
// Funções de lógica principal do jogo:
// Função utilitária:

// --- Função Principal (main) ---
// Função principal que orquestra o fluxo do jogo, chamando as outras funções em ordem.
int main() {
    // 1. Configuração Inicial (Setup):
    // - Define o locale para português.
    // - Inicializa a semente para geração de números aleatórios com base no tempo atual.
    // - Aloca a memória para o mapa do mundo e verifica se a alocação foi bem-sucedida.
    // - Preenche os territórios com seus dados iniciais (tropas, donos, etc.).
    // - Define a cor do jogador e sorteia sua missão secreta.

    // 2. Laço Principal do Jogo (Game Loop):
    // - Roda em um loop 'do-while' que continua até o jogador sair (opção 0) ou vencer.
    // - A cada iteração, exibe o mapa, a missão e o menu de ações.
    // - Lê a escolha do jogador e usa um 'switch' para chamar a função apropriada:
    //   - Opção 1: Inicia a fase de ataque.
    //   - Opção 2: Verifica se a condição de vitória foi alcançada e informa o jogador.
    //   - Opção 0: Encerra o jogo.
    // - Pausa a execução para que o jogador possa ler os resultados antes da próxima rodada.

    // 3. Limpeza:
    // - Ao final do jogo, libera a memória alocada para o mapa para evitar vazamentos de memória.

    return 0;
}

// --- Implementação das Funções ---

// alocarMapa():
// Aloca dinamicamente a memória para o vetor de territórios usando calloc.
// Retorna um ponteiro para a memória alocada ou NULL em caso de falha.

// inicializarTerritorios():
// Preenche os dados iniciais de cada território no mapa (nome, cor do exército, número de tropas).
// Esta função modifica o mapa passado por referência (ponteiro).

// liberarMemoria():
// Libera a memória previamente alocada para o mapa usando free.

// exibirMenuPrincipal():
// Imprime na tela o menu de ações disponíveis para o jogador.

// exibirMapa():
// Mostra o estado atual de todos os territórios no mapa, formatado como uma tabela.
// Usa 'const' para garantir que a função apenas leia os dados do mapa, sem modificá-los.

// exibirMissao():
// Exibe a descrição da missão atual do jogador com base no ID da missão sorteada.

// faseDeAtaque():
// Gerencia a interface para a ação de ataque, solicitando ao jogador os territórios de origem e destino.
// Chama a função simularAtaque() para executar a lógica da batalha.

// simularAtaque():
// Executa a lógica de uma batalha entre dois territórios.
// Realiza validações, rola os dados, compara os resultados e atualiza o número de tropas.
// Se um território for conquistado, atualiza seu dono e move uma tropa.

// sortearMissao():
// Sorteia e retorna um ID de missão aleatório para o jogador.

// verificarVitoria():
// Verifica se o jogador cumpriu os requisitos de sua missão atual.
// Implementa a lógica para cada tipo de missão (destruir um exército ou conquistar um número de territórios).
// Retorna 1 (verdadeiro) se a missão foi cumprida, e 0 (falso) caso contrário.

// limparBufferEntrada():
// Função utilitária para limpar o buffer de entrada do teclado (stdin), evitando problemas com leituras consecutivas de scanf e getchar.

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Variables globales seguras
volatile sig_atomic_t contador_usr1 = 0;
volatile sig_atomic_t ejecutar = 1;

// SIGINT: no termina
void handler_sigint(int sig) {
    printf("\nSIGINT recibido (Ctrl+C)\n");
}

// SIGTERM: termina
void handler_sigterm(int sig) {
    printf("\nSIGTERM recibido. Saliendo...\n");
    ejecutar = 0;
}

// SIGUSR1: suma contador
void handler_sigusr1(int sig) {
    contador_usr1++;
    printf("\nSIGUSR1 recibido. Contador = %d\n", contador_usr1);
}

int main() {
    struct sigaction sa_int, sa_term, sa_usr1, sa_ignore;
    sigset_t bloqueadas, antiguas;

    // Handlers
    sa_int.sa_handler = handler_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    sa_term.sa_handler = handler_sigterm;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    sigaction(SIGTERM, &sa_term, NULL);

    sa_usr1.sa_handler = handler_sigusr1;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    sigaction(SIGUSR1, &sa_usr1, NULL);

    // Bloquear SIGINT y SIGUSR1
    sigemptyset(&bloqueadas);
    sigaddset(&bloqueadas, SIGINT);
    sigaddset(&bloqueadas, SIGUSR1);

    sigprocmask(SIG_BLOCK, &bloqueadas, &antiguas);
    printf("SIGINT y SIGUSR1 bloqueadas...\n");

    sleep(5);

    // Desbloquear
    sigprocmask(SIG_SETMASK, &antiguas, NULL);
    printf("Señales desbloqueadas\n");

    // Ignorar SIGINT temporalmente
    sa_ignore.sa_handler = SIG_IGN;
    sigemptyset(&sa_ignore.sa_mask);
    sa_ignore.sa_flags = 0;
    sigaction(SIGINT, &sa_ignore, NULL);

    printf("Ignorando SIGINT por 5 segundos...\n");
    sleep(5);

    // Restaurar handler
    sigaction(SIGINT, &sa_int, NULL);
    printf("SIGINT restaurado\n");

    // Generar señal interna
    printf("Generando SIGUSR1 con raise()\n");
    raise(SIGUSR1);

    // Usar sigwait
    sigset_t set_wait;
    sigemptyset(&set_wait);
    sigaddset(&set_wait, SIGUSR1);

    sigprocmask(SIG_BLOCK, &set_wait, NULL);

    printf("Esperando SIGUSR1 con sigwait...\n");
    int sig;
    sigwait(&set_wait, &sig);
    printf("sigwait capturo: %d\n", sig);

    // Loop principal
    printf("Esperando señales...\n");

    while (ejecutar) {
        pause();
    }

    printf("Fin del programa\n");
    return 0;
}
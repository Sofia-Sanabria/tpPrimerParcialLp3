#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

volatile sig_atomic_t seguir = 1;

void manejar_error(const char *mensaje) {
    perror(mensaje);
    exit(EXIT_FAILURE);
}

void handler_sigusr1(int sig) {
    (void)sig;
    printf("SOY %d y recibi SIGUSR1\n", getpid());
    fflush(stdout);
}

void handler_sigterm(int sig) {
    (void)sig;
    printf("SOY %d y finalice\n", getpid());
    fflush(stdout);
    seguir = 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <N>\n", argv[0]);
        fprintf(stderr, "Donde N es la cantidad de hijos a crear\n");
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Error: N debe ser un entero positivo\n");
        return EXIT_FAILURE;
    }

    pid_t hijos[n];

    printf("Padre %d: creando %d hijos...\n", getpid(), n);

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            manejar_error("Error en fork");
        }

        if (pid == 0) {
            struct sigaction sa_usr1;
            struct sigaction sa_term;

            sigemptyset(&sa_usr1.sa_mask);
            sa_usr1.sa_flags = 0;
            sa_usr1.sa_handler = handler_sigusr1;
            if (sigaction(SIGUSR1, &sa_usr1, NULL) < 0) {
                manejar_error("Error al registrar SIGUSR1");
            }

            sigemptyset(&sa_term.sa_mask);
            sa_term.sa_flags = 0;
            sa_term.sa_handler = handler_sigterm;
            if (sigaction(SIGTERM, &sa_term, NULL) < 0) {
                manejar_error("Error al registrar SIGTERM");
            }

            printf("Hijo %d listo. PID = %d, Padre = %d\n", i + 1, getpid(), getppid());
            fflush(stdout);

            while (seguir) {
                pause();
            }

            _exit(i + 1);
        }

        hijos[i] = pid;
    }

    sleep(1);
    printf("\nPadre %d: hijos creados y esperando señales.\n", getpid());
    for (int i = 0; i < n; i++) {
        printf("Padre %d: hijo %d con PID %d\n", getpid(), i + 1, hijos[i]);
    }

    for (int i = 0; i < n; i++) {
        int estado;
        pid_t terminado = waitpid(hijos[i], &estado, 0);

        if (terminado < 0) {
            manejar_error("Error en waitpid");
        }

    }

    printf("Padre %d: todos los hijos finalizaron.\n", getpid());
    return EXIT_SUCCESS;
}

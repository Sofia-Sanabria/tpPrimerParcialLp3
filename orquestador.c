#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

// Función registrada con atexit() que se ejecuta cuando el padre termina
void finalizar_padre(void) {
    printf("\n=== El proceso padre (PID: %d) está finalizando correctamente ===\n", getpid());
}

// Función para manejar errores
void manejar_error(const char *mensaje) {
    perror(mensaje);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    // Verificar argumentos de línea de comandos
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <N>\n", argv[0]);
        fprintf(stderr, "Donde N es la cantidad de procesos hijos a crear\n");
        return EXIT_FAILURE;
    }
    
    // Convertir argumento a entero
    int N = atoi(argv[1]);
    if (N <= 0) {
        fprintf(stderr, "Error: N debe ser un entero positivo\n");
        return EXIT_FAILURE;
    }
    
    // Registrar función atexit para el padre
    if (atexit(finalizar_padre) != 0) {
        manejar_error("Error al registrar atexit");
    }
    
    // Inicializar semilla para números aleatorios
    srand(time(NULL));
    
    printf("Proceso padre (PID: %d) creando %d hijos...\n", getpid(), N);
    
    // Arreglo para almacenar los PIDs de los hijos
    pid_t hijos[N];
    
    // Crear N procesos hijos
    for (int i = 0; i < N; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            manejar_error("Error en fork");
        } 
        else if (pid == 0) {
            // Código del proceso hijo
            int tiempo_sleep = (rand() % 5) + 1; // Aleatorio entre 1 y 5 segundos
            
            printf("Hijo %d: PID = %d, Padre PID = %d, Durmiendo por %d segundos\n", 
                   i + 1, getpid(), getppid(), tiempo_sleep);
            
            sleep(tiempo_sleep);
            
            // Finalizar con código de salida = número del hijo (1-indexado)
            printf("Hijo %d: PID = %d, Terminando con código %d\n", 
                   i + 1, getpid(), i + 1);
            exit(i + 1);
        } 
        else {
            // Padre: guardar el PID del hijo creado
            hijos[i] = pid;
        }
    }
    
    // El padre espera la finalización de todos los hijos usando waitpid()
    printf("\nPadre esperando la finalización de los %d hijos...\n\n", N);
    
    for (int i = 0; i < N; i++) {
        int status;
        pid_t pid_finalizado = waitpid(hijos[i], &status, 0);
        
        if (pid_finalizado < 0) {
            manejar_error("Error en waitpid");
        }
        
        // Verificar cómo terminó el hijo
        if (WIFEXITED(status)) {
            int codigo_salida = WEXITSTATUS(status);
            printf("Hijo finalizado: PID = %d, Código de salida = %d\n", 
                   pid_finalizado, codigo_salida);
        } 
        else if (WIFSIGNALED(status)) {
            printf("Hijo finalizado: PID = %d, Terminado por señal %d\n", 
                   pid_finalizado, WTERMSIG(status));
        }
    }
    
    printf("\nTodos los hijos han finalizado.\n");
    
    // El programa termina y automáticamente se ejecuta la función registrada con atexit()
    return EXIT_SUCCESS;
}
# Trabajo Práctico - Lenguajes de Programación III

## Integrantes
- Sofia Sanabria
- Nicolas Villalba
- Abel Noguera

## Descripción
Se desarrollaron dos programas en C para trabajar con procesos y señales en sistemas UNIX.

## Archivos
- **orquestador.c**: crea N procesos hijos con `fork()`, cada uno ejecuta un `sleep()` y finaliza con un código. El padre espera con `waitpid()` y usa `atexit()` al terminar.
- **monitor.c**: maneja señales (`SIGINT`, `SIGTERM`, `SIGUSR1`) usando `sigaction`, `sigprocmask`, `sigwait` y `raise`.

## Compilación

```bash
gcc orquestador.c -o orquestador
gcc monitor.c -o monitor

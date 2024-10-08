#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define BUFFER_SIZE 1024

// Pipes para la comunicación
int pipe_hamburguesas[2];
int pipe_vegano[2];
int pipe_papas[2];
int pipe_despacho[2];

// Función para simular la preparación de hamburguesas
void* preparar_hamburguesas(void* arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        read(pipe_hamburguesas[0], buffer, BUFFER_SIZE);
        if (strlen(buffer) > 0) {
            sleep(2); // Simular tiempo de preparación
            write(pipe_despacho[1], buffer, strlen(buffer) + 1);
            printf("Hamburguesa lista: %s\n", buffer);
        }
    }
    return NULL;
}

// Función para simular la preparación de papas fritas
void* preparar_papas(void* arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        read(pipe_papas[0], buffer, BUFFER_SIZE);
        if (strlen(buffer) > 0) {
            sleep(1); // Simular tiempo de preparación
            write(pipe_despacho[1], buffer, strlen(buffer) + 1);
            printf("Papas fritas listas: %s\n", buffer);
        }
    }
    return NULL;
}

// Proceso de recepción de pedidos aleatorios
void recepcion_pedidos() {
    char pedido[BUFFER_SIZE];
    const char* tipos_comida[] = {"hamburguesa", "vegano", "papas"};
    srand(time(NULL)); // Inicializar la semilla para rand()

    while (1) {
        sleep(rand() % 3 + 1); // Esperar un tiempo aleatorio entre 1 y 3 segundos
        int tipo = rand() % 3; // Elegir un tipo de comida
        int es_vip = rand() % 4; // 25% de probabilidad de ser VIP

        if (es_vip == 0) {
            snprintf(pedido, BUFFER_SIZE, "VIP %s", tipos_comida[tipo]);  //formatear pedido
            printf("Cliente VIP hace un pedido: %s\n", pedido);
            write(pipe_hamburguesas[1], pedido + 4, strlen(pedido) - 4 + 1); // Enviar solo el tipo de comida
        } else {
            snprintf(pedido, BUFFER_SIZE, "%s", tipos_comida[tipo]);
            printf("Cliente hace un pedido: %s\n", pedido);
            write(pipe_hamburguesas[1], pedido, strlen(pedido) + 1);
        }
    }
}

// Proceso de atención al cliente
void atender_clientes() {
    char despacho[BUFFER_SIZE];
    while (1) {
        read(pipe_despacho[0], despacho, BUFFER_SIZE);
        if (strlen(despacho) > 0) {
            printf("Pedido despachado: %s\n", despacho);
        }
    }
}

int main() {
    // Crear los pipes
    pipe(pipe_hamburguesas);
    pipe(pipe_vegano);
    pipe(pipe_papas);
    pipe(pipe_despacho);

    // Crear hilos para la preparación de papas fritas
    pthread_t hilos[2];
    for (int i = 0; i < 2; i++) {
        pthread_create(&hilos[i], NULL, preparar_papas, NULL);
    }

    // Crear procesos
    if (fork() == 0) {
        preparar_hamburguesas(NULL);
        exit(0);
    }
    if (fork() == 0) {
        recepcion_pedidos();
        exit(0);
    }
    if (fork() == 0) {
        atender_clientes();
        exit(0);
    }

    // Esperar a que los hilos terminen (no se espera en este caso)
    for (int i = 0; i < 2; i++) {
        pthread_join(hilos[i], NULL);
    }

    return 0;
}

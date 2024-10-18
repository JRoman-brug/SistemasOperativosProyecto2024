#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 1024

// Pipes para la comunicación
int pipe_hamburguesas[2];
int pipe_vegano[2];
int pipe_papas[2];
int pipe_despacho[2];
int pipe_respuesta[2]; // Pipe para respuesta al cliente

// Estructura para manejar los pedidos
typedef struct {
    char tipo[20];
    int es_vip;
} Pedido;

// Proceso para simular la preparación de hamburguesas
void preparar_hamburguesas() {
    Pedido pedido;
    while (1) {
        read(pipe_hamburguesas[0], &pedido, sizeof(Pedido));
        if (strlen(pedido.tipo) > 0) {
            sleep(2); // Simular tiempo de preparación
            write(pipe_despacho[1], &pedido, sizeof(Pedido)); // Notificar que el pedido está listo
            printf("Hamburguesa lista: %s\n", pedido.tipo);
        }
    }
}

// Proceso para simular la preparación de papas fritas
void preparar_papas() {
    Pedido pedido;
    while (1) {
        read(pipe_papas[0], &pedido, sizeof(Pedido));
        if (strlen(pedido.tipo) > 0) {
            sleep(1); // Simular tiempo de preparación
            write(pipe_despacho[1], &pedido, sizeof(Pedido)); // Notificar que el pedido está listo
            printf("Papas fritas listas: %s\n", pedido.tipo);
        }
    }
}

// Proceso para simular la preparación de comida vegana
void preparar_vegano() {
    Pedido pedido;
    while (1) {
        read(pipe_vegano[0], &pedido, sizeof(Pedido));
        if (strlen(pedido.tipo) > 0) {
            sleep(1); // Simular tiempo de preparación
            write(pipe_despacho[1], &pedido, sizeof(Pedido)); // Notificar que el pedido está listo
            printf("Comida vegana lista: %s\n", pedido.tipo);
        }
    }
}

// Proceso de recepción de pedidos
void recepcion_pedidos() {
    Pedido pedido;
    const char* tipos_comida[] = {"hamburguesa", "vegano", "papas"};
    srand(time(NULL));

    while (1) {
        sleep(rand() % 3 + 1); // Espera aleatoria entre 1 y 3 segundos
        int tipo = rand() % 3;
        int es_vip = rand() % 4; // 25% de probabilidad de ser VIP
        pedido.es_vip = (es_vip == 0) ? 1 : 0; // Asignar si es VIP o normal
        strcpy(pedido.tipo, tipos_comida[tipo]);

        if (pedido.es_vip) {
            printf("Cliente VIP hace un pedido: %s\n", pedido.tipo);
        } else {
            printf("Cliente normal hace un pedido: %s\n", pedido.tipo);
        }

        // Enviar pedido al pipe correspondiente
        if (tipo == 0) {
            write(pipe_hamburguesas[1], &pedido, sizeof(Pedido));
        } else if (tipo == 1) {
            write(pipe_vegano[1], &pedido, sizeof(Pedido));
        } else {
            write(pipe_papas[1], &pedido, sizeof(Pedido));
        }
    }
}

// Proceso de atención al cliente
void atender_clientes() {
    Pedido despacho;
    Pedido vip_despacho; // Para manejar prioridad VIP
    int hay_vip = 0; // Indica si hay un pedido VIP listo  //Ver tema prioridades


/*lee del pipe de despacho y si es vip lo entrega primero 



*/

   while (1) {
    read(pipe_despacho[0], &despacho, sizeof(Pedido)); // Esperar un pedido listo

    // Despachar inmediatamente si es VIP
    if (despacho.es_vip) {
        printf("Despachando pedido VIP: %s\n", despacho.tipo);
        write(pipe_respuesta[1], &despacho, sizeof(Pedido));
    } else {
        // Despachar el pedido normal
        printf("Pedido despachado: Normal %s\n", despacho.tipo);
        write(pipe_respuesta[1], &despacho, sizeof(Pedido));
    }
}



// Proceso para simular la espera de los clientes
void atender_respuestas() {
    Pedido respuesta;
    while (1) {
        read(pipe_respuesta[0], &respuesta, sizeof(Pedido)); // Esperar la respuesta de despacho
        printf("Cliente recibido: %s %s\n", respuesta.es_vip ? "VIP" : "Normal", respuesta.tipo);
    }
}

int main() {
    // Crear los pipes
    pipe(pipe_hamburguesas);
    pipe(pipe_vegano);
    pipe(pipe_papas);
    pipe(pipe_despacho);
    pipe(pipe_respuesta); // Pipe para respuestas a los clientes

    // Crear procesos de recepción de pedidos
    if (fork() == 0) {
        recepcion_pedidos();
        exit(0);
    }

    // Crear procesos de preparación
    if (fork() == 0) {
        preparar_hamburguesas();
        exit(0);
    }

    if (fork() == 0) {
        preparar_vegano();
        exit(0);
    }

    if (fork() == 0) {
        preparar_papas();
        exit(0);
    }

    // Proceso de atención al cliente
    if (fork() == 0) {
        atender_clientes();
        exit(0);
    }

    // Proceso para simular la espera de los clientes
    if (fork() == 0) {
        atender_respuestas();
        exit(0);
    }

    // Cerrar extremos de lectura en el proceso principal
    close(pipe_hamburguesas[0]);
    close(pipe_vegano[0]);
    close(pipe_papas[0]);
    close(pipe_despacho[1]);
    close(pipe_respuesta[1]);

    // Esperar a que los procesos terminen (en este caso, no se espera)
    while (1) {
        pause(); // Esperar indefinidamente
    }

    return 0;
}




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

/*
Este código crea un socket, lo enlaza a un puerto específico, escucha conexiones entrantes y maneja cada conexión en un bucle infinito. 
Dentro del bucle, el código lee los datos enviados por el cliente, imprime la solicitud en la consola, procesa la solicitud (por hacer) 
y envía una respuesta de prueba al cliente.

Es importante tener en cuenta que este código solo maneja una conexión a la vez. 
Para manejar múltiples conexiones, se debe utilizar una técnica de multiproceso o multihilo. Además, este código no maneja errores 
ni verifica la entrada del cliente, lo que lo hace inseguro.
*/

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CONNECTIONS 10
#define MAX_REQUEST_SIZE 4096
#define MAX_RESPONSE_SIZE 4096
#define OK 200
#define BAD_REQUEST 400
#define NOT_FOUND 404


// Definir la estructura de la petición HTTP:
typedef struct {
    char method[8];
    char path[256];
    char version[16];
} http_request;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main()
{
    // crea el socket
    int sockfd, newsockfd, n;
    socklen_t clilen;

    // crea el buffer
    /* El buffer es un array de caracteres que se utiliza para almacenar los datos que se leen del socket y que se envían como respuesta al cliente. */
    char buffer[BUFFER_SIZE];

    /* 
    "serv_addr" es una estructura de tipo sockaddr_in que representa la dirección del socket en el lado del servidor. Esta estructura contiene información sobre el protocolo a utilizar, la dirección IP del servidor y el número de puerto que se está utilizando.
    "cli_addr" es una estructura de tipo sockaddr_in que representa la dirección del socket en el lado del cliente. Cuando se establece una conexión con un cliente, se utiliza esta estructura para almacenar la dirección del cliente. */
    struct sockaddr_in serv_addr, cli_addr;

    // inicializa el socket

    /* AF_INET y SOCK_STREAM son dos constantes que se utilizan para especificar el tipo de socket que se desea crear en la función socket().
    AF_INET significa "Address Family - Internet", lo que indica que se utilizará la familia de protocolos de Internet para la comunicación. Es decir, el socket creado podrá ser utilizado para comunicación a través de IPv4 o IPv6.
    SOCK_STREAM significa que el socket será orientado a conexión y se utilizará el protocolo TCP (Transmission Control Protocol) para la transmisión de datos. Es decir, el socket creado será capaz de transmitir datos de manera confiable y ordenada, garantizando que los datos lleguen sin errores y en el mismo orden en que se enviaron. */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // si hay un error inicializando el socket
    if (sockfd < 0)
        error("Error opening socket");

    // Limpiar y configurar la estructura de direcciones del servidor
    /* bzero((char *) &serv_addr, sizeof(serv_addr));: Esta función se utiliza para establecer todos los bytes de la estructura de dirección serv_addr a 0. Es una forma rápida de asegurarse de que la estructura está limpia antes de configurar los valores.
    serv_addr.sin_family = AF_INET;: Establece la familia de direcciones a utilizar en la estructura serv_addr como IPv4.
    serv_addr.sin_addr.s_addr = INADDR_ANY;: Asigna la dirección IP del servidor a la dirección del host local. Esto significa que el servidor aceptará conexiones entrantes desde cualquier dirección IP.
    serv_addr.sin_port = htons(PORT);: Establece el puerto en el que el servidor estará escuchando las conexiones entrantes. htons() convierte el número de puerto del byte ordenado por host al byte ordenado por red. En este caso, se está utilizando el valor predefinido PORT (8080). */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // hacer bind del socket
    /* La línea if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) verifica si la función bind() falló, lo que significa que no se pudo asociar el socket a la dirección y puerto especificados. En ese caso, la función error() se llama con un mensaje de error correspondiente. */
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("Error on binding");

    // poner el socket a escuchar
    /* La función listen(sockfd, 5); indica que el socket creado en sockfd estará a la escucha de conexiones entrantes, y el número 5 indica el tamaño máximo de la cola de conexiones pendientes que el sistema operativo mantendrá para este socket.
    La variable clilen es del tipo socklen_t y es utilizada para almacenar el tamaño de la estructura cli_addr, que se utilizará en la función accept() más adelante para aceptar nuevas conexiones. En esta línea se le asigna el valor del tamaño de cli_addr, que es obtenido a través de la función sizeof(). */
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    printf("Server started on port %d...\n", PORT);

    // loop de escucha
    while (1) {
        /* La función accept() espera y acepta conexiones entrantes.
        Si la conexión se establece correctamente, accept() devuelve un nuevo descriptor de archivo (newsockfd) que se utiliza para enviar y recibir datos en la conexión. */
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
            error("Error on accept");

        /*  inicializa todos los elementos del buffer buffer con valor 0 */
        bzero(buffer, BUFFER_SIZE);

        /* read() se utiliza para leer los datos enviados por el cliente desde newsockfd y almacenarlos en el búfer buffer. */
        n = read(newsockfd, buffer, BUFFER_SIZE);
        if (n < 0)
            error("Error reading from socket");

        /* La línea printf("Received request:\n%s\n", buffer); imprime en la consola el mensaje "Received request" seguido del contenido del buffer que contiene la solicitud recibida. */
        printf("Received request:\n%s\n", buffer);

        /* Luego, se utiliza la función strstr() para buscar en la solicitud recibida si se trata de una solicitud GET, HEAD o POST. Dependiendo del resultado de esta búsqueda, se imprime en la consola un mensaje indicando el tipo de solicitud recibida. */
        // Parse and handle the request
        if (strstr(buffer, "GET") != NULL) {
            printf("GET request\n");
            // TODO: Handle GET request
        } else if (strstr(buffer, "HEAD") != NULL) {
            printf("HEAD request\n");
            // TODO: Handle HEAD request
        } else if (strstr(buffer, "POST") != NULL) {
            printf("POST request\n");
            // TODO: Handle POST request
        } else {
            printf("Unknown request\n");
        }

        /* Después, se escribe en el socket la respuesta a la solicitud utilizando la función write(). En este caso, se envía un mensaje de respuesta HTTP simple que contiene el encabezado "HTTP/1.1 200 OK" seguido de los encabezados "Content-Type" y "Content-Length" y, finalmente, un mensaje de "Hello world!". */
        n = write(newsockfd, "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!", 74);
        if (n < 0)
            error("Error writing to socket");

        close(newsockfd);
    }

    close(sockfd);
    return 0;
}

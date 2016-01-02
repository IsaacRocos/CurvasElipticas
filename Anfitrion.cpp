#include "h/Chat.h"
#include "h/Mensaje.h"


int main(int argv, char** argc){
        int host_port= 1101;
        struct sockaddr_in my_addr;
        int hsock;
        int * p_int ;
        int err;

        socklen_t addr_size = 0;
        int* cSocket;
        sockaddr_in sadr;
        pthread_t thread_id=5;
        pthread_t thread_id1=0;

        hsock = socket(AF_INET, SOCK_STREAM, 0);
        if(hsock == -1){
                printf("Error inicializando socket %d\n", errno);
        }

        p_int = (int*)malloc(sizeof(int));
        *p_int = 1;
        if((setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||
                (setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 )){
                printf("Error fijando opciones %d\n", errno);
                free(p_int);
        }
        free(p_int);

        my_addr.sin_family = AF_INET ;
        my_addr.sin_port = htons(host_port);

        memset(&(my_addr.sin_zero), 0, 8);
        my_addr.sin_addr.s_addr = INADDR_ANY ;

        if(bind( hsock, (sockaddr*)&my_addr, sizeof(my_addr)) == -1 ){
          printf("Error enlazando el socket, asegurese de que el puerto esta libre. %d\n",errno);
        }
        if(listen( hsock, 10) == -1 ){
          printf("Error en listen %d\n",errno);
        }

        string usuario;
        cout<<"Nombre de Usuario: ";
        getline(cin,usuario);
        setUsuario(usuario);
        setTUsuario(1);
        
        //Esperar conexiones
        printf("Esperando alguien con quien conversar...\n");
        addr_size = sizeof(sockaddr_in);
        int tamanioMSG;
        while(true){
                cSocket = (int*)malloc(sizeof(int));
                if((*cSocket = accept( hsock, (sockaddr*)&sadr, &addr_size))!= -1){
                        printf("---------------------\nPeticion de chat desde %s\n---------------------\n",inet_ntoa(sadr.sin_addr));
                        //Enviar nombre de usuario y bandera "disponible"
                        Mensaje mensaje = mkMsMSJSesion(1,usuario);
                        char* paquete = serializarMensaje(&mensaje,&tamanioMSG);
                        enviar_Mensaje(cSocket,paquete,&tamanioMSG);
                        //Escuchar y enviar mensajes
                        pthread_create(&thread_id,0,&SocketHandler, (void*)cSocket);
                        pthread_create(&thread_id1,0,&conversar, (void*)cSocket);
                        pthread_detach(thread_id);
                }
                else{
                        printf("Error aceptando conexion %d\n", errno);
                }
        }
}

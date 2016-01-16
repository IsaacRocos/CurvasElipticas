/*
* Invitado. Se ejecuta cuando el usuario desea unirse a otro para chatear.
*/

#include "h/Chat.h"


int init_Socket(int *chat_Socket){
	*chat_Socket = socket(AF_INET, SOCK_STREAM, 0);
    if(*chat_Socket == -1){
        printf("Error initializing socket %d\n",errno);
    }else{
    	printf("Socket inicializado.\n");
    }
}


int conectar_Chat(int * p_int, int *chat_Socket, struct sockaddr_in *direccion, int * host_port, char * host_name){
	p_int = (int*)malloc(sizeof(int));
    *p_int = 1;
    if((setsockopt(*chat_Socket, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 ) || 
    	(setsockopt(*chat_Socket, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 )){
		printf("Error setting options %d\n",errno);
        free(p_int);
    }else{
    	printf("Opciones establecidas.\n");
    }
    free(p_int);
    
    direccion->sin_family = AF_INET ;
    direccion->sin_port = htons(*host_port);
    memset(&(direccion->sin_zero), 0, 8);
    direccion->sin_addr.s_addr = inet_addr(host_name);

    if( connect( *chat_Socket, (struct sockaddr*)&*direccion, sizeof(*direccion)) == -1 ){
        if(errno != EINPROGRESS){
            printf("Error conectando socket. CE: %d\n", errno);
        }
    }else{
       	printf("¡Conectado! \n");
        int tamanioMSG;
        Mensaje conect = mkMsMSJSesion(1,getUsuario());
        char *paquete = serializarMensaje(&conect,&tamanioMSG);
        enviar_Mensaje(chat_Socket,paquete,&tamanioMSG);
    }
}


int cerrarConexion(int *chat_Socket){
	close(*chat_Socket);
}


int main(int argc, char** argv){

	int host_port= 1101;
    char* host_name="127.0.0.1";
    //char* host_name="192.168.43.28";
    struct sockaddr_in direccion;
    pthread_t thread_id=0;
    pthread_t thread_id1=0;
    char buffer[1024];
    int buffer_len=0;
    int chat_Socket;
    int *p_int;
    int err;
    Mensaje mensaje;
    int tamanioMSG;
    char *msjSerializado;

    if(argc==2){
        host_name = argv[1];
        setMostrarFlujo(0);
    }else if(argc ==3){
        host_name = argv[1];
        setMostrarFlujo(atoi(argv[2]));
    }else{
        cout << "Usando dirección local"<<endl;
        setMostrarFlujo(0);
    }

    string usuario;
    cout<<">> Nombre de Usuario: ";
    getline(cin,usuario);
    setUsuario(usuario);
    setTUsuario(2);

    cout<<"Conectando a: "<<host_name<<endl;

    init_Socket(&chat_Socket);
	conectar_Chat(p_int,&chat_Socket, &direccion,&host_port,host_name);    
    pthread_create(&thread_id,0,&conversar, (void*)&chat_Socket);
    pthread_create(&thread_id1,0,&SocketHandler, (void*)&chat_Socket);
    void* estado;
    pthread_join(thread_id, &estado);
    pthread_join(thread_id1, &estado);
    cerrarConexion(&chat_Socket);
    return 0; 
}



#include "h/Chat.h"
#include "h/Mensaje.h"

int tUsuario = 0;
string usuario;
string amigo = "Amigo";
Mensaje* parametrosDiffie;
Mensaje* puntoLlave;
int llavePublica;

void setUsuario(string us){
	usuario = us;
}

void setTUsuario(int tipo){
	tUsuario = tipo;
}

string getUsuario(void){
	return usuario;
}

// Funcion que se ejecuta en un hilo. Permite leeer e iniciar envio de mensajes de conversación
void* conversar(void *refSocket){
    int* chatSocket = (int*)refSocket;
    string dialogo;
    char * paquete;
    int tamanioMSG;
    while(1){
        cout<<usuario<<"-----"<<endl;
        cout<<">> ";
        getline(cin,dialogo);
        cout<<"----------------------------"<<endl;
        Mensaje mensaje = mkMsMSJDialogo(dialogo);
        paquete = serializarMensaje(&mensaje,&tamanioMSG);
        enviar_Mensaje(chatSocket,paquete,&tamanioMSG);
    }
}

// 
void leerMSJ(int cSocket,google::protobuf::uint32 siz)
{
  int bytecount;
  Mensaje mensaje;
  char buffer [siz+4];
  if((bytecount = recv(cSocket, (void *)buffer, 4+siz, MSG_WAITALL))== -1){
    fprintf(stderr, "Error recibiendo datos %d\n", errno);
  }
  google::protobuf::io::ArrayInputStream ais(buffer,siz+4);
  CodedInputStream coded_input(&ais);
  coded_input.ReadVarint32(&siz);
  google::protobuf::io::CodedInputStream::Limit msgLimit = coded_input.PushLimit(siz);
  mensaje.ParseFromCodedStream(&coded_input);
  coded_input.PopLimit(msgLimit);
  //cout<<"El mensaje es:  "<<mensaje.DebugString();
  interpretarMSJ(&mensaje, tUsuario, &usuario, &amigo, parametrosDiffie, puntoLlave);
}

void* SocketHandler(void* lp){
    int *cSocket = (int*)lp;
        char buffer[4];
        int bytecount=0;
        string output,pl;
        memset(buffer, '0', 4);
        while (1) {
          //Obtener el tamanio del paquete
          if((bytecount = recv(*cSocket, buffer, 4, MSG_PEEK))== -1){
              printf("Error recibiendo datos %d\n", errno);
          }else if (bytecount == 0)
                  break;
          leerMSJ(*cSocket,leerTamanioMSJ(buffer));
        }
}

int enviar_Mensaje(int *chat_Socket, char* paquete, int *tamanio){
  //printf("\t\t Enviando %d bytes...\n",*tamanio);
  int byteCont;
  if( (byteCont = send(*chat_Socket, (void *)paquete,*tamanio,0))== -1 ) {
      printf("Error enviando datos %d\n", errno);
    }
    //printf("Bytes enviados: %d\n", byteCont);
    //usleep(3000);
}

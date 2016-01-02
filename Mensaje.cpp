#include "h/Mensaje.h"

// Identifica el tipo de Mensaje recibido y realiza 
// las operaciones correspondientes al tipo 
void interpretarMSJ(Mensaje* msj,int tUsuario, string* usuario, string* amigo, Mensaje* parametrosDiffie, Mensaje* puntoLlave){
    cout<<"\x1b[A \r                                 "<<endl;
    cout<<"\r                                        "<<endl;
    int tipoMSJ = msj->msg_id();
    switch(tipoMSJ){
      case 1:
        parametrosDiffie = msj;
        // Calcular llavePublica con cierto proceso y recortar a 10 bits      break;
      	// Crear mensaje DiffiePunto y enviar llavePublica
      break;
      case 2:
        //Se recibe el punto que correspnde a la clave publica
        puntoLlave = msj;
      case 3:
        if(msj->msgsn().idevento() == 1){
          *amigo = msj->msgsn().usuario();
          if(tUsuario==1){
            cout<<*amigo << " quiere chatear contigo."<<endl<<endl;
          }else{
            cout<<*amigo << " acepta chatear contigo."<<endl<<endl;
          }
        }
      break;
      case 4:
        //Llamada a DES para descifrar texto contenido en mensaje
        cout<<"\t\t\t\t"<<*amigo<<": --------"<<endl;
        cout<<"\t\t\t\t"<< msj->msgd().msg() <<" <<"<<endl;
        cout<<"\t\t\t\t----------------------------"<<endl;
        cout<<*usuario<<"-----"<<endl;
      break;
    }
}


// Crea un mensaje de tipo Diffieparams (parametros de la curva para el protocolo DH).
Mensaje mkMsDiffieParams(int p, int a, int b, int g_x, int g_y, int n, int h){
	Mensaje msg;
	msg.set_msg_id(1);
	DiffieParams* dpm = msg.mutable_dprms();
	dpm->set_p(p);
	dpm->set_a(a);
	dpm->set_b(b);
	dpm->set_g_x(g_x);
	dpm->set_g_y(g_y);
	dpm->set_n(n);
	dpm->set_h(h);
	return msg;
}


// Crea un mensaje de tipo DiffiePunto (intercambio de puntos durante ejecucion de protocolo DH).
Mensaje mkMsDiffiePunto(int xVal, int yVal){
	Mensaje msg;
	msg.set_msg_id(2);
	DiffiePunto* dpt = msg.mutable_dpt();
	dpt->set_x_val(xVal);
	dpt->set_y_val(yVal);
	return msg;
}


// Crea un mensaje de tipo MSJ Sesion. Anuncia conexion o desconexion de @usuario.
Mensaje mkMsMSJSesion(int idEvento, string usuario){
    Mensaje msg;
    msg.set_msg_id(3);
    MSJSesion* msgsn = msg.mutable_msgsn();
    msgsn->set_idevento(idEvento);
    msgsn->set_usuario(usuario);
    return msg;
}


// Crea un mensaje de tipo MSJDialogo. Contiene un dialogo de una conversacion.
Mensaje mkMsMSJDialogo(string dialogo){
    Mensaje msg;
    msg.set_msg_id(4);
    MSJDialogo* msgd = msg.mutable_msgd();
    msgd->set_msg(dialogo);
    return msg;
}


// Serializa un mensaje en un paquete listo para enviar por un socket
char* serializarMensaje(Mensaje *mensaje, int *tamanio){
    *tamanio = mensaje->ByteSize()+4;
    //cout<<"Tamanio msj: "<<*tamanio<<endl;
    char *pkt = new char [*tamanio];
    google::protobuf::io::ArrayOutputStream  aos(pkt,*tamanio);
    CodedOutputStream *coded_output = new CodedOutputStream(&aos);
    coded_output->WriteVarint32(mensaje->ByteSize());
    mensaje->SerializeToCodedStream(coded_output);
    //printf("Mensaje listo.\n");
    //cout<<pkt<<endl;
    return pkt;
}


google::protobuf::uint32 leerTamanioMSJ(char *buf)
{
  google::protobuf::uint32 tamsj;
  google::protobuf::io::ArrayInputStream ais(buf,4);
  CodedInputStream coded_input(&ais);
  coded_input.ReadVarint32(&tamsj);
  //cout<<"Tamanio de mensaje: "<<tamsj<<endl;
  return tamsj;
}


// Muestra la estrutura protobuf de un mensaje y su contenido.
void debugMensaje(Mensaje* msj){
	cout<<"----------------------"<<endl;
	cout<<"Contenido de mensaje: "<<endl;
	cout<<msj->DebugString();	
	cout<<"----------------------"<<endl;
}


// Funcion de prueba de mensajes con parametros arbitrarios.
void testMesajes(void){
	Mensaje msg1 = mkMsDiffieParams(1,2 ,3 ,4 ,5 ,6 ,7);
	debugMensaje(&msg1);
	Mensaje msg2 = mkMsDiffiePunto(34,65);
	debugMensaje(&msg2);
	Mensaje msg3 = mkMsMSJSesion(1,"ISAAC");
	debugMensaje(&msg3);
	Mensaje msg4 = mkMsMSJDialogo("Hola !!");
	debugMensaje(&msg4);	
	int tamanio = 0;
	char* msjSerializado = serializarMensaje(&msg3,&tamanio);
}

/*
int main(){
	testMesajes();
	return 0;
}
*/

//Fuente con info sobre oneof para mensajes en protobuff
//https://developers.google.com/protocol-buffers/docs/proto#oneof

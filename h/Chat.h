#ifndef _CHAT_H_
#define _CHAT_H_

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "../proto/MPI.pb.h"

#include <time.h>
#include <iostream>
#include <sys/types.h> 
#include <unistd.h>
#include "google/protobuf/message.h"
#include "google/protobuf/descriptor.h"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "google/protobuf/io/zero_copy_stream_impl_lite.h"


using namespace std;
using namespace google::protobuf;
using namespace google::protobuf::io;



// -----------------------------------------------
// Prototipos de funcionalidad de chat 
// -----------------------------------------------
	void setMostrarFlujo(int valor);
	void setUsuario(string us);
	void setTUsuario(int tipo);
	string getUsuario(void);
	void* SocketHandler(void* lp);
	void* conversar(void *refSocket);
	void  leerMSJ(int cSocket,google::protobuf::uint32 siz);
	int   enviar_Mensaje(int *chat_Socket, char* paquete, int *tamanio);
	void set_n(int val);
	void set_pxPub(int val);
	void set_pyPub(int val);
	int get_n(void);
	int* get_pxPub(void);
	int* get_pyPub(void);
	//void set_parametrosDiffie(Mensaje* msj);
	void set_parametrosDiffie(int vp, int va, int vb, int vg_x, int vg_y);
//-----------------------------------------



// ---------------------------------------------
// Prototipos funciones para operar con mensajes
// ---------------------------------------------
	google::protobuf::uint32 leerTamanioMSJ(char *buf);
	void debugMensaje(Mensaje* msg);
	Mensaje* interpretarMSJ(Mensaje* msj, int* chatSocket);
	Mensaje mkMsDiffieParams(int p, int a, int b, int g_x, int g_y, int n, int h);
	Mensaje mkMsDiffiePunto(int xVal, int yVal, int tipo);
	Mensaje mkMsMSJSesion(int idEvento, string usuario);
	Mensaje mkMsMSJDialogo(string dialogo);
	char* serializarMensaje(Mensaje *mensaje, int *tamanio);
	void testMesajes(void);
	int escalar(int p);
//-----------------------------------------


// ------------------------------------------------
// Prototipos funciones para operaciones con puntos
// ------------------------------------------------
	int curva(int a, int b, int p,int *p1,int *p2);
	int powmod(int a, int x,int p);
	int convertirModulo(int num,int mod);
	int inversoMultiplicativo(int a,int b);
	int multil1(int a,int p,int px,int py,int n,int *px1,int *py1);
	int llave(int n,int p1,int p2);
	int escalar(int p);
	int bits(int px, int py );
//-------------------------------------------------


// ------------------------------------------------
// Prototipos de funciones para Cifrar y Descifrar
// ------------------------------------------------

unsigned char sdes_enc(int k, unsigned char m);
unsigned char sdes_dec(int k, unsigned char c);
string CBCd(string ct, int k);
string CBCe(string pt, int k);
//-------------------------------------------------


#endif

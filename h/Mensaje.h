#ifndef _MENSAJE_H_
#define _MENSAJE_H_

#include <sys/types.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#include "../proto/MPI.pb.h"

#include "google/protobuf/message.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl_lite.h"

using namespace google::protobuf;
using namespace google::protobuf::io;
using namespace std;

google::protobuf::uint32 leerTamanioMSJ(char *buf);
void debugMensaje(Mensaje* msg);
void interpretarMSJ(Mensaje* msj,int tUsuario, string* usuario, string* amigo, Mensaje* parametrosDiffie, Mensaje* puntoLlave);
Mensaje mkMsDiffieParams(int p, int a, int b, int g_x, int g_y, int n, int h);
Mensaje mkMsDiffiePunto(int xVal, int yVal);
Mensaje mkMsMSJSesion(int idEvento, string usuario);
Mensaje mkMsMSJDialogo(string dialogo);
char* serializarMensaje(Mensaje *mensaje, int *tamanio);
void testMesajes(void);

#endif

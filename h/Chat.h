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

#include <iostream>
#include "google/protobuf/message.h"
#include "google/protobuf/descriptor.h"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace std;
using namespace google::protobuf::io;

void setUsuario(string us);
void setTUsuario(int tipo);
string getUsuario(void);
void* SocketHandler(void* lp);
void* conversar(void *refSocket);
void  leerMSJ(int cSocket,google::protobuf::uint32 siz);
int   enviar_Mensaje(int *chat_Socket, char* paquete, int *tamanio);

#endif

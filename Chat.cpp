/* *************************************************************
	|Chat.cpp|
	Contiene la  implementación de todas las funciones del Chat
*/ 

#include "h/Chat.h"

/* Atributos de Chat */
int tUsuario;
string usuario;
string amigo;
Mensaje* parametrosDiffie;
Mensaje* llavePubAmigo;
int n;
int p, a, b, g_x, g_y;
int pxPub;
int pyPub;  
int pxSesion;
int pySesion;
int llaveSesion;
int mostrarFlujo;

/*
**************************************************************************
**** Funciones de manejo de sockets **************************************
**************************************************************************
*/

void setMostrarFlujo(int valor){
	mostrarFlujo = valor;
}

void set_parametrosDiffie(int vp, int va, int vb, int vg_x, int vg_y){
	p = vp;
	a = va;
	b = vb; 
	g_x = vg_x; 
	g_y = vg_y;
}

void setUsuario(string us){
	usuario = us;
}

void setTUsuario(int tipo){
	tUsuario = tipo;
}

string getUsuario(void){
	return usuario;
}

void set_n(int val){
  n = val;
}

void set_pxPub(int val){
  pxPub = val;
}

void set_pyPub(int val){
  pyPub = val;
}

int get_n(void){
  return n;
}

int* get_pxPub(void){
  return &pxPub;
}

int* get_pyPub(void){
  return &pyPub ;
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
        //Mensaje mensaje = mkMsMSJDialogo(dialogo);
        Mensaje mensaje = mkMsMSJDialogo(CBCe(dialogo,llaveSesion));
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
  interpretarMSJ(&mensaje,&cSocket);
}

//
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

//
int enviar_Mensaje(int *chat_Socket, char* paquete, int *tamanio){
  //printf("\t\t Enviando %d bytes...\n",*tamanio);
  int byteCont;
  if( (byteCont = send(*chat_Socket, (void *)paquete,*tamanio,0))== -1 ) {
      printf("Error enviando datos %d\n", errno);
    }
    //printf("Bytes enviados: %d\n", byteCont);
    //usleep(3000);
}


/* *************************************************************************
   *	Funciones de manejo de MENSAJES ************************************
   *************************************************************************
*/

// Identifica el tipo de Mensaje recibido y realiza 
// las operaciones correspondientes al tipo 
Mensaje* interpretarMSJ(Mensaje* msj, int* chatSocket){
    cout<<"\x1b[A \r                                 "<<endl;
    cout<<"\r                                        "<<endl;
    int tipoMSJ = msj->msg_id();
    Mensaje *mensajeRes = NULL;
    char* paquete;
    switch(tipoMSJ){
      case 1:
        parametrosDiffie = msj;
        
        if(mostrarFlujo){
        	cout<<"Parametros recibidos: "<<endl;
        	debugMensaje(msj);
    	}
        set_parametrosDiffie(msj->dprms().p(), msj->dprms().a(), msj->dprms().b(), msj->dprms().g_x(), msj->dprms().g_y());
        //Se calcula n
        set_n( escalar(msj->dprms().p()) );
        // Se hace  n*P(x,y)
        
       multil1(msj->dprms().a() , msj->dprms().p() , msj->dprms().g_x() , msj->dprms().g_y() , n , &pxPub , &pyPub);


        // Crear y enviar un mensaje que contenga el punto calculado P(pxPub,pyPub)
        mensajeRes = new Mensaje();
        *mensajeRes = mkMsDiffiePunto(pxPub,pyPub,1);
        int tamanioMSG;
        // Serializando
        paquete = serializarMensaje(mensajeRes,&tamanioMSG); 
        // Enviando punto calculado
        enviar_Mensaje(chatSocket,paquete,&tamanioMSG);
      break;
      case 2:
        //Se recibe el punto que corresponde a la clave publica del otro usuario
        llavePubAmigo = msj;
      	if(mostrarFlujo){
	      	cout<<"Punto recibido: "<<endl;
	        debugMensaje(msj);
	        cout<<"Parametros actuales: "<<endl;
	        cout<<"a: "<<a<<endl;
	        cout<<"p: "<<p<<endl;
	        cout<<"Ppub=: ("<<pxPub<<","<<pyPub<<")"<<endl;
	        cout<<"n: "<<n<<endl;
	    }
        
        // Calcular punto de Sesion
        //multil1(parametrosDiffie->dprms().a() , parametrosDiffie->dprms().p() , pxPub , pyPub , n , &pxSesion , &pySesion);
        
        //multil1(a , p , pxPub , pyPub , n , &pxSesion , &pySesion);
        multil1(a , p , msj->dpt().x_val() , msj->dpt().y_val() , n , &pxSesion , &pySesion);
        
        if(mostrarFlujo){
        	printf("(%d,%d)\n",pxSesion, pySesion );
        }

        llaveSesion = bits(pxSesion, pySesion);

        if(mostrarFlujo){
        	cout<<"Llave Sesion 10 bits: "<< llaveSesion<<endl;
        	cout<<"P(x,y) = " << pxSesion << " , " << pySesion << endl;
        }
        cout<<usuario<<"-----"<<endl;
        cout<<">> ";
        break;
      case 3:
        if(msj->msgsn().idevento() == 1){
          amigo = msj->msgsn().usuario();
          if(tUsuario==1){
            cout<<amigo << " quiere chatear contigo."<<endl<<endl;
          }else{
            cout<<amigo << " acepta chatear contigo."<<endl<<endl;
          }
        }
      break;
      case 4:
        //Llamada a DES para descifrar texto contenido en mensaje
      	//desCifrarCadena(msj->msgd().msg());
        cout<<"\t\t\t\t"<<amigo<<": --------"<<endl;
        cout<<"\t\t\t\t"<< CBCd(msj->msgd().msg(),llaveSesion) <<" <<"<<endl;
        if (mostrarFlujo){
      		cout<<"\t\t\t\t"<< msj->msgd().msg() <<" <<"<<endl<<endl;
  		}
  		cout<<"\t\t\t\t----------------------------"<<endl;
        cout<<usuario<<"-----"<<endl;
        
      break;
    }
    return mensajeRes;
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
Mensaje mkMsDiffiePunto(int xVal, int yVal, int tipo){
	Mensaje msg;
	msg.set_msg_id(2);
	DiffiePunto* dpt = msg.mutable_dpt();
	dpt->set_x_val(xVal);
	dpt->set_y_val(yVal);
	dpt->set_tipo(tipo);
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

//
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
	Mensaje msg2 = mkMsDiffiePunto(34,65,1);
	debugMensaje(&msg2);
	Mensaje msg3 = mkMsMSJSesion(1,"ISAAC");
	debugMensaje(&msg3);
	Mensaje msg4 = mkMsMSJDialogo("Hola !!");
	debugMensaje(&msg4);	
	int tamanio = 0;
	char* msjSerializado = serializarMensaje(&msg3,&tamanio);
}


/* *************************************************************************
   *	Funciones de manejo de PUNTOS en Curvas Elipticas    ***************
   *************************************************************************
*/

int bits(int px, int py )
{
	int pol=566,b;// para poder enviar los 10 bits al cifrado viene del polinomio irreducible de la caja S en el AES, solo recorrido una posicion a la izquierda
	b = px^py;  //se hace un xor entre los puntos de la K 
	b = b^pol;  //se hace otro xor con el polinomio seleccionado 
	return b; //se regresa el valor calculado
}

int escalar(int p)
{
	//se determina el valor aleatorio que se multiplicara por el punto aleatorio
	int n;
	do
	{
		n = rand() % (p-1);
	}while(n <= 2);
	return n;
}

int curva(int a, int b, int p,int *p1,int *p2)
{
	srand(time(NULL));
	//Ejemplo para hallar "x^3 mod p" 
	int x,lim,conta=0,n,j,i,w=0;;
	int px1,py1;
	int* valido;
	lim = ( (p-1)/2) ;
	valido = (int*)malloc(lim*sizeof(int)); //memoria dinamica para los valores validos de y
	for (j = 1; j <= lim ; j++)
	{
		valido[j] = powmod(j,2,p); //se hace la operacion y^2 mod p
		for ( i = 0; i < p; i++)
		{
			x = powmod(i,3,p) + a*i + b; //se realiza la operacion de la ecuacion de la curva	
			x = convertirModulo(x,p); // se saca el modulo del resultado anterior
			if (valido[j] == x) // vemos que numeros de y^2 son iguales al de x evaluada en la ecuacion de la curva
			{
				conta++; // indica cuantos elementos son validos
			}
		}
	}
	int px[conta]; // el numero de componentes x de los puntos
	conta = conta*2; 
	int py[conta]; // el numero de compones y de los puntos
	int z=0,y=0,r=1,ramdomx,ramdomy;
	for ( j = 1; j <= lim; j++)
	{
		for ( i = 0; i < p ; i++)
		{
			x = powmod(i,3,p) + a*i + b;	//se realiza la operacion de la ecuacion de la curva
			x = convertirModulo(x,p); //se saca el modulo del resultado anterior
			if (valido[j] == x ) // vemos que numeros de y^2 son iguales al de x evaluada en la ecuacion de la curva
			{
				px[z] = i; // ya que "x" sera parte de dos puntos con diferente coordenada "y" ejemplo= P1(x1,y1),P2(x1,y2). se asigna el valor 
				py[y] = j ; //se le asigna el valor de la raiz positiva a la componente y
				y++;
				py[y] = p-j;//se le asigna el valor de la raiz negativa "modulo p" a la componente "y" del siguiente punto 
				z++;
				y++;
				
			}
		}
	}
	
	do
	{
		ramdomx = rand() % z; //se genera el punto aleatorio de la componente x
		ramdomy = rand() % 2 ; // se selecciona la posicion del componente y
		
		if (ramdomy == 0) // es para sacar el punto que correponde a y, que sea valido para x
		{	
			ramdomy=(ramdomx*2);
		}
		else
		{	
			ramdomy=(ramdomx*2)+1;
		}

			for ( i= 2; i <= p+1 ; i++)
			{
				multil1(a,p,px[ramdomx],py[ramdomy],i,&px1,&py1); //este punto sera el que se envie al usuario, para encontrar la k
				for ( j = 0; j < z; j++)
				{
					if (px1 == px[j])
					{
						if (py1==py[j*2] || py1==py[(j*2)+1])
						{
							w=0;
						}	
						else 
						{
							w++;
						}
					}			
				}
			}
		}while(w >= 1);

	*p1=px[ramdomx];
   	*p2=py[ramdomy];

   	return 0;  
}

int multil1(int a,int p,int px,int py,int n,int *px1,int *py1)
{
	int m,num,den,rx,ry,tx,ty,res,escalar,tope;
	//se realiza las operaciones necesarias para la la operacion 2P
	res = powmod(px,2,p);
	num = 3*res+a;
	den = 2 * py;
	m = num * inversoMultiplicativo(den,p);
	m = convertirModulo (m,p);
	rx =  powmod(m,2,p) - (2*px);
	rx = convertirModulo (rx,p);
	ry = -py + m*(px-rx);
	ry = convertirModulo (ry,p);		
	//se realizan las operaciones necesarias para la multiplicacion mayor a 2P, siendo como limite el numero aleatorio seleccionado
	if (n != 2)
	{
		for (tope = 2; tope < n; tope++)
		{
			m = convertirModulo(convertirModulo((ry-py),p)*inversoMultiplicativo(convertirModulo((rx-px),p),p),p);
			tx = convertirModulo(powmod(m,2,p)-rx-px,p);
			ty = convertirModulo(-ry+m*(rx-tx),p);
			rx=tx;
			ry=ty;
		}
	}
	// se regresa el valor de la multiplicacion realizada
	*px1 = rx;
	*py1 = ry;
	return 0;
}

int powmod(int a, int x, int p )
{
	int i;
	int res=a;
	for(i=1;i<x;i++)
	{
		res*=a;
		res=convertirModulo(res,p);
	}
	return res;
}

int convertirModulo(int num,int mod)
{
	if(num<0)
		return convertirModulo(num+mod, mod);
	else if (num>(mod-1))
		return convertirModulo(num-mod, mod);
	return num;
}

int inversoMultiplicativo(int a,int b)
{
	int i;
	for(i=2;i<=b;i++)
		if(b%i==0 && a%i==0)
			return -1;
	int q,x,y,r,d;
	int u=a,v=b;
	int x1=1,y1=0,x2=0,y2=1;
	while(u!=0)
	{
		q=(v/u); r=v-(q*u); x=x2-(q*x1); y=y2-(q*y1);
		v=u; u=r; x2=x1; x1=x; y2=y1; y1=y;
	}
	d=convertirModulo(v, b); 
	x=convertirModulo(x2, b); 
	y=convertirModulo(y2, b);
	return x;
}

// ------------------------------------
// ------ Cifrado y descifrado  -------
// ------------------------------------

///////// S-DES (Cifrado)
unsigned char sdes_dec(int k, unsigned char c){
    unsigned char kbit[10]={0};
    unsigned char sk1,sk2;//subllaves
    unsigned char aux,aux2,i=0;
    unsigned char pizq=0, pder=0,fder;
    unsigned char key;
    unsigned char rs0,rs1;
    int S0[16]={1,0,3,2,3,2,1,0,0,2,1,3,3,1,3,2};
	int S1[16]={0,1,2,3,2,0,1,3,3,0,1,0,2,1,0,3};
	//printf("c%d",c);
	/****************************
	 *Obtención de subclaves 	*
	 *K1 y K2					*
	 ****************************/
	 
	//INICIO//obtención de los 10 bits de la llave dada (se almacena en un arreglo)
	kbit[0]=k&1;
	for(i=1;i<10;i++)
	{
		kbit[i]=(k>>i)&1;
	}
	//FIN//obtencion de los 10 bits de la llave dada
	
	//INICIO//Permutación P10
	//kbit[4]<<=0; - No necesario
	kbit[2]<<=1;
	kbit[1]<<=2;
	kbit[9]<<=3;
	kbit[0]<<=4;
	//kbit[6]<<=0; - No necesario
	kbit[3]<<=1;
	kbit[8]<<=2;
	kbit[5]<<=3;
	kbit[7]<<=4;
	//FIN//Permutación P10
	
	//INICIO//Obtención de particion derecha e izquierda
	//Partición derecha (bits 0-4)
	pder=kbit[4]+kbit[2]+kbit[1]+kbit[9]+kbit[0];
	//Partición izquierda (bits 5-9)
	pizq=kbit[6]+kbit[3]+kbit[8]+kbit[5]+kbit[7];
	//FIN//Obtencion de particiones
	
	
	//INICIO//Corrimiento circular (Partición derecha)
	aux=(pder>>4)&1;
	pder = pder & ~(aux<<4);
	pder<<=1;
	pder+=aux;
	//FIN//Corrimiento circular (Partición derecha)
	
	//INICIO//Corrimiento circular (Partición izquierda)
	aux=(pizq>>4)&1;
	pizq = pizq & ~(aux<<4);
	pizq<<=1;
	pizq+=aux;
	pizq<<=5;//se añade corrimiento de 5 para poder ser concatenado de nuevo(recordatorio llave es de 10 bits NO 8)
	//FIN//Corrimiento circular (Partición izquierda)
	
	//Concatenamos Partición Izq y Der
	k=pizq+pder;
	
	
	//INICIO//Obtencion de los primeros 8 bits resultado de los corrimientos
	kbit[0]=k&1;
	//Corrimientos con el fin de obtener la P8
	for(i=1;i<8;i++)
	{
		kbit[i]=(k>>i)&1;
	}
	
	//INICIO//Permutacion P8
	//kbit[1]<<=0; - No necesario
	kbit[0]<<=1;
	kbit[5]<<=2;
	kbit[2]<<=3;
	kbit[6]<<=4;
	kbit[3]<<=5;
	kbit[7]<<=6;
	kbit[4]<<=7;

	//INICIO//SubClave K1 8 bits
	sk1=kbit[1]+kbit[0]+kbit[5]+kbit[2]+kbit[6]+kbit[3]+kbit[7]+kbit[4];
	
	
	//INICIO//Corrimiento circular 2 (Partición derecha) - se toman los resultados de los corrimientos anteriores
	aux=(pder>>4)&1;
	aux2=(pder>>3)&1;
	pder = pder & ~(aux<<4);
	pder = pder & ~(aux2<<3);
	pder<<=2;
	pder+=(aux<<1)+aux2;
	
	//INICIO//Corrimiento circular 2(Partición izquierda)
	pizq>>=5;
	aux=(pizq>>4)&1;
	aux2=(pizq>>3)&1;
	pizq = pizq & ~(aux<<4);
	pizq = pizq & ~(aux<<3);
	pizq<<=2;
	pizq+=aux;
	pizq<<=5;//se añade esto para poder ser concatenado de nuevo
	
	//Concatenamos Partición Izq y Der de los corrimientos a 2
	k=pizq+pder;
	
	
	//INICIO// Se obtienen los 10 bits del resultado anterior para realizar la permutacion P8
	kbit[0]=k&1;
	//Corrimientos con el fin de obtener la P8
	for(i=1;i<8;i++)
	{
		kbit[i]=(k>>i)&1;
	}

	//INICIO//Permutacion P8-2
	//kbit[1]<<=0; - No necesario
	kbit[0]<<=1;
	kbit[5]<<=2;
	kbit[2]<<=3;
	kbit[6]<<=4;
	kbit[3]<<=5;
	kbit[7]<<=6;
	kbit[4]<<=7;

	//INICIO//SubClave K2 8 bits
	sk2=kbit[1]+kbit[0]+kbit[5]+kbit[2]+kbit[6]+kbit[3]+kbit[7]+kbit[4];
	
	//FIN// OBTENCION DE SUBCLAVES K1 y K2
	
	
	
	
    /* aplicamos la permutación IP al texto cifrado*/
    
    kbit[0]=c&1;
	//Corrimientos con el fin de obtener la 1P
	for(i=1;i<8;i++)
	{
		kbit[i]=(c>>i)&1;
	}
	
	//Corrimientos con el fin de obtener la 1P
	//kbit[7]<<=0;
	kbit[5]<<=1;
	kbit[2]<<=2;
	kbit[6]<<=3;
	pizq=kbit[7]+kbit[5]+kbit[2]+kbit[6];
	
	fder=kbit[1]<<0;//bit 0 - 1
	fder+=kbit[3]<<1;//bit 1 - 2
	fder+=kbit[0]<<2;//bit 2 - 3
	fder+=kbit[4]<<3;//bit 3 - 0
	
	
	/*permutación de expansión*/
	
	kbit[4]<<=0;//bit 3 - 0
	kbit[1]<<=1;//bit 0 - 1
	kbit[3]<<=2;//bit 1 - 2
	kbit[0]<<=3;//bit 2 - 3
	pder=kbit[1]+kbit[4]+kbit[0]+kbit[3];
	kbit[7]=kbit[3]<<2;//bit 1 - 4
	kbit[5]=kbit[0]<<2;//bit 2 - 5
	kbit[2]=kbit[4]<<6;//bit 3 - 6
	kbit[6]=kbit[1]<<6;//bit 0 - 7
	pder+=kbit[7]+kbit[5]+kbit[2]+kbit[6];
	key=pder^sk2;
	
	kbit[0]=key&1;
	//Corrimientos con el fin de obtener la 1P
	for(i=1;i<8;i++)
	{
		kbit[i]=(key>>i)&1;
	}
	
	//Corrimientos con el fin de obtener la P8-2
	kbit[0]<<=0;
	kbit[1]<<=0;
	kbit[2]<<=1;
	kbit[3]<<=1;
	kbit[4]<<=0;
	kbit[5]<<=0;
	kbit[6]<<=1;
	kbit[7]<<=1;
	
	aux=kbit[4]+kbit[7];
	aux2=kbit[5]+kbit[6];
	aux=(aux<<2)+aux2;
	rs0=S0[aux];
	
	aux=kbit[0]+kbit[3];
	aux2=kbit[1]+kbit[2];
	aux=(aux<<2)+aux2;
	rs1=S1[aux];

	aux=(rs0<<2)+rs1;
	
	/*permutacion P4*/
	kbit[0]=aux&1;
	//Corrimientos con el fin de obtener la 1P
	for(i=1;i<4;i++)
	{
		kbit[i]=(aux>>i)&1;
	}
	
	
	kbit[0]<<=2;
	kbit[1]<<=1;
	kbit[2]<<=3;
	kbit[3]<<=0;
	
	aux=kbit[0]+kbit[1]+kbit[2]+kbit[3];
	pizq=aux^pizq;
	
	pder=pizq;
	pizq=fder;
	fder=pder;
	
	kbit[0]=pder&1;
	//Corrimientos con el fin de obtener la 1P
	for(i=1;i<4;i++)
	{
		kbit[i]=(pder>>i)&1;
	}
	
	/*permutacion de expansion*/
	kbit[3]<<=0;//bit 3 - 0
	kbit[0]<<=1;//bit 0 - 1
	kbit[1]<<=2;//bit 1 - 2
	kbit[2]<<=3;//bit 2 - 3
	pder=kbit[1]+kbit[2]+kbit[0]+kbit[3];
	kbit[7]=kbit[1]<<2;//bit 1 - 4
	kbit[5]=kbit[2]<<2;//bit 2 - 5
	kbit[2]=kbit[3]<<6;//bit 3 - 6
	kbit[6]=kbit[0]<<6;//bit 0 - 7
	pder+=kbit[7]+kbit[5]+kbit[2]+kbit[6];

	
	pder^=sk1;
	
	///
	kbit[0]=pder&1;
	//Corrimientos con el fin de obtener la 1P
	for(i=1;i<8;i++)
	{
		kbit[i]=(pder>>i)&1;
	}
	
	//Corrimientos con el fin de obtener la P8-2
	kbit[0]<<=0;
	kbit[1]<<=0;
	kbit[2]<<=1;
	kbit[3]<<=1;
	kbit[4]<<=0;
	kbit[5]<<=0;
	kbit[6]<<=1;
	kbit[7]<<=1;
	
	aux=kbit[4]+kbit[7];
	aux2=kbit[5]+kbit[6];
	aux=(aux<<2)+aux2;
	rs0=S0[aux];
	
	aux=kbit[0]+kbit[3];
	aux2=kbit[1]+kbit[2];
	aux=(aux<<2)+aux2;
	rs1=S1[aux];
	
	aux=(rs0<<2)+rs1;
	
	//
	kbit[0]=aux&1;
	//Corrimientos con el fin de obtener la 1P
	for(i=1;i<4;i++)
	{
		kbit[i]=(aux>>i)&1;
	}
	
	kbit[0]<<=2;
	kbit[1]<<=1;
	kbit[2]<<=3;
	kbit[3]<<=0;
	
	aux=kbit[0]+kbit[1]+kbit[2]+kbit[3];
	pizq=aux^pizq;
	
	aux=(pizq<<4)+fder;
	
	//
	kbit[0]=aux&1;
	//Corrimientos con el fin de obtener la 1P
	for(i=1;i<8;i++)
	{
		kbit[i]=(aux>>i)&1;
	}
	
	//Corrimientos con el fin de obtener la P8-2
	kbit[2]<<=0;
	kbit[0]<<=1;
	kbit[6]<<=2;
	kbit[1]<<=3;
	kbit[3]<<=4;
	kbit[5]<<=5;
	kbit[7]<<=6;
	kbit[4]<<=7;
	
	aux=kbit[1]+kbit[0]+kbit[5]+kbit[2]+kbit[6]+kbit[3]+kbit[7]+kbit[4];
	//printf("p%d ",aux);
	return aux;	
}

///////// S-DES (Descifrado)
unsigned char sdes_enc(int k, unsigned char m){
	int S0[16]={1,0,3,2,3,2,1,0,0,2,1,3,3,1,3,2};//caja S0
	int S1[16]={0,1,2,3,2,0,1,3,3,0,1,0,2,1,0,3};//ja S1
	unsigned char aux,aux2,i=0;//variables auxiliares/temporales y contador
	unsigned char sk1,sk2;//subllaves
	unsigned char pizq=0, pder=0,fder;//particiones derecha izquierda y una fija
	unsigned char kbit[10]={0};//almacenamiento de bits para pemutaciones
	unsigned char rs0,rs1;//resultados de posiciones en caja S0 y S1
	
	/****************************
	 *Obtención de subclaves 	*
	 *K1 y K2					*
	 ****************************/
	 
	//INICIO//obtención de los 10 bits de la llave dada (se almacena en un arreglo)
	kbit[0]=k&1;
	for(i=1;i<10;i++)
	{
		kbit[i]=(k>>i)&1;
	}
	//FIN//obtencion de los 10 bits de la llave dada
	
	//INICIO//Permutación P10
	//kbit[4]<<=0; - No necesario
	kbit[2]<<=1;
	kbit[1]<<=2;
	kbit[9]<<=3;
	kbit[0]<<=4;
	//kbit[6]<<=0; - No necesario
	kbit[3]<<=1;
	kbit[8]<<=2;
	kbit[5]<<=3;
	kbit[7]<<=4;
	//FIN//Permutación P10
	
	//INICIO//Obtención de particion derecha e izquierda
	//Partición derecha (bits 0-4)
	pder=kbit[4]+kbit[2]+kbit[1]+kbit[9]+kbit[0];
	//Partición izquierda (bits 5-9)
	pizq=kbit[6]+kbit[3]+kbit[8]+kbit[5]+kbit[7];
	//FIN//Obtencion de particiones
	
	
	//INICIO//Corrimiento circular (Partición derecha)
	aux=(pder>>4)&1;
	pder = pder & ~(aux<<4);
	pder<<=1;
	pder+=aux;
	//FIN//Corrimiento circular (Partición derecha)
	
	//INICIO//Corrimiento circular (Partición izquierda)
	aux=(pizq>>4)&1;
	pizq = pizq & ~(aux<<4);
	pizq<<=1;
	pizq+=aux;
	pizq<<=5;//se añade corrimiento de 5 para poder ser concatenado de nuevo(recordatorio llave es de 10 bits NO 8)
	//FIN//Corrimiento circular (Partición izquierda)
	
	//Concatenamos Partición Izq y Der
	k=pizq+pder;
	
	
	//INICIO//Obtencion de los primeros 8 bits resultado de los corrimientos
	kbit[0]=k&1;
	//Corrimientos con el fin de obtener la P8
	for(i=1;i<8;i++)
	{
		kbit[i]=(k>>i)&1;
	}
	
	//INICIO//Permutacion P8
	//kbit[1]<<=0; - No necesario
	kbit[0]<<=1;
	kbit[5]<<=2;
	kbit[2]<<=3;
	kbit[6]<<=4;
	kbit[3]<<=5;
	kbit[7]<<=6;
	kbit[4]<<=7;

	//INICIO//SubClave K1 8 bits
	sk1=kbit[1]+kbit[0]+kbit[5]+kbit[2]+kbit[6]+kbit[3]+kbit[7]+kbit[4];
	
	
	//INICIO//Corrimiento circular 2 (Partición derecha) - se toman los resultados de los corrimientos anteriores
	aux=(pder>>4)&1;
	aux2=(pder>>3)&1;
	pder = pder & ~(aux<<4);
	pder = pder & ~(aux2<<3);
	pder<<=2;
	pder+=(aux<<1)+aux2;
	
	//INICIO//Corrimiento circular 2(Partición izquierda)
	pizq>>=5;
	aux=(pizq>>4)&1;
	aux2=(pizq>>3)&1;
	pizq = pizq & ~(aux<<4);
	pizq = pizq & ~(aux<<3);
	pizq<<=2;
	pizq+=aux;
	pizq<<=5;//se añade esto para poder ser concatenado de nuevo
	
	//Concatenamos Partición Izq y Der de los corrimientos a 2
	k=pizq+pder;
	
	
	//INICIO// Se obtienen los 10 bits del resultado anterior para realizar la permutacion P8
	kbit[0]=k&1;
	//Corrimientos con el fin de obtener la P8
	for(i=1;i<8;i++)
	{
		kbit[i]=(k>>i)&1;
	}

	//INICIO//Permutacion P8-2
	//kbit[1]<<=0; - No necesario
	kbit[0]<<=1;
	kbit[5]<<=2;
	kbit[2]<<=3;
	kbit[6]<<=4;
	kbit[3]<<=5;
	kbit[7]<<=6;
	kbit[4]<<=7;

	//INICIO//SubClave K2 8 bits
	sk2=kbit[1]+kbit[0]+kbit[5]+kbit[2]+kbit[6]+kbit[3]+kbit[7]+kbit[4];
	
	//FIN// OBTENCION DE SUBCLAVES K1 y K2
	
	/****************
	 * S-DES		*
	 * Cifrado		*
	 ****************/
	
	//INICIO// ronda #1
	
	//INICIO//Obtencion de los 8 bits del bloque del mensaje
	kbit[0]=m&1;
	//Corrimientos con el fin de obtener la 1P
	for(i=1;i<8;i++)
	{
		kbit[i]=(m>>i)&1;
	}
	
	//Permutación 1P
	//kbit[7]<<=0;
	kbit[5]<<=1;
	kbit[2]<<=2;
	kbit[6]<<=3;
	pizq=kbit[7]+kbit[5]+kbit[2]+kbit[6];
	
	fder=kbit[1]<<0;//bit 0 - 1
	fder+=kbit[3]<<1;//bit 1 - 2
	fder+=kbit[0]<<2;//bit 2 - 3
	fder+=kbit[4]<<3;//bit 3 - 0
	
	
	//Corrimiento con el fin de obtener la parde derecha en expancion en 8
	kbit[4]<<=0;//bit 3 - 0
	kbit[1]<<=1;//bit 0 - 1
	kbit[3]<<=2;//bit 1 - 2
	kbit[0]<<=3;//bit 2 - 3
	pder=kbit[1]+kbit[4]+kbit[0]+kbit[3];
	kbit[7]=kbit[3]<<2;//bit 1 - 4
	kbit[5]=kbit[0]<<2;//bit 2 - 5
	kbit[2]=kbit[4]<<6;//bit 3 - 6
	kbit[6]=kbit[1]<<6;//bit 0 - 7
	pder+=kbit[7]+kbit[5]+kbit[2]+kbit[6];

	k=pder^sk1;
	
	kbit[0]=k&1;
	
	//obtencion de bit con el fin de obtener la las posiciones en So y S1
	for(i=1;i<8;i++)
	{
		kbit[i]=(k>>i)&1;
	}
	
	//Corrimientos con el fin de obtener la las posiciones en So y S1
	kbit[0]<<=0;
	kbit[1]<<=0;
	kbit[2]<<=1;
	kbit[3]<<=1;
	kbit[4]<<=0;
	kbit[5]<<=0;
	kbit[6]<<=1;
	kbit[7]<<=1;
	
	aux=kbit[4]+kbit[7];
	aux2=kbit[5]+kbit[6];
	aux=(aux<<2)+aux2;
	rs0=S0[aux];
	
	aux=kbit[0]+kbit[3];
	aux2=kbit[1]+kbit[2];
	aux=(aux<<2)+aux2;
	rs1=S1[aux];
	

	
	aux=(rs0<<2)+rs1;
	
	kbit[0]=aux&1;
	//Corrimientos con el fin de obtener la 1P
	for(i=1;i<4;i++)
	{
		kbit[i]=(aux>>i)&1;
	}
	
	kbit[0]<<=2;
	kbit[1]<<=1;
	kbit[2]<<=3;
	kbit[3]<<=0;
	
	aux=kbit[0]+kbit[1]+kbit[2]+kbit[3];
	pizq=aux^pizq;
	
	pder=pizq;
	pizq=fder;
	fder=pder;
	
	
	kbit[0]=pder&1;
	//Corrimientos con el fin de obtener la 1P
	for(i=1;i<4;i++)
	{
		kbit[i]=(pder>>i)&1;
	}
	
	//Corrimiento con el fin de obtener la parde derecha en expansion en 8
	kbit[3]<<=0;//bit 3 - 0
	kbit[0]<<=1;//bit 0 - 1
	kbit[1]<<=2;//bit 1 - 2
	kbit[2]<<=3;//bit 2 - 3
	pder=kbit[1]+kbit[2]+kbit[0]+kbit[3];
	kbit[7]=kbit[1]<<2;//bit 1 - 4
	kbit[5]=kbit[2]<<2;//bit 2 - 5
	kbit[2]=kbit[3]<<6;//bit 3 - 6
	kbit[6]=kbit[0]<<6;//bit 0 - 7
	pder+=kbit[7]+kbit[5]+kbit[2]+kbit[6];
	

	pder^=sk2;
	
	
	/////////////////////////
	
	kbit[0]=pder&1;
	//Corrimientos con el fin de obtener la 1P
	for(i=1;i<8;i++)
	{
		kbit[i]=(pder>>i)&1;
	}
	
	//Corrimientos con el fin de obtener la P8-2
	kbit[0]<<=0;
	kbit[1]<<=0;
	kbit[2]<<=1;
	kbit[3]<<=1;
	kbit[4]<<=0;
	kbit[5]<<=0;
	kbit[6]<<=1;
	kbit[7]<<=1;
	
	aux=kbit[4]+kbit[7];
	aux2=kbit[5]+kbit[6];
	aux=(aux<<2)+aux2;
	rs0=S0[aux];
	
	aux=kbit[0]+kbit[3];
	aux2=kbit[1]+kbit[2];
	aux=(aux<<2)+aux2;
	rs1=S1[aux];
	
	/////////////////////
	
	aux=(rs0<<2)+rs1;
	
	kbit[0]=aux&1;
	//Corrimientos con el fin de obtener la 1P
	for(i=1;i<4;i++)
	{
		kbit[i]=(aux>>i)&1;
	}
	
	kbit[0]<<=2;
	kbit[1]<<=1;
	kbit[2]<<=3;
	kbit[3]<<=0;
	
	aux=kbit[0]+kbit[1]+kbit[2]+kbit[3];
	pizq=aux^pizq;
	
	/////////////////////
	aux=(pizq<<4)+fder;
	
	///////////
	
	kbit[0]=aux&1;
	//Corrimientos con el fin de obtener la 1P
	for(i=1;i<8;i++)
	{
		kbit[i]=(aux>>i)&1;
	}
	
	//Corrimientos con el fin de obtener la P8-2
	kbit[2]<<=0;
	kbit[0]<<=1;
	kbit[6]<<=2;
	kbit[1]<<=3;
	kbit[3]<<=4;
	kbit[5]<<=5;
	kbit[7]<<=6;
	kbit[4]<<=7;
	
	aux=kbit[1]+kbit[0]+kbit[5]+kbit[2]+kbit[6]+kbit[3]+kbit[7]+kbit[4];
	//printf("%d ",aux);
	return aux;
}


string CBCe(string pt, int k){
	char *m = strdup(pt.c_str());
	unsigned char c[strlen(m)+1];
	srand(time(NULL));
	unsigned char IV =rand()%255;//Vector de Inicializacion
	unsigned char res=IV;
	
	for(int i=0; i < strlen(m); i++){
		res^=m[i];
		res=sdes_enc(k,res);
		c[i]=res;
    }
    c[strlen(m)]=IV;
    std::cout << "\n";
    std::string ct( c, c + sizeof c / sizeof c[0] );
    return ct;
}


string CBCd(string ct, int k){
	char *c = strdup(ct.c_str());
    unsigned char m[strlen(c)+1];
    unsigned char res;
    res=c[strlen(c)-1];
    
    for(int i=0; i < strlen(c)-1; i++){
		res^=sdes_dec(k,c[i]);
		m[i]=res;
		res=c[i];
    }
    std::cout << "\n";
    std::string pt( m, m-1 + sizeof m-1 / sizeof m[0] );
    return pt;
}








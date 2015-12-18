#include <stdio.h>
#include <stdlib.h>

int curva(int a, int b, int p);
int powmod(int a, int x,int p);
int convertirModulo(int num,int mod);
//hola 
//hola soy are
int main(int argc, char const *argv[])
{
	int *punto;
	int a,b,p;

	curva(2,5,103);
	return 0;
}

int curva(int a, int b, int p)
{

	//Ejemplo para hallar "x^3 mod p" 
	int x,lim,conta=0,y;
	int* valido;
	lim = ( (p-1)/2) ;

	valido = (int*)malloc(lim*sizeof(int)); //memoria dinamica para los valores validos de y

	printf("Residuos cuadraticos: \n");
	for (int j = 1; j <= lim ; j++)
	{
		valido[j] = powmod(j,2,p); //se hace la operacion y^2 mod p

		printf(" %d\n",valido[j] ); // se almacenan los valores encontrados
		
		for (int i = 0; i < p; i++)
		{
			x = powmod(i,3,p) + a*i + b; //se realiza la operacion de la ecuacion de la curva	
			x = convertirModulo(x,p); // se saca el modulo del resultado anterior

	//		printf("%d: %d\n",i,x);

			if (valido[j] == x) // vemos que numeros de y^2 son iguales al de x evaluada en la ecuacion de la curva
			{
	//			printf("%d\n",x);
				conta++; // indica cuantos elementos son validos
			}
		}
	}
//	printf("%d\n",conta );

	int px[conta]; // el numero de componentes x de los puntos
	conta = conta*2; 
	int py[conta]; // el numero de compones y de los puntos
	
	int z=0,r=1;

	printf("Puntos que pertenecen a la curva: \n");
	
	
	for (int j = 1; j <= lim; j++)
	{
		for (int i = 0; i < p ; i++)
		{
			x = powmod(i,3,p) + a*i + b;	//se realiza la operacion de la ecuacion de la curva
			x = convertirModulo(x,p); //se saca el modulo del resultado anterior

			if (valido[j] == x ) // vemos que numeros de y^2 son iguales al de x evaluada en la ecuacion de la curva
			{
				px[z] = i; // ya que "x" sera parte de los puntos don diferente coordenada "y" ejemplo= P1(x1,y1),P2(x1,y2). se asigna el valor 
			//	printf("px=%d\n",px[z] );  
				py[z] = j ; //se le asigna el valor de la raiz positiva a la componente y
				py[z+2] = p-j;//se le asigna el valor de la raiz negativa "modulo p" a la componente "y" del siguiente punto 
			//	printf("%d\n",z );
				printf("%d: (%d,%d)\n",r,px[z],py[z] );
			//	printf("%d\n",z+2 );
				r++;
				printf("%d: (%d,%d)\n",r,px[z],py[z+2] );
				r++;
				z++;
			}
		}
	}
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

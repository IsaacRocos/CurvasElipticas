#Correr en terminal con:   bash Make.sh
echo ""
echo "|||Chat Seguro|||"
echo ">> Compilando fuentes... "
if [ -f obj/Chat.o ];
	then
		echo ">> Chat.o Borrando..."
		rm obj/Chat.o
fi

echo ">> Chat.cpp  Compilando... "
g++ Chat.cpp -c  proto/MPI.pb.cc proto/MPI.pb.h -L /usr/lib/ -I /usr/include/ -lprotobuf -pthread
mv Chat.o obj/

echo ">> Anfitrion.cpp Compilando... "
g++ Anfitrion.cpp  obj/Chat.o  proto/MPI.pb.cc proto/MPI.pb.h -L /usr/lib/ -I /usr/include/ -lprotobuf -pthread -o ANFITRION

echo ">> Invitado.cpp  Compilando... "
g++ Invitado.cpp   obj/Chat.o  proto/MPI.pb.cc proto/MPI.pb.h -L /usr/lib/ -I /usr/include/ -lprotobuf -pthread -o INVITADO

echo "Proceso terminado."

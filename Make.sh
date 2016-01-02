#Correr en terminal con:   bash Make.sh
echo ""
echo ">>Compilando fuentes... "
if [ -f obj/Mensaje.o ];
	then
	echo ">>Borrando Mensaje.o"
	rm obj/Mensaje.o
fi
if [ -f obj/Chat.o ];
	then
		echo ">>Borrando Chat.o"
		rm obj/Chat.o
fi
echo ">>Compilando Mensaje.cpp"
g++ Mensaje.cpp -c  proto/MPI.pb.cc proto/MPI.pb.h -L /usr/lib/ -I /usr/include/ -lprotobuf -pthread
echo ">>Compilando Chat.cpp"
g++ Chat.cpp -c  proto/MPI.pb.cc proto/MPI.pb.h -L /usr/lib/ -I /usr/include/ -lprotobuf -pthread
mv Mensaje.o Chat.o obj/
echo ">Compilando Anfitrion.cpp"
g++ Anfitrion.cpp obj/Mensaje.o obj/Chat.o  proto/MPI.pb.cc proto/MPI.pb.h -L /usr/lib/ -I /usr/include/ -lprotobuf -pthread -o ANFITRION
echo ">>Compilando Invitado.cpp"
g++ Invitado.cpp obj/Mensaje.o obj/Chat.o  proto/MPI.pb.cc proto/MPI.pb.h -L /usr/lib/ -I /usr/include/ -lprotobuf -pthread -o INVITADO
echo "Proceso terminado."

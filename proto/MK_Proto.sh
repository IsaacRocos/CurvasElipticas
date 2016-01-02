#Correr en terminal con:   bash MK_Proto.sh
echo ""
echo ">>Compilando MPI.proto... "
protoc MPI.proto --cpp_out=.  
echo ">>Proceso terminado."
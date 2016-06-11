rm -f saída.data &&
make &&
time mpiexec -np $1 ./main $2

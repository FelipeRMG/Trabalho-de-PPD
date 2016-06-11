#Executar por último bash remote_run.sh p;(onde p é o número de processos)
#Este Shell com a ajuda estimável do técnico de laborario: Jean Barbosa Siqueira e do Colega Acadêmico: Alvaro Leonel Socorro Queiroz Mariano
#Qualquer dúvida perguntar a um dos dois.
rm -f saída.data
time mpirun -np $1 --hostfile hostfile ./main $2

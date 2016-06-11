#Executar segundo bash configure.sh
#Este Shell com a ajuda estimável do técnico de laborario: Jean Barbosa Siqueira e do Colega Acadêmico: Alvaro Leonel Socorro Queiroz Mariano
#Qualquer dúvida perguntar a um dos dois.
echo "Compilando..."
make

echo "Liberando a permissao do arquivo..."
chmod +x main

count=0

while read ip; do
  if test $count != 0
   then
    echo "Sincrozinando o arquivo no ip: $ip..."
    rsync -apq --rsync-path="mkdir -p `pwd` && rsync" main $ip:`pwd`
  fi
  count=$count+1
  
done < hostfile

echo "Lamboot..."
lamboot hostfile

echo "Configuracao Realizado com Sucesso!"

#Executar primeiro bash ssh_configure.sh
#Este Shell com a ajuda estimável do técnico de laborario: Jean Barbosa Siqueira e do Colega Acadêmico: Alvaro Leonel Socorro Queiroz Mariano
#Qualquer dúvida perguntar a um dos dois.
echo "Liberando o acesso do usuario..."
if ! test -f ~/.ssh/id_rsa.pub && ! test -f ~/.ssh/id_rsa
	then
		ssh-keygen -N "" -q -f ~/.ssh/id_rsa
		ssh-add
fi

if ! test -f ~/.ssh/authorized_keys
	then
		cat ~/.ssh/id_rsa.pub > ~/.ssh/authorized_keys
fi

count=0

while read ip; do
	if test $count != 0
		then
			rsync -aq ~/.ssh/authorized_keys $ip:~/.ssh/
fi
count=$count+1

done < hostfile
echo "...Acesso liberado com SUCESSO!"

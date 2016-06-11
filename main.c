/*
Descrição: Algoritmo paralelo para busca de um elemento no vetor desordenado.
Autor: Felipe da Rocha Moralles Guterres
RGM: 023401
Curso: Ciência da  Computação - UEMS
*/
/*bibliotecas*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

/*Prototipos*/
/*função utilizada para realizar a busca do elemento*/
void search(int *vector, int end, int start, int** vector_result_proc, int* qtd_find, int target);
/*função utilizada para contar o numero de elementos do arquivo*/
int cont_numbers(FILE *arq);
/*função utilizada para fazer a quebra dos vetores para outras máquinas*/
void break_vector(int* vector, int start, int end, int** vector_per_proc);

int main(int argc, char** argv)
{
	FILE *arq = NULL;/*arquivo do conjunto de entrada*/
	FILE *save = NULL;/*arquivo para salvar os índices*/
	int qtd_numbers = 0;/*número total de elementos do arquivo de entrada*/
	int *vector = NULL;/*vetor principal que irá conter todos os elementos do arquivo*/

	int i = 0, j =0;/*variaveis para controle*/

	int target = atoi(argv[1]);/*recebe o numero a ser buscado pela linha de comando do terminal*/
	int size, rank = 0;/*size  é o número de processo rodando; rank é qual processo está rodando*/
	int qtd_proc_first; /*Quantidade de numeros de processos para maquina 0*/
	int qtd_proc_other; /*Quantidade de numeros de processos para demais máquinas*/
	int* vector_aux = NULL;/*Vetor que irá receber as partes do vetor principal, para percorrer em cada processo*/
	int* vector_result_proc = NULL;/*vetor que retornara em qual posição o elemento a ser buscado se encontra para cada processo*/
	int qtd_find = 0;/*Quantidade total de elementos encontrada*/
 	int tag = 10; /*canal utilizado*/

	MPI_Init(&argc, &argv);/*Inicialização da MPI*/
	MPI_Status status;

	MPI_Comm_size(MPI_COMM_WORLD, &size);/*quantidade de máquinas executando para o programa*/
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);/*defini que processo será executado*/

	
	if(rank == 0)/*Atribuições para máquina mestre*/
	{
		printf("Iniciando...\n");
		printf("\n-------------Meu rank eh: %d\n", rank);
		qtd_numbers = cont_numbers(arq);/*quantidade total de elementos*/
		
		arq = fopen("dados.dat", "r");/*abre o arquivo*/
		vector =(int *) malloc(sizeof(int) * qtd_numbers);/*aloca a quantidade deespaço total de elementos*/

		while(!feof(arq))/*percorre o arquivo*/
		{
			 fscanf(arq, "%d\n", &vector[i]);/*faz a leitura do elementos para o vetor*/
			i++;
		}
		fclose(arq);/*encerra o arquivo*/

	 	save = fopen("saída.dat","w");/*abre arquivo de saída*/

		qtd_proc_first = qtd_numbers / size + qtd_numbers % size;/*estabelece quantidade de elementos para máquina mestre*/
		qtd_proc_other = qtd_numbers / size;/*estabelece a quantidade de elementos para as máquinas de processamento*/
		printf("para o mestre %d para outras %d\n", qtd_proc_first, qtd_proc_other);
		
		for(i = 1; i < size; i++)/*Para o processemando das máquinas escravas caso o numero de size for maior igual 2 */
		{
			/*para cada máquina escrava terá um vetor com parte que irá buscar*/
			vector_aux = (int *) malloc(sizeof(int) * qtd_proc_other);
			/*faz a quebra das posições do vetor principal, para as máquina adjacentes*/
			break_vector(vector, qtd_proc_first + ((i - 1) * qtd_proc_other), qtd_proc_other, &vector_aux);
			/*envia para as máquinas adjacentes a quantidade de elementos que as máquinas adjacentes irão possuir*/
			MPI_Send(&qtd_proc_other, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
			/*envia para as máquinas adjacentes a quantidade de elementos que a máquina mestre terá*/
			MPI_Send(&qtd_proc_first, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
			/*enviar o vetor auxiliar para cada máquina adjcente*/
			MPI_Send(vector_aux, qtd_proc_other, MPI_INT, i, tag, MPI_COMM_WORLD);
			free(vector_aux);
		}
		/*esse vetor auxiliar será para máquina mestre*/
		vector_aux = (int *) malloc(sizeof(int) * qtd_proc_first);
		/*faz a quebra do vetor principaL, podendo ter quantidade a mais  de numeros, caso  a quantidade total de elementos não possa ser distribuido igualmente*/
		break_vector(vector, 0, qtd_proc_first, &vector_aux);
		/*faz a busca do elemento para máquina mestre*/
		search(vector_aux, 0, qtd_proc_first, &vector_result_proc, &qtd_find, target);
		/*salva no arquivo de saída.txt, a posição dos elementos encontrados na máquina principal*/
		for(j = 0; j < qtd_find; j++)
		{
			fprintf(save, "%d\n", vector_result_proc[j]);
		}
		printf("Founds in rank %d; %d\n", rank, qtd_find);
		/*percorre as máquinas escravas*/
		for(i = 1; i < size; i++){
			/*desaloca o vetor auxiliar utilizado nos outros processos*/
			free(vector_result_proc);
			/*retorna a quantidade total no vetor auxiliar enviado*/
			MPI_Recv(&qtd_find, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
			/*Aloca para o vetor resultante a quantidade de elementos encontrada no vetor auxiliar enviado para as máquinas escrevas*/
			vector_result_proc = (int*) malloc(sizeof(int) * qtd_find);
			/*retorna um vetor com as posições, onde o elemento se encontra*/
			MPI_Recv(vector_result_proc, qtd_find, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
			/*salva no arquivo de saída.txt, as posições*/
			for(j = 0; j < qtd_find; j++)
			{
				fprintf(save, "%d\n", vector_result_proc[j]);
			}
			printf("Founds in rank %d; %d\n", i, qtd_find);
		}
		fclose(save);
	}
	else
	{
		printf("\n-------------Meu rank eh: %d\n", rank);
		/*Recebi a quantidade de elementos para o vetor auxiliar*/
		MPI_Recv(&qtd_proc_other, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
		/*aloca a quantidade de elementos para a quantidade para as máquinas escravas*/
		vector_aux = (int *) malloc(sizeof(int) * qtd_proc_other);
		/*recebe a quantidade da primeira máquina, para realizar calculo de quebra*/
		MPI_Recv(&qtd_proc_first, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
		/*recebe o vetor auxiliar da máquina mestre*/
		MPI_Recv(vector_aux, qtd_proc_other, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
		/*faz busca do elemento para as máquinas auxiliar*/
		search(vector_aux, qtd_proc_first + (rank -1) * qtd_proc_other, qtd_proc_other, &vector_result_proc, &qtd_find, target);
		/*envia para máquina principal a quantidade de elementos encontrado no vetor auxiliar*/
		MPI_Send(&qtd_find, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
		/*envia para máquina principal o vetor com as posições encontradas*/
		MPI_Send(vector_result_proc, qtd_find, MPI_INT, 0, tag, MPI_COMM_WORLD);
	}
	MPI_Finalize();/*finaliza a MPI*/
	if(vector) free(vector);/*desaloca o vetor principal*/
	if(vector_aux) free(vector_aux);/*desaloca o vetor auxiliar*/
	if(vector_result_proc) free(vector_result_proc);/*desaloca o vetor com as posições encontradas*/
	printf("\n...Encerrado! %d\n", rank);
	return 0;
}
/*(vetor para busca, posição de partida, quantidade de elementos, vetor com os indices, quantidade encrontra no vetor, número a ser buscado)*/
void search(int *vector, int start, int end, int** vector_result_proc, int* qtd_find, int target)
{
	int pos;
	for(pos = 0 ; pos < end ; pos++)
	{
		/*compara elemento a ser buscado com posição do vetor*/
		if(vector[pos] == target)
		{	
			(*qtd_find)++;/*conta a quantidade encontrada*/
			/*aloca uma nova posição cada vez que elemento for encontrado*/
			*vector_result_proc = (int *) realloc(*(vector_result_proc), (*qtd_find) * sizeof(int));
			/*insere no vetor resultante, a posição encontrada mais a posição onde o vetor for quebrado */
			(*vector_result_proc)[(*qtd_find) - 1] = pos + 1 + start;
		}
	}
}
int cont_numbers(FILE *arq)/*função utilizada para contar o numero de elementos do arquivo*/
{
	int cont = 0;/*contador*/
	arq = fopen("dados.dat", "r");/*abertura do arquivo*/
	if(arq == NULL)/*erro do arquivo*/
	{
		printf("Erro de leitura de arquivo!\n");
		exit(1);
	}
	
	if(getc(arq) == EOF)/*verifica se o arquivo possue algum numero*/
	{
		printf("Arquivo vazio\n");
		exit(1);
	}
	while(!feof(arq))/*percorre o arquivo analisando as quebras de linhas, para contar o númeri de elementos presente no arquivo*/
		if(getc(arq) == '\n')
			cont++;
	printf("Total de numeros: %d\n", cont);/*quantidade total*/
	return(cont);
	fclose(arq);/*fecha o arquivo*/
}
/*realiza a quebra do vetor principal para o numero de processos*/
/*(vetor principal, valor de partida do vetor principal, quantidade de elementos, vetor auxiliar)*/
void break_vector(int* vector, int start, int end, int** vector_per_proc)
{
	int i, j = 0;
	/*utiliza um cálculo para determinar a posição inicial dao vetor principal*/
	for(i = start; i < start + end ; i++)
	{	
		(*vector_per_proc)[j] = vector[i];/*faz a cópia do vetor principal para o vetor auxiliar*/
		j++;
	}
}

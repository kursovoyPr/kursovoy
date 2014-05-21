#include <stdio.h>
#include <stdlib.h>	
#include <time.h>
#include <string.h>	
#include <stdbool.h>

#define P 40 			//вероятность точечной комбинации
#define Q 50 			//вероятность переброса в поэлементном
#define MUTATION 4 		//вероятность мутации

#define HEAD_SIZE 10	//H
#define MAX_ARN 4 		//MaxArn
#define POPULATION_SIZE 51

#define MAX_COUNT 50


int size; //длина хромосомы
int generation = 0;
double max_fitness = 0;
double average_fitness = 0;
double sum_fitness = 0;


void init(int** best, double* fitness, int*** f, int*** f_n);
void build_person(int** f);											
void write_person(int** f);											
void generate_battle(int x1, int y1, int x2, int y2);
double results_scan();												
int find_sum_and_max_fitness(double* fitness, int*** f);			
void selection(int elite, double* fintess, int*** f, int*** f_n);	
void crossover(int elite, int*** f_n);								
void crossover_small(int* one, int* duo);							
void mutation(int*** f_n);								
void update(int***f, int***f_n);									
void clear_it(double* fitness, int*** f, int*** f_n);				

int main()
{
	time_t startProgTime, endProgTime;
	time_t start_test_time, end_test_time, acc_test_time;

	startProgTime = time(NULL);
	acc_test_time = 0;

	int i;
	double* fitness;
	int** best;
	int*** f;
	int*** f_n;
	int best_num;
	
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	size = HEAD_SIZE * MAX_ARN + 1;
	
	best = (int **)malloc(sizeof(int*) * 4);
	fitness = (double *)malloc(sizeof(double) * POPULATION_SIZE);
	f = (int ***)malloc(sizeof(int**) * POPULATION_SIZE);
	f_n = (int ***)malloc(sizeof(int**) * POPULATION_SIZE);
	init(best, fitness, f, f_n);

	for(i = 0; i < POPULATION_SIZE; i++)
	{
		
		build_person(f[i]);
		write_person(f[i]);

		generate_battle(x2, y2, x1, y1);
		
		start_test_time = time(NULL);
		system("StartBattle.bat"); //запустить просчет
		end_test_time = time(NULL);
		acc_test_time += (end_test_time - start_test_time);
		
		fitness[i] = results_scan();
	}

	best_num = find_sum_and_max_fitness(fitness, f);
	generation++;
	
	while(generation < MAX_COUNT)
	{
	
		selection(best_num, fitness,f,f_n);
		crossover(best_num, f_n);
		mutation(f_n);
		update(f, f_n);
		
	
		for(i = 0; i < POPULATION_SIZE; i++)
		{			
			write_person(f[i]);		
			generate_battle(x2, y2, x1, y1);	
			start_test_time = time(NULL);
			system("StartBattle.bat");
			end_test_time = time(NULL);
			acc_test_time += (end_test_time - start_test_time);		
			fitness[i] = results_scan();		
		}

		best_num = find_sum_and_max_fitness(fitness, f);
		generation ++;
	}

	endProgTime = time(NULL);
	printf("Program launch time: %d seconds\n", endProgTime - startProgTime);
	printf("Program test time: %d seconds\n", acc_test_time);
	clear_it(fitness, f, f_n);
	return 1;
}

void selection(int elite, double* fitness, int*** f, int*** f_n)
{
	int i, j, k;
	double random, tmp;


	for(i = 0; i < POPULATION_SIZE; i++)
	{
		if(i != elite)
		{
			random = (rand() / (double)RAND_MAX) * 100;
			
			for(k = 0, tmp = 0; tmp < random;)
				tmp += fitness[k++];
	   			
			for(j = 0; j < size; j++){
				f_n[i][0][j] = f[k - 1][0][j];
				f_n[i][1][j] = f[k - 1][1][j];
				f_n[i][2][j] = f[k - 1][2][j];
				f_n[i][3][j] = f[k - 1][3][j];
			}
		}
		else
		{
			// Лучший переносится в другое поколение
			for(j = 0; j < size; j++)
			{
				f_n[i][0][j] = f[i][0][j];
				f_n[i][1][j] = f[i][1][j];
				f_n[i][2][j] = f[i][2][j];
				f_n[i][3][j] = f[i][3][j];
			}
		}
	}
}


void crossover(int elite, int*** f_n){

	int i, j, k;
	
	i = 0;
	while(POPULATION_SIZE - i > 1)
	{	
		if(i == elite)
		 i++;
		
		k = i + 1;

		if(k == elite)
			k++;

		for (j = 0; j < 4; j++)
			crossover_small(f_n[i][j],f_n[k][j]);
		i+=2;
	}
}

void crossover_small(int* one, int* duo)
{
	int j;
	double random;
	int num, num2;
	int tmp;
	
	random = (rand() / (double)RAND_MAX)*100;
	if(random < P)
	{
		num = rand() % size;
		num2 = rand() % size;
		
		if(num2 > num)
		{
			for (j = num; j <= num2; j++)
			{
				tmp = one[j];
				one[j] = duo[j];
				duo[j] = tmp; 
			}		
		}
		else if (num2 < num)
		{
			for(j = 0; j <= num2; j++)
			{
				tmp = one[j];
				one[j] = duo[j];
				duo[j] = tmp;
			}	
			for (j = num; j < size; j++)
			{
				tmp = one[j];
				one[j] = duo[j];
				duo[j] = tmp;
			}			
		}		
	}
	else
	{		
		for(j = 0; j < size; j++)
		{
			if(rand() % 100 < Q) 
			{
				tmp = one[j];
				one[j] = duo[j];
				duo[j] = tmp;
			}
		}	
	}		
}



void mutation(int*** f_n)
{
	int i, j, k;
	for(i = 0; i < POPULATION_SIZE; i++)
	{
			for(j = 0; j < 4; j++)
			{
				for(k = 0; k < size; k++)
				{
					if((rand() / (double)RAND_MAX) * 100 < MUTATION)
						f_n[i][j][k] = (k < HEAD_SIZE) ? (rand() % 24) : (8 + rand() % 16);
				}	

			}
	}
}


double results_scan()
{
	FILE *results;
	int my_score, enemy_score, my_win, win, enemy_win, my_score2, enemy_score2, tmp, tmp2;
	char* name = (char*)malloc(40);
	results = fopen ("battleResults.txt","r");
	
	fscanf(results, "Results for %d rounds\n", &tmp);
	fscanf(results, "Robot Name	    Total Score    	Survival	Surv Bonus	Bullet Dmg	Bullet Bonus	Ram Dmg * 2	Ram Bonus	 1sts 	 2nds 	 3rds\n");
	
	fscanf(results,"%d%c%c: %s\t%d (%d%%)\t\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t",&tmp, &name[0], &name[1], name, &tmp2, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &win, &tmp, &tmp);
	if (!strncmp(name, "sample.GeneticRobot", 11))
	{
		my_score = tmp2;
		my_win=win;
	}
	else
		enemy_score = tmp2;
	
	fscanf(results,"%d%c%c: %s\t%d (%d%%)\t\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t",&tmp, &name[0], &name[1], name, &tmp2, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &win, &tmp, &tmp);
	if (!strncmp(name, "sample.GeneticRobot", 11))
	{
		my_score = tmp2; my_win=win;
	}
	else
		enemy_score = tmp2;
	
	fclose(results);
	
	free(name);
	
	return 100 * (double)my_score / (my_score + enemy_score);
}

int find_sum_and_max_fitness(double* fitness, int*** f){
	int i, j;
	int index = -1;
	FILE *evolution;
	FILE *bests;
	
	max_fitness = 0;
	sum_fitness = 0;
	
	for(i = 0; i < POPULATION_SIZE; i++)
	{
		sum_fitness += fitness[i];
		if(fitness[i] >= max_fitness)
		{
			max_fitness = fitness[i];
			index = i;
		}
	}
	
	average_fitness = sum_fitness / POPULATION_SIZE;
	
	evolution = fopen ("evolution.txt","a");
	bests = fopen ("bests.txt","a");
	
	printf(        "sum: %lf, average: %lf, max[%d]: %lf\n", sum_fitness, average_fitness, index, max_fitness);
	fprintf(evolution, "%d %.3lf %.3lf\n", generation, average_fitness, max_fitness);
	
	if(index != -1)
	{
		fprintf(bests, "%d %.3lf %.3lf (%d):\n", generation, average_fitness, max_fitness, index);
		for(i = 0; i < 4; i++)
		{
			for(j = 0; j < size; j++)
				fprintf(bests, "%d ", f[index][i][j]);
			
			fprintf(bests, "\n");
		}
		fprintf(bests, "================================================================\n");
	}
	
	fclose(evolution);
	fclose(bests);
	
	//нормализация
	for(i = 0; i < POPULATION_SIZE; i++)
	{
		fitness[i] *= 100;
		fitness[i] /= sum_fitness;
	}
	
	return index;
}


//генерируем особь для нулевого поколения
void build_person(int** f)
{
	int i, j;
	
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < HEAD_SIZE; j++)
			f[i][j] = rand() % 24;
		
		for(; j < size; j++)
			f[i][j] = 8 + rand() % 16;
		
	}
}


//записываем особь f в file для последующей интерпретации в танке
void write_person(int** f){
	FILE *genome;
	int i, j;
	
	genome = fopen ("robots/sample/GeneticRobot.data/chromosome.dat","w");
	
	for(i = 0; i < 4; i++){
		for(j = 0; j < size; j++){
			fprintf(genome, "%d ", f[i][j]);
		}
		fprintf(genome, "\n");
	}
	fclose(genome);
}


void update(int*** f, int*** f_n){
	int i, j, k;
	for(i = 0; i < POPULATION_SIZE; i++)
		for(j = 0; j < 4; j++)
			for(k = 0; k < size; k++)
				f[i][j][k] = f_n[i][j][k];
			
}

//создаем file my.battle, в который записываем параметры боя
void generate_battle(int x1, int y1, int x2, int y2){
	FILE *battle;
	
	battle = fopen ("battles/new.battle","w");
	
	fprintf(battle, "robocode.battleField.width=800\n");
	fprintf(battle, "robocode.battleField.height=600\n");
	
	fprintf(battle, "robocode.battle.numRounds=20\n");
	
	fprintf(battle, "robocode.battle.gunCoolingRate=0.1\n");
	fprintf(battle, "robocode.battle.rules.inactivityTime=450\n");
	// Change enemy jere
	fprintf(battle, "robocode.battle.selectedRobots=sample.GeneticRobot,sample.Crazy\n");
	
	fclose(battle);
}


void init(int** best, double* fitness, int***f, int***f_n){
	int i, j;
	srand(time(NULL));

	for(i = 0; i < 4; i++){
		best[i] = (int *)malloc(sizeof(int)*size);
	}
	
	for(i = 0; i < POPULATION_SIZE; i++){
		f[i] = (int **)malloc(sizeof(int*)*4);
		f_n[i] = (int **)malloc(sizeof(int*)*4);
		for(j = 0; j < 4; j++){
			f[i][j] = (int *)malloc(sizeof(int)*size);
			f_n[i][j] = (int *)malloc(sizeof(int)*size);
		}
	}
}

void clear_it(double* fitness, int***f, int***f_n){
	int i, j;
	
	for(i = 0; i < POPULATION_SIZE; i++){
		for(j = 0; j < 4; j++){
			free(f[i][j]);
			free(f_n[i][j]);
		}
		free(f[i]);
		free(f_n[i]);
	}
	
	free(fitness);
	free(f);
	free(f_n);
}

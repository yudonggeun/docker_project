/*
 * made by yudonggeun 2017154022
 * 
 * health check ping
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "../include/health.h"
#include <string.h>

pthread_mutex_t ping_mutex;
pthread_mutex_t ping_cond;


void restartContainer(Container* con);
void parsingContainerAddress(Container* con);
void startPing(void*);

void restartContainer(Container* con){
	char command[150];
	
	fprintf(stdout, "docker restart %s : \n", con->name);
	sprintf(command, "docker restart %s\n", con->name);
	
	system(command);
	parsingContainerAddress(con);
}

void parsingContainerAddress(Container* con){
	char command[100] = {0,};
	FILE* fp;
	
	sprintf(command, "docker inspect -f '{{ .NetworkSettings.Networks.%s.IPAddress}}' %s", 
			con->bridge/*bridge*/, con->name/*container name*/);

	fp = popen(command, "r");

	if(fp == NULL){
		fprintf(stdout, "parsing Container addr failed\n");
		return ;
	}
	
	while( fgets( con->addr, 16, fp) );
	
	if( con->addr[strlen(con->addr)-1] == '\n')
		con->addr[strlen(con->addr)-1] = 0;	
	
	pclose(fp);
	
	printf("%s IP : %s\n", con->name, con->addr);


	return;
}

void startPing(void* info){

	pthread_t pid;

	while(1){
		pthread_create(&pid, NULL, ping, info);
		pthread_join(pid, NULL);
	}

}

int main(){
	
	get_container_info_from_json();

	pthread_t* pid = malloc(sizeof(pthread_t) * conCount);
	
	for(int i = 0; i< conCount ; i++){
		parsingContainerAddress(conSet+i);
		if( strlen((conSet+i)->addr) < 4 ){
			restartContainer(conSet+i);
		}
	}
	
	while(1){
		for(int i = 0; i< conCount; i++){
			pthread_create(pid+i, NULL, ping, (void*)(conSet+i));
			pthread_join(pid[i], NULL);
		}
	}
}
/* yudongeun end */

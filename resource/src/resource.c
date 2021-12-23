/*
 * made by yudonggeun 2017154022
 * 
 * resource header
 *
 */

#include "../include/extract.h"

#define STANDARD_UP 70  //%
#define STANDARD_DONW 40 //%
#define REQUEST_CYCLE 5 //sec
float cpu_use_sum;//modify here

char* command_header = "sudo docker update --cpus=";
int container_count;


void updateCpuMAX(Resource* con){
	
	/* STANDARD over */
	if( (con->max_cpu_usage * STANDARD_UP /100) < con->cpu_usage  
		&& cpu_use_sum != max_cpu_usage){
		
		printf("cpu up[%s]", con->name);
		
		float upSize;
		if(con->max_cpu_usage >= (max_cpu_usage - cpu_use_sum) ){
			upSize = (max_cpu_usage - cpu_use_sum);
		}
		else {
			upSize = con->max_cpu_usage;
		}
		
		con->max_cpu_usage += upSize;
		cpu_use_sum += upSize;
	}
	
	/* STANDARD down */
	else if( (con->max_cpu_usage * STANDARD_DONW /100) > con->cpu_usage 
			&& con->max_cpu_usage > 0.1f){
				
		printf("cpu down[%s]", con->name);
		con->max_cpu_usage -= con->max_cpu_usage /2;
		cpu_use_sum -= con->max_cpu_usage /2;
		
		if(con->max_cpu_usage < 0.1f)
			con->max_cpu_usage = 0.1f;
	}
	
	else{		
	fprintf(stdout, "None[%s] : limit cpu : %.2f, use cpu : %.2f\n", con->name, con->max_cpu_usage, con->cpu_usage);
	
		return;
		}
	
	char command[200];
	sprintf(command, "%s%.1f %s", command_header, con->max_cpu_usage, con->name);
	
	fprintf(stdout, "command : %s\n", command);
	fprintf(stdout, " : limit cpu : %.2f, use cpu : %.2f\n", con->max_cpu_usage, con->cpu_usage);
	if(con->name == NULL)
		return;
	system(command);
	
}

void main(int argc, char** argv){
	
	if(argc < 4){
		fprintf(stdout, "usage : ./promgram [monitoring container name] [bridge] [container count]\n\n");
		
		return;
	}
	
	monitoring_name = argv[1];
	monitoring_birdge = argv[2];
	
	container_count = atoi(argv[3]);
	
	info = malloc(sizeof(Resource) * container_count);// modify please
	
	for(int i = 0; i< container_count; i++){
		info[i].max_cpu_usage = 0.1f; /* default cpu usage */
	}
	
	printf("start resouce manager\n");
	
	while(1){
	
		/* extract container resource by prometheus */
		printf("extract data\n");
		
		extract();
		
		for(int i = 0; i< container_count; i++){
			updateCpuMAX(info+i);
		}
		sleep(REQUEST_CYCLE);
	}
	
}

/* yudonggeun end */

/*
 * made by yudonggeun 2017154022
 * 
 * start
 *
 */

#include "../include/extract.h"

static char usage_query[200];// = "curl -G \"http:\/\/10.0.42.131:9090/api/v1/query\" --data-urlencode \"query=rate(container_cpu_usage_seconds_total{id=~\\\"/docker/.*\\\"}[1m])\"";

static char machine_cpu_cores_query[200];// = "curl \"http://10.0.42.131:9090/api/v1/query?query=sum(machine_cpu_cores)\"";

char* json;
char* json_machine;

char* monitoring_name;
char* monitoring_birdge;
char* monitoring_ip;

float max_cpu_usage;

Resource* info = NULL;

static void setUsageQL(){
	char* pront = "curl -G \"http://";
	char* back = ":9090/api/v1/query\" --data-urlencode \"query=rate(container_cpu_usage_seconds_total{id=~\\\"/docker/.*\\\"}[1m])\"";
	
	sprintf(usage_query, "%s%s%s", pront, monitoring_ip, back);
}

static void setMachineCpuQL(){
	char* pront = "curl \"http://";
	char* back = ":9090/api/v1/query?query=sum(machine_cpu_cores)\"";
	
	sprintf(machine_cpu_cores_query, "%s%s%s", pront, monitoring_ip, back);
}

char* parsingContainerAddress(char* name, char* bridge ){
	char command[150] = {0,};
	FILE* fp;
	
	char * ip = malloc(sizeof(char) * 16);
	
	sprintf(command, "docker inspect -f '{{.NetworkSettings.Networks.%s.IPAddress}}' %s", 
			bridge/*bridge*/, name/*container name*/);
	
	fp = popen(command, "r");

	if(fp == NULL){
		fprintf(stdout, "parsing Container addr failed\n");
		return NULL;
	}
	
	while( fread( ip, 1, 16, fp) );
	
	if( ip[strlen(ip)-1] == '\n')
		ip[strlen(ip)-1] = 0;	
	
	pclose(fp);
	
	return ip;
}

char* parsingJson(char* query){
	
	FILE* fp;
	
	int size = 0;
	
	char* file;
	
	int tem;
	/* system curl query to prometheus 
	 * json response open */

	file = malloc(sizeof(char) * 1000);

	fp = popen(query, "r");
	
	if(fp == NULL){
		fprintf(stdout, "monitoring data open failed\n");
		return;
	}
	
	while( (tem = fread(file+size, 1, 1000, fp)) >= 1000 ){
		
		size += tem;
		file = realloc(file, sizeof(char) * (size + 1000));
	}
	
	size+= tem;
	
	return file;
}

void extract_cpu_core_count(){
		
	json_object * root, *status, *data;

	if( (root = json_tokener_parse(json_machine) ) == NULL) /* json type data read */{
		fprintf(stdout, "json query failed\n");
		return;
	}

	status = json_object_object_get(root, "status");
	
	if( strcmp(json_object_get_string(status), "success") != 0){
		fprintf(stdout, "json query failed\n");
		return;
	}

	data = json_object_object_get(root, "data");
	
	json_object *target, *metric, *value, *result;

	result = json_object_object_get(data, "result");

	int dataSize = json_object_array_length(result);

	for(int i = 0; i < dataSize; i++){
		/* get target object from data */
		target = json_object_array_get_idx(result, i);
		/* target extract metric info and value */
		metric = json_object_object_get(target, "metric");
		value  = json_object_object_get(target, "value");
		
		/* value information */
		json_object * j_core_count;
		char* core_count_string;
		int core_count;
		
		j_core_count = json_object_array_get_idx(value, 1);
		core_count_string = json_object_get_string(j_core_count);
		core_count = atoi(core_count_string);
		max_cpu_usage = core_count;
		
		//fprintf(stdout, "core count[ %d ]\n", core_count);
		
		}
	
	free(json_machine);
	
}

void extract_cpu_usage(){
	json_object * root, *status, *data;

	if( (root = json_tokener_parse(json) ) == NULL) /* json type data read */{
		fprintf(stdout, "json query failed\n");
		return;
	}

	status = json_object_object_get(root, "status");
	
	if( strcmp(json_object_get_string(status), "success") != 0){
		fprintf(stdout, "json query failed\n");
		return;
	}

	data = json_object_object_get(root, "data");
	
	json_object *target, *metric, *value, *result;

	result = json_object_object_get(data, "result");

	int dataSize = json_object_array_length(result);
	
	/* Resource malloc */
	
	//fprintf("data size %d\n", dataSize);
	
	for(int i = 0; i < dataSize; i++){
		/* get target object from data */
		target = json_object_array_get_idx(result, i);
		/* target extract metric info and value */
		metric = json_object_object_get(target, "metric");
		value  = json_object_object_get(target, "value");

		/* metric infromation */
		json_object *j_name;

		j_name = json_object_object_get(metric, "name");

		info[i].name = json_object_get_string(j_name);
		
		//fprintf(stdout, "metric data : name[ %s ]\n", info[i].name);
		
		/* value information */
		json_object *j_cpu_persentage;
		char* cpu_persentage_string;
		float cpu_persentage;

		j_cpu_persentage = json_object_array_get_idx(value, 1);
		cpu_persentage_string = json_object_get_string(j_cpu_persentage);
		info[i].cpu_usage = atof(cpu_persentage_string);
		cpu_use_sum += info[i].max_cpu_usage;
		
		//fprintf(stdout, "value : cpu [ %f ]\n\n", info[i].cpu_usage);


		
	}
	free(json);
}

void extract(){
	
	monitoring_ip = parsingContainerAddress(monitoring_name, monitoring_birdge);
	
	setUsageQL();
	setMachineCpuQL();
	
	json_machine = parsingJson(machine_cpu_cores_query);
	json = parsingJson(usage_query);
	
	cpu_use_sum=0;
	
	extract_cpu_usage();
	extract_cpu_core_count();
	
}
/* yudonggeun end */

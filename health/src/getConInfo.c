/*
 * made by yudonggeun 2017154022
 * 
 * health check ping
 *
 */

#include "../include/health.h"
#include <stdio.h>
#include <json-c/json.h>

Container* conSet = NULL;

int conCount;

static char* jsonFile = "../json/health_container.json";

void get_container_info_from_json(){
	
	if(conSet != NULL){
		free(conSet);
		conSet = NULL;
	}

	json_object *root, *header, *containerSet;

	json_object *obj_count;

	json_object *container_set, *item, *name, *bridge;

	int arraySize;

	root = json_object_from_file(jsonFile);
	if(root == NULL){
		printf("error : %s\n", json_util_get_last_err());
	}
	header = json_object_object_get(root, "header");

	/* header parsing */
	/* count parsing */
	obj_count = json_object_object_get(header, "count");

	/* containerSet parsing */
	containerSet = json_object_object_get(root, "containerSet");

	if( json_object_get_type( containerSet ) == json_type_array ){
		
		arraySize = json_object_array_length(containerSet);
		conCount = arraySize;	
		/* container Set malloc */
		conSet = malloc(sizeof(Container) * arraySize);
		
		/* set container infomation info at Container Array */
		for(int i = 0; i< arraySize; i++){
			item = json_object_array_get_idx(containerSet, i);

			name = json_object_object_get(item, "name");

			bridge = json_object_object_get(item, "bridge");
			
			conSet[i].name = json_object_get_string(name);

			conSet[i].bridge = json_object_get_string(bridge);
			
			conSet[i].addr = malloc(sizeof(char) * 17);
		}
	}
}


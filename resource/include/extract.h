/*
 * made by yudonggeun 2017154022
 * 
 * resource header
 *
 */

#ifndef EXTRACT

#include <json-c/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char* name;
	float cpu_usage;
	float max_cpu_usage;
} Resource;

extern void extract();

extern void extract_container_count();

extern char* parsingJson(char* query);

extern char* monitoring_name;
extern char* monitoring_birdge;
extern char* monitoring_ip;

extern float max_cpu_usage;
extern float cpu_use_sum;
extern Resource *info;
extern int container_count;

#endif

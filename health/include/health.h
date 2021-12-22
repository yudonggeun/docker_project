/*
 * made by yudonggeun 2017154022
 * 
 * health check ping
 *
 */

#ifndef HEALTH
#define HEALTH

extern void ping(void*);

typedef struct {
	char* addr;
	char* name;
	char* bridge;
} Container;

extern void restartContainer(Container*);
extern void parsingContainerAddress(Container*);
extern void get_container_info_from_json();

extern Container* conSet;
extern int conCount;
//extern pthread_mutex_t ping_mutex;

#endif

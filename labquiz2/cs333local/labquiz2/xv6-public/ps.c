#include "types.h"
#include "stat.h"
#include "user.h"

// enum to store process state by name
enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// a character array to print correspding state names
char *psstate[6] = {"Unused  ", "Embryo  ", "Sleeping ", "Runnable", "Running ", "Zombie  "};

// the format for each process's ps entry
struct ps_entry
{
  int pid;
  int ppid;
  uint sz;
  enum procstate state;
  char name[16];
};



int
main(int argc, char *argv[])
{
	int ret;

	// make space for ps_entrys first, assume a maximum of 64 processes
	// ...
	struct ps_entry ps[64];
	// intia(ps);
	ret = getps(ps);
	// make the system call to get all process information

    // print process information in required format
    printf(1,"%s\t%s\t%s\t%s\t\t%s\n","PID" ,"PPID" ,"Size" , "State", "Name");
    if (ret == 1) {
	    for (int i=0;i<64;i++) {
	    	if (ps[i].pid != 0) {
	    		if (ps[i].ppid == -135756403) ps[i].ppid = 0;
	    		printf(1, "%d\t%d\t%d\t%s\t%s\t\n", ps[i].pid, ps[i].ppid, ps[i].sz,psstate[ps[i].state], ps[i].name);
	    	}
			// printf(1,"%d\n", ret);
		}
	}
    // print per process information

	exit();
}

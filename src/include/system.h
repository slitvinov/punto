#include <string.h>
#include <linux/kernel.h>
#include <sys/sysinfo.h>

void duerme(unsigned long tiempo);
void duerme_old(unsigned long tiempo);
void print_sysinfo(void);

void sigfpe_handler(int sig);
void sigint_handler(int sig);
void sigterm_handler(int sig);
void sigquit_handler(int sig);
void TakeEnv(char **envp, char *string, char *var);

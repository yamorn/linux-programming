#include "netcomplex.h"


void Sigemptyset(sigset_t *set){
    if(sigemptyset(set) == -1)
	err_sys("sigemptyset error");
}

void Sigfillset(sigset_t *set){
    if(sigfillset(set) == -1)
	err_sys("sigfillset error");
}

int Sigismember(const sigset_t *set, int signo){
    int n;
    if((n = sigismember(set, signo)) == -1)
	err_sys("sigismember error");
    return(n);
}

void Sigaddset(sigset_t *set, int signo){
    if(sigaddset(set, signo) == -1)
	err_sys("sigaddset error");
}

void Sigdelset(sigset_t *set, int signo){
    if(sigdelset(set, signo) == -1)
	err_sys("sigdelset error");
}

void Sigpending(sigset_t *set){
    if(sigpending(set) == -1)
	err_sys("sigpending error");
}

void Sigprocmask(int how, const sigset_t *set, sigset_t *oset){
    if(sigprocmask(how, set ,oset) == -1)
	err_sys("sigpromask error");
}

Sigfunc *signal(int signo, Sigfunc *func){
    struct sigaction act, oact;
    
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if(signo == SIGALRM){
	#ifdef SA_INTERRUPT
	act.sa_flags |= SA_INTERRUPT; /* SunOS 4.x */
	#endif
    }else{
	#ifdef SA_RESTART
	act.sa_flags |= SA_RESTART; /* SVR4, 4.4BSD */
	#endif
    }
    if(sigaction(signo, &act, &oact) < 0)
	return(SIG_ERR);
    return(oact.sa_handler);
}

Sigfunc *Signal(int signo, Sigfunc *func){
    Sigfunc *sigfunc;

    if((sigfunc = signal(signo, func)) == SIG_ERR)
	err_sys("signal error");
    return(sigfunc);
}




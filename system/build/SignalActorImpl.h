#ifndef SIGNALACTORIMPL_H
#define SIGNALACTORIMPL_H

#include <QObject>

/*
1. SIGHUP: 연결된terminal이hangup하였을때(terminate)
2. SIGINT: interrupt key(^C)를입력하였을때(terminate)
3. SIGQUIT: quit key(^\)를입력하였을때(terminate+core)
4. SIGILL: illegal instruction을수행하였을때(terminate+core)
5. SIGTRAP: implementation defined hardware fault (terminate+core)
6. SIGABRT: abort시스템호출을불렀을때(terminate+core)
7. SIGBUS: implementation defined hardware fault (terminate+core)
8. SIGFPE: arithmetic exception, /0, floating-point overflow (terminate+core)
9. SIGKILL: process를kill하기위핚signal, catch 혹은ignore될수없는signal임(terminate)
10. SIGUSR1: user defined signal 1 (terminate)
11. SIGSEGV: invalid memory reference (terminate+core)
12. SIGUSR2: user defined signal 2 (terminate)
13. SIGPIPE: reader가terminate된pipe에write핚경우발생(terminate)
14. SIGALRM: alarm시스템호출후timer가expire된경우(terminate)
15. SIGTERM: kill시스템호출이보내는software termination signal (terminate)
16. SIGCHLD: child가stop or exit되었을때parent에게전달되는신호(ignore)
17. SIGCONT: continue a stopped process (continue/ignore)
18. SIGSTOP: sendable stop signal, cannot be caught or ignored (stop process)
19. SIGTSTP: stop key(^Z)를입력하였을때(stop process)
20. SIGTTIN: background process가control tty로부터read핛경우(stop process)
21. SIGTTOU: background process가control tty로write핛경우(stop process)
22. SIGURG: urgent condition on IO, socket의OOB data (ignore)
23. SIGXCPU: exceeded CPU time limit (terminate+core/ignore)
24. SIGXFSZ: exceeded file size limit (terminate+core/ignore)
25. SIGVTALRM: virtual time alarm, setitimer, (terminate)
26. SIGPROF: profiling time alarm, setitimer, (terminate)
27. SIGWINCH: terminal window size changed, (ignore)
28. SIGIO: 어떤fd에서 asynchronous I/O event가발생하였을경우(terminate/ignore)
29. SIGPWR: system power fail (terminate/ignore)
30. SIGSYS: bad argument to system call (terminate+core)
*/

class SignalActorImpl : public QObject 
{
    Q_OBJECT
public:
    explicit SignalActorImpl(QObject *parent = nullptr);

    // Unix signal handlers.
    static void signalActorUpdate(int sig);
    static void signalActorFinalize(int sig);

    virtual void run();
    virtual void on_finalize(int type);
    virtual void on_updated(int type);

    QString getSignalString(int nSignal);

signals:

public slots:
};

#endif // SIGNALACTORIMPL_H

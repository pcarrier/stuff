/**
 * sigomgbt - LD_PRELOAD library to print a backtrace on "OMG" signals
 *
 * Copyright (c) 2010, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
**/

#include <unistd.h>
#include <signal.h>
#include <execinfo.h>

#define TRACE_MAX_LEN 1024

static void segv_hdr(int sig) {
    void *trace[TRACE_MAX_LEN];
    struct sigaction act;
    int trace_size = backtrace(trace, TRACE_MAX_LEN);

    backtrace_symbols_fd(trace, trace_size, STDERR_FILENO);

    sigemptyset (&act.sa_mask);
    act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
    act.sa_handler = SIG_DFL;

    sigaction (sig, &act, NULL);
    kill(getpid(), sig);
}

__attribute__ ((constructor))
void init(void)
{
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND | SA_SIGINFO;
    act.sa_sigaction = segv_hdr;
    sigaction(SIGSEGV, &act, NULL);
    sigaction(SIGBUS, &act, NULL);
    sigaction(SIGFPE, &act, NULL);
    sigaction(SIGILL, &act, NULL);
}

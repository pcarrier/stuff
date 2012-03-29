/**
 * conntail
 *
 * tracks netfilter events
 * load nf_conntrack_netlink.ko first
**/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libnetfilter_conntrack/libnetfilter_conntrack.h>

#define CONNTAIL_MAX_LINE 1024
#define FAIL(name) {perror(#name); goto err;}

static int callback(enum nf_conntrack_msg_type type,
                    struct nf_conntrack *ct, void *data)
{
    char buffer[CONNTAIL_MAX_LINE];

    if (nfct_snprintf(buffer, sizeof(buffer), ct, type,
                      NFCT_O_XML, NFCT_OF_TIME) < 0)
        FAIL("nfct_snprintf");

    if (printf("%s\n", buffer) < 0)
        FAIL("printf");

    return NFCT_CB_CONTINUE;

  err:
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    struct nfct_handle *handle = NULL;

    handle = nfct_open(CONNTRACK, NFCT_ALL_CT_GROUPS);

    if (!handle)
        FAIL("nfct_open");

    if (nfct_callback_register(handle,
    	                       NFCT_T_NEW | NFCT_T_DESTROY,
                               callback, NULL) < 0)
        FAIL("nfct_callback_register");

    if (nfct_catch(handle) < 0)
        FAIL("nfct_catch");

    if (handle && nfct_close(handle) < 0) {
        FAIL("nfct_close");
    }

    return EXIT_SUCCESS;

  err:
    return EXIT_FAILURE;
}

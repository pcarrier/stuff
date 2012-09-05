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
#define FAIL(n) {perror(#n); goto err;}

static const enum nfct_filter_attr EXCLUDED_ATTRS[] = {
    NFCT_FILTER_L4PROTO,
    NFCT_FILTER_SRC_IPV4,
    NFCT_FILTER_SRC_IPV6,
    NFCT_FILTER_MAX
};

static int callback(enum nf_conntrack_msg_type type,
                    struct nf_conntrack *ct, void *data)
{
    char buffer[CONNTAIL_MAX_LINE];

    if (nfct_snprintf(buffer, sizeof(buffer), ct, type,
                      NFCT_O_XML, NFCT_OF_TIME) < 0)
        FAIL(nfct_snprintf);

    if (printf("%s\n", buffer) < 0)
        FAIL(printf);

    return NFCT_CB_CONTINUE;

  err:
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    struct nfct_handle *handle = NULL;
    struct nfct_filter *filter = NULL;
    static const enum nfct_filter_attr *filter_attr = EXCLUDED_ATTRS;

    struct nfct_filter_ipv4 lo_ipv4 = {
        .addr = ntohl(inet_addr("127.0.0.1")),
        .mask = 0xffffffff,
    };

    struct nfct_filter_ipv6 lo_ipv6 = {
        .addr = {0x0, 0x0, 0x0, 0x1},
        .mask = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff},
    };

    handle = nfct_open(CONNTRACK,
                       NF_NETLINK_CONNTRACK_NEW |
                       NF_NETLINK_CONNTRACK_DESTROY);
    if (!handle)
        FAIL(nfct_open);

    filter = nfct_filter_create();
    if (!filter)
        FAIL(nfct_filter_create);

    while (*filter_attr != NFCT_FILTER_MAX) {
        if (nfct_filter_set_logic(filter,
                                  *filter_attr,
                                  NFCT_FILTER_LOGIC_NEGATIVE) < 0)
            FAIL(nfct_filter_set_logic);
        filter_attr++;
    }

    nfct_filter_add_attr_u32(filter, NFCT_FILTER_L4PROTO, IPPROTO_ICMP);
    nfct_filter_add_attr_u32(filter, NFCT_FILTER_L4PROTO, IPPROTO_ICMPV6);
    nfct_filter_add_attr(filter, NFCT_FILTER_SRC_IPV4, &lo_ipv4);
    nfct_filter_add_attr(filter, NFCT_FILTER_SRC_IPV6, &lo_ipv6);

    if (nfct_filter_attach(nfct_fd(handle), filter) < 0) {
        FAIL(nfct_filter_attach);
    }

    if (nfct_callback_register(handle, NFCT_T_ALL, callback, NULL) < 0)
        FAIL(nfct_callback_register);

    if (nfct_catch(handle) < 0)
        FAIL(nfct_catch);

    if (handle && nfct_close(handle) < 0) {
        FAIL(nfct_close);
    }

    return EXIT_SUCCESS;

  err:
    return EXIT_FAILURE;
}

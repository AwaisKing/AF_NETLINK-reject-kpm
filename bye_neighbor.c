/*
KernelPatch KPM module documentation
https://github.com/LyraVoid/KernelPatch/blob/main/doc/en/module.md
*/

#include <hook.h>
#include <kallsyms.h>
#include <kpmodule.h>
#include <kputils.h>
// #include <uapi/asm-generic/errno.h>
#include <uapi/asm-generic/errno-base.h>

// module metadata
KPM_NAME("bye-neighbor");
KPM_VERSION("1.0.0");
KPM_LICENSE("GPL v2");
KPM_AUTHOR("AWAiS");
KPM_DESCRIPTION("Hides AF_NETLINK bullshit (detected by Duck Detector)");


// taken from https://github.com/torvalds/linux/blob/master/tools/include/uapi/linux/netlink.h#L44
struct nlmsghdr {
	__u32		nlmsg_len;	/* Length of message including header */
	__u16		nlmsg_type;	/* Message content */
	__u16		nlmsg_flags;	/* Additional flags */
	__u32		nlmsg_seq;	/* Sequence number */
	__u32		nlmsg_pid;	/* Sending process port ID */
};

// taken from https://github.com/torvalds/linux/blob/master/tools/include/uapi/linux/rtnetlink.h#L32 & #L55
#define RTM_GETLINK    18
#define RTM_GETNEIGH   30

// taken from https://github.com/tiann/KernelSU/blob/main/kernel/policy/allowlist.h
#define FIRST_APPLICATION_UID 10000
#define LAST_APPLICATION_UID 19999

static void *orig_rtnetlink_rcv_msg = NULL;
static void *found_rtnetlink_rcv_msg = NULL;


int hooked_rtnetlink_rcv_msg(struct sk_buff *skb, struct nlmsghdr *nlh, struct netlink_ext_ack *extack) {
    uid_t uid = current_uid();
	uid_t app_id = uid % 100000; // for multiple user profiles
	
	logkv("BYE_NEIGHBOR --> HOOK CALLER UID == %d -- APP ID == %d", uid, app_id);
    
    if (
		(app_id >= FIRST_APPLICATION_UID && app_id <= LAST_APPLICATION_UID)	/* uids for untrusted_apps (10000-19999)
		|| uid == 2000					/* uid for shell */
	) {
        if (nlh->nlmsg_type == RTM_GETLINK || nlh->nlmsg_type == RTM_GETNEIGH) {
            logkv("BYE_NEIGHBOR --> BLOCK RTM_* CALLS FOR UID:%d - APP ID:%d", uid, app_id);
            return -EACCES;
        }
    }

    int (*original)(struct sk_buff *, struct nlmsghdr *, struct netlink_ext_ack *) = orig_rtnetlink_rcv_msg;
    return original(skb, nlh, extack);
}

static long bye_init(const char *args, const char *event, void *__user reserved) {
	found_rtnetlink_rcv_msg = (void *) kallsyms_lookup_name("rtnetlink_rcv_msg");
    if (!found_rtnetlink_rcv_msg) {
        logke("BYE_NEIGHBOR --> `rtnetlink_rcv_msg` NOT FOUND\n");
        return -ENOENT;
    }

	logkd("BYE_NEIGHBOR --> FOUND `rtnetlink_rcv_msg` @ %d", (int*) found_rtnetlink_rcv_msg);

    hook_err_t err = hook(found_rtnetlink_rcv_msg, (void *)hooked_rtnetlink_rcv_msg, &orig_rtnetlink_rcv_msg);
    if (err != HOOK_NO_ERR) {
		logke("BYE_NEIGHBOR --> ERROR HOOKING rtnetlink_rcv_msg : %d", err);
        found_rtnetlink_rcv_msg = NULL;
        orig_rtnetlink_rcv_msg = NULL;
        return -EINVAL;
    }

    return 0;
}

static long bye_control0(const char *args, char *__user out, int outlen) {
    // logkd("BYE_NEIGHBOR --> control\n");
    return 0;
}

static long bye_exit(void *__user reserved) {
    if (orig_rtnetlink_rcv_msg) {
        unhook(found_rtnetlink_rcv_msg);
        
        orig_rtnetlink_rcv_msg = NULL;
        found_rtnetlink_rcv_msg = NULL;
        
        logkd("BYE_NEIGHBOR --> EXIT - UNHOOKED\n");
    } else {
		logkv("BYE_NEIGHBOR --> EXIT - NO UNHOOK\n");
	}

    return 0;
}

KPM_INIT(bye_init);
KPM_CTL0(bye_control0);
KPM_EXIT(bye_exit);
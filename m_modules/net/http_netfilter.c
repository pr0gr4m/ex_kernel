#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/netdevice.h>

static unsigned int hook_http(void *priv,
		struct sk_buff *skb, const struct nf_hook_state *state)
{
	struct iphdr *iph;
	struct tcphdr *th;
	char *data = NULL;
	int length = 0;
	
	if (!skb)
		return NF_ACCEPT;

	iph = ip_hdr(skb);

	if (iph->protocol == IPPROTO_TCP) {
		th = tcp_hdr(skb);

		length = skb->len - iph->ihl * 4 - th->doff * 4;
		if (length <= 4)
			return NF_ACCEPT;

		data = kzalloc(length, GFP_KERNEL);
		memcpy(data, (unsigned char *)th + th->doff * 4, length);

		if (strstr(data, "HTTP") != NULL) {
			printk("[Kernel:%s] HTTP Detected\n", __func__);
			kfree(data);
			return NF_DROP;
		}

		kfree(data);
	}

	return NF_ACCEPT;
}

static struct nf_hook_ops *nf_ops = NULL;

static int __init nfilter_init(void)
{
	nf_ops = (struct nf_hook_ops *)kzalloc(sizeof(struct nf_hook_ops), GFP_KERNEL);
	nf_ops->hook = (nf_hookfn *)hook_http;
	nf_ops->pf = PF_INET;
	nf_ops->hooknum = NF_INET_LOCAL_IN;
	nf_ops->priority = NF_IP_PRI_FIRST;

	nf_register_net_hook(&init_net, nf_ops);
	printk("[Kernel:%s] NFilter Init\n", __func__);
	return 0;
}

static void __exit nfilter_exit(void)
{
	nf_unregister_net_hook(&init_net, nf_ops);
	kfree(nf_ops);
	printk("[Kernel:%s] NFilter Exit\n", __func__);
}

module_init(nfilter_init);
module_exit(nfilter_exit);
MODULE_LICENSE("GPL");

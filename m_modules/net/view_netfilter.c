#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h> 
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/netdevice.h>

static void print_skb(struct sk_buff *skb)
{
	int i = 0, j = 0;

	printk("%7s", "offset ");

	for (i = 0; i < 16; i++) {
		printk(KERN_CONT "%02x ", i);

		if (!(i % 16 - 7))
			printk(KERN_CONT "- ");
	}
	printk(KERN_CONT "\n");

	for (i = 0; i < skb->len; i++) {
		if (!(i % 16))
			printk(KERN_CONT "0x%04x ", i);
		printk(KERN_CONT "%02x ", skb->data[i]);

		if (!(i % 16 - 7))
			printk(KERN_CONT "- ");

		if (!(i % 16 - 15)) {
			printk(KERN_CONT "\t");
			for (j = i - 15; j <= i; j++) {
				printk(KERN_CONT "%c", skb->data[j] >= 0x20 && skb->data[j] < 0x80 ? skb->data[j] : '.');
			}

			printk(KERN_CONT "\n");
		}
	}

	printk("\n");
}

static unsigned int hook_http(void *priv,
		struct sk_buff *skb, const struct nf_hook_state *state)
{
	struct iphdr *iph;
	struct tcphdr *th;
	int length = 0;
	unsigned long saddr = 0;
	unsigned long daddr = 0;
	unsigned short sport = 0;
	unsigned short dport = 0;
	int ctr = 0, keyword_walker = 0;
	char *keyword = "HTTP";
	
	if (!skb)
		return NF_ACCEPT;

	iph = ip_hdr(skb);

	if (iph->protocol == IPPROTO_TCP) {
		saddr = ntohl(iph->saddr);
		daddr = ntohl(iph->daddr);
		th = tcp_hdr(skb);
		length = skb->len - iph->ihl * 4 - th->doff * 4;
		if (length <= 0)
			return NF_ACCEPT;
		sport = th->source;
		dport = th->dest;
	}

	for (ctr = 0; ctr < skb->len; ctr++) {
		char ch = (char)(*(skb->data + ctr));
		if (ch == keyword[keyword_walker])
			keyword_walker++;
		else
			keyword_walker = 0;
		if (keyword_walker == 3) {
			print_skb(skb);
		}
	}
		
	return NF_ACCEPT;
}

static struct nf_hook_ops *nf_ops = NULL;

static int __init nfilter_init(void)
{
	int hooknums[NF_INET_NUMHOOKS] = {
		NF_INET_PRE_ROUTING,
		NF_INET_LOCAL_IN,
		NF_INET_FORWARD,
		NF_INET_LOCAL_OUT,
		NF_INET_POST_ROUTING
	};
	nf_ops = (struct nf_hook_ops *)kzalloc(sizeof(struct nf_hook_ops), GFP_KERNEL);
	nf_ops->hook = (nf_hookfn *)hook_http;
	nf_ops->pf = PF_INET;
	nf_ops->hooknum = hooknums[0];
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

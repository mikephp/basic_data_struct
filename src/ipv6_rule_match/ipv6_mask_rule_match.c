#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#if defined (__i386__)
static __inline__ unsigned long long GetCycleCount(void)
{
        unsigned long long int x;
        __asm__ volatile("rdtsc":"=A"(x));
        return x;
}
#elif defined (__x86_64__)
static __inline__ unsigned long long GetCycleCount(void)
{
        unsigned hi,lo;
        __asm__ volatile("rdtsc":"=a"(lo),"=d"(hi));
        return ((unsigned long long)lo)|(((unsigned long long)hi)<<32);
}
#endif
#define FREQUENCY 2000000000
void time_tick_used(unsigned long t2,unsigned long t1)
{
    printf("Use Ticks:%llu,Use Time:%.9f\n",(t2 - t1),(t2 - t1)*1.0/FREQUENCY);
}

#define IPV6_MASK_RULE_NUM 100000
#define PKT_KEY_NUM  10000

struct ipv6_mask_rule_item
{
    uint8_t protocol;
    uint8_t protocol_mask;
	uint8_t prot_mk;
    uint16_t sport;
    uint16_t sport_mask;
	uint16_t sp_mk;
    uint16_t dport;
    uint16_t dport_mask;
	uint16_t dp_mk;
    uint16_t sipv6[8];
    uint16_t sipv6_mask[8];
	uint16_t sipv6_mk[8];
    uint16_t dipv6[8];
    uint16_t dipv6_mask[8];
	uint16_t dipv6_mk[8];
	uint32_t rule_id;
} __attribute__((__packed__))ipv6_mask_rule_array[IPV6_MASK_RULE_NUM];

struct ipv6_pkt_key
{
    uint8_t protocol;
    uint16_t sport;
    uint16_t dport;
    uint16_t sipv6[8];
    uint16_t dipv6[8];
	uint32_t pkt_id;
	uint32_t match_id;
} __attribute__((__packed__))pkt_key_array[PKT_KEY_NUM];

int traverse(struct ipv6_pkt_key *key_array,struct ipv6_mask_rule_item * mrp_array)
{
	int i,j,k,flag;
	struct ipv6_pkt_key *key;
	struct ipv6_mask_rule_item * mrp;
	key = (struct ipv6_pkt_key *)&pkt_key_array[0];
	mrp = (struct ipv6_mask_rule_item *)&ipv6_mask_rule_array[0];
	for(i=0;i<PKT_KEY_NUM;i++){
		key = (struct ipv6_pkt_key *)&pkt_key_array[i];
		for(j=0;j<IPV6_MASK_RULE_NUM;j++){
			mrp = (struct ipv6_mask_rule_item *)&ipv6_mask_rule_array[j];
			if( key->protocol & mrp->protocol_mask == mrp->prot_mk){
				if(key->sport & mrp->sport_mask == mrp->sp_mk){
					if(key->dport & mrp->dport_mask == mrp->dp_mk){
						for(k=0;k<8;k++){
							if(key->sipv6[k] & mrp->sipv6_mask[k] == mrp->sipv6_mk[k]){
								flag = 0;
								if(key->dipv6[k] & mrp->dipv6_mask[k] == mrp->dipv6_mk[k]){
									flag = 1;
								}
							}
						}
						if(flag){
							key->match_id=j;
							break;
						}
						mrp++;
					}
					mrp++;
				}
				mrp++;
			}
		}
	}
	return 0;
}
int main(int argc, char *argv[])
{
    int rv = 0, opt = 0;
    unsigned long t1,t2;
    t1 = (unsigned long)GetCycleCount();

    int i, j, msg_id = 1;

    struct ipv6_mask_rule_item *mrp;//ipv6_mask_rule_pointer
    mrp = ipv6_mask_rule_array;

	//create 10w ipv6 mask rule
	mrp->rule_id = 0;
    for (i= 0; i< IPV6_MASK_RULE_NUM;i++){
        mrp->sipv6[i%8]=0x1234+i;
		mrp->sipv6_mask[i%8]=(0x01234+i);
		mrp->dipv6[i%8]=0x5678+i;
		mrp->dipv6_mask[i%8]=(0xBCDEF-i);
		mrp->sport = 80;
		mrp->sport_mask = 0xFFFF;
		mrp->dport = 8080;
		mrp->dport_mask = 0xFFFF;
   		mrp->protocol = 6;
		mrp->protocol_mask = 0xFF;
		mrp->rule_id++;

		for(j=0;j<8;j++){
			mrp->sipv6_mk[j] = mrp->sipv6[j] & mrp->sipv6_mask[j];
			mrp->dipv6_mk[j] = mrp->dipv6[j] & mrp->dipv6_mask[j];
		}
		mrp->sp_mk = mrp->sport & mrp->sport_mask;
		mrp->dp_mk = mrp->dport & mrp->dport_mask;
		mrp->prot_mk= mrp->protocol & mrp->protocol_mask;

		mrp++;
    }

	//create 10 pkt key
	struct ipv6_pkt_key * pkey = pkt_key_array;
	for(i=0;i<PKT_KEY_NUM;i++){
		pkey->sport = 80 ;
		pkey->protocol = 6;
		pkey->match_id = IPV6_MASK_RULE_NUM;
		pkey->pkt_id = i;
		pkey->sipv6[i%8] = 0x1234+i;
		pkey->dipv6[i%8] = 0xabcd+i;
		pkey++;
	}


    t2 = (unsigned long)GetCycleCount();
	time_tick_used(t2,t1);

	//match
	t1 = (unsigned long)GetCycleCount();
	printf("\nStart mask rule match ...\n");
	traverse(pkt_key_array,ipv6_mask_rule_array);
	t2 = (unsigned long)GetCycleCount();
	time_tick_used(t2,t1);
	printf("Over mask rule match ...\n\n");

	// print traverse result
	printf("\ntraverse results\n");
	int hit=0;
	for(i=0;i<PKT_KEY_NUM;i++){
		if(pkt_key_array[i].match_id<IPV6_MASK_RULE_NUM){
			hit++;
			printf("pkt_id=%d,rule_id=%d\n",pkt_key_array[i].pkt_id,pkt_key_array[i].match_id);
		}
	}
	printf("total hit num %d\n",hit);

	printf("\nsizeof(ipv6_mask_rule_array)=%d B\n",sizeof(ipv6_mask_rule_array));

	return 0;

}

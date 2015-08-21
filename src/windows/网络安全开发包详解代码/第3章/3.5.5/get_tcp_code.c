/* �ļ����֣�get_tcp_code.c */
#include "pcap.h"

/*
 -----------------------------------------------------------------------------------------------------------------------
    Libpcapͷ�ļ� ;
    ��������̫��Э���ʽ�Ķ���
 -----------------------------------------------------------------------------------------------------------------------
 */
struct ether_header
{
	u_int8_t	ether_dhost[6];

	/* Ŀ����̫����ַ */
	u_int8_t	ether_shost[6];

	/* Դ��̫����ַ */
	u_int16_t	ether_type;

	/* ��̫������ */
};

/* IP��ַ��ʽ�Ķ��� */
typedef u_int32_t	in_addr_t;
struct in_addr
{
	in_addr_t	s_addr;
};

/*
 -----------------------------------------------------------------------------------------------------------------------
    IPЭ���ʽ�Ķ���
 -----------------------------------------------------------------------------------------------------------------------
 */
struct ip_header
{
#if de f WORDS_BIGENDIAN
	u_int8_t		ip_version : 4, /* �汾 */
	ip_header_length : 4;			/* �ײ����� */
#else
	u_int8_t		ip_header_length : 4, ip_version : 4;
#endif
	u_int8_t		ip_tos_taowenliu;

	/* �������� */
	u_int16_t		ip_length;

	/* �ܳ��� */
	u_int16_t		ip_id;

	/* ��ʶ */
	u_int16_t		ip_off;

	/* ƫ�� */
	u_int8_t		ip_ttl;

	/* ����ʱ�� */
	u_int8_t		ip_protocol;

	/* Э������ */
	u_int16_t		ip_checksum;

	/* У��� */
	struct in_addr	ip_souce_address;

	/* ԴIP��ַ */
	struct in_addr	ip_destination_address;

	/* Ŀ��IP��ַ */
};

/*
 -----------------------------------------------------------------------------------------------------------------------
    ������TCPЭ���ʽ�Ķ���
 -----------------------------------------------------------------------------------------------------------------------
 */
struct tcp_header
{
	u_int16_t	tcp_source_port;

	/* Դ�˿� */
	u_int16_t	tcp_destination_port;

	/* Ŀ�Ķ˿� */
	u_int32_t	tcp_acknowledgement;

	/* ���к� */
	u_int32_t	tcp_ack;

	/* ȷ�Ϻ� */
#ifdef WORDS_BIGENDIAN
	u_int8_t	tcp_offset : 4,

	/* ƫ�� */
	tcp_reserved : 4;

	/* ���� */
#else
	u_int8_t	tcp_reserved : 4,

	/* ���� */
	tcp_offset : 4;

	/* ƫ�� */
#endif
	u_int8_t	tcp_flags;

	/* ��־ */
	u_int16_t	tcp_windows;

	/* ���ڴ�С */
	u_int16_t	tcp_checksum;

	/* У��� */
	u_int16_t	tcp_urgent_pointer;

	/* ����ָ�� */
};

/*
 =======================================================================================================================
    ������ʵ�ֲ���ͷ���TCP���ݰ��ĺ�������
 =======================================================================================================================
 */
void tcp_protocol_packet_callback
(
	u_char						*argument,
	const struct pcap_pkthdr	*packet_header,
	const u_char				*packet_content
)
{
	struct tcp_header	*tcp_protocol;

	/* TCPЭ����� */
	u_char				flags;

	/* ��� */
	int					header_length;

	/* �ײ����� */
	u_short				source_port;

	/* Դ�˿ں� */
	u_short				destination_port;

	/* Ŀ�Ķ˿ں� */
	u_short				windows;

	/* ���ڴ�С */
	u_short				urgent_pointer;

	/* ����ָ�� */
	u_int				sequence;

	/* ���к� */
	u_int				acknowledgement;

	/* ȷ�Ϻ� */
	u_int16_t			checksum;

	/* У��� */
	tcp_protocol = (struct tcp_header *) (packet_content + 14 + 20);

	/* ���TCPЭ���������ݣ�Ӧ��������̫��ͷ��IPͷ���� */
	source_port = ntohs(tcp_protocol->tcp_source_port);

	/* ���Դ�˿ں� */
	destination_port = ntohs(tcp_protocol->tcp_destination_port);

	/* ���Ŀ�Ķ˿ں� */
	header_length = tcp_protocol->tcp_offset * 4;

	/* ����ײ����� */
	sequence = ntohl(tcp_protocol->tcp_acknowledgement);

	/* ������к� */
	acknowledgement = ntohl(tcp_protocol->tcp_ack);

	/* ���ȷ�Ϻ� */
	windows = ntohs(tcp_protocol->tcp_windows);

	/* ��ô��ڴ�С */
	urgent_pointer = ntohs(tcp_protocol->tcp_urgent_pointer);

	/* ��ý���ָ�� */
	flags = tcp_protocol->tcp_flags;

	/* ��ñ�� */
	checksum = ntohs(tcp_protocol->tcp_checksum);

	/* ���У��� */
	printf("-------  TCP Protocol  (Transport Layer)  -------\n");
	printf("Source Port:%d\n", source_port);

	/* ���Դ�˿ں� */
	printf("Destination Port:%d\n", destination_port);

	/* ���Ŀ�Ķ˿ں� */
	switch(destination_port)	/* �ж��ϲ�Э������ */
	{
	case 80:	printf("HTTP protocol\n"); break;
	/* �˿���80����ʾ�ϲ�Э����HTTPЭ�� */
	case 21:	printf("FTP protocol\n"); break;
	/* �˿���21����ʾ�ϲ�Э����FTPЭ�� */
	case 23:	printf("TELNET protocol\n"); break;
	/* �˿���23����ʾ�ϲ�Э����TELNETЭ�� */
	case 25:	printf("SMTP protocol\n"); break;
	/* �˿���25����ʾ�ϲ�Э����SMTPЭ�� */
	case 110:	printf("POP3 protocol\n"); break;
	/* �˿���110����ʾ�ϲ�Э����POP3Э�� */
	default:	break;
	}

	printf("Sequence Number:%u\n", sequence);

	/* ������к� */
	printf("Acknowledgement Number:%u\n", acknowledgement);

	/* ���ȷ�Ϻ� */
	printf("Header Length:%d\n", header_length);

	/* ����ײ����� */
	printf("Reserved:%d\n", tcp_protocol->tcp_reserved);
	printf("Flags:");

	/* ������ */
	if(flags & 0x08) printf("PSH ");
	if(flags & 0x10) printf("ACK ");
	if(flags & 0x02) printf("SYN ");
	if(flags & 0x20) printf("URG ");
	if(flags & 0x01) printf("FIN ");
	if(flags & 0x04) printf("RST ");
	printf("\n");
	printf("Window Size:%d\n", windows);

	/* ������ڴ�С */
	printf("Checksum:%d\n", checksum);

	/* ���У��� */
	printf("Urgent pointer:%d\n", urgent_pointer);

	/* �������ָ�� */
}

/*
 =======================================================================================================================
    ������ʵ�ַ���IPЭ��ĺ�������
 =======================================================================================================================
 */
void ip_protocol_packet_callback
(
	u_char						*argument,
	const struct pcap_pkthdr	*packet_header,
	const u_char				*packet_content
)
{
	struct ip_header	*ip_protocol;

	/* IPЭ����� */
	u_int				header_length;

	/* �ײ����� */
	u_int				offset;

	/* ƫ�� */
	u_char				tos;

	/* �������� */
	u_int16_t			checksum;

	/* У��� */
	printf("-------   IP Protocol  (Network Layer)   --------\n");
	ip_protocol = (struct ip_header *) (packet_content + 14);

	/* ���IPЭ���������ݣ�Ӧ��������̫��Э��ͷ */
	checksum = ntohs(ip_protocol->ip_checksum);

	/* ��ȡУ��� */
	header_length = ip_protocol->ip_header_length * 4;

	/* ��ȡ�ײ����� */
	tos = ip_protocol->ip_tos_taowenliu;

	/* ��ȡ�������� */
	offset = ntohs(ip_protocol->ip_off);

	/* ��ȡƫ���� */
	printf("IP Version:%d\n", ip_protocol->ip_version);

	/* ��ȡ�汾 */
	printf("Header length:%d\n", header_length);

	/* ������� */
	printf("TOS:%d\n", tos);

	/* ����������� */
	printf("Total length:%d\n", ntohs(ip_protocol->ip_length));

	/* ��ȡ�ܳ��� */
	printf("Identification:%d\n", ntohs(ip_protocol->ip_id));

	/* ��ȡ��ʶ */
	printf("Offset:%d\n", (offset & 0x1fff) * 8);

	/* ���ƫ�� */
	printf("TTL:%d\n", ip_protocol->ip_ttl);

	/* ��ȡTTL */
	printf("Protocol:%d\n", ip_protocol->ip_protocol);

	/* ��ȡЭ������ */
	switch(ip_protocol->ip_protocol)	/* �ж��ϲ�Э������ */
	{
	case 6:		printf("The Transport Layer Protocol is TCP\n"); break;
	/* ���Э��ֵ��6����ʾ�ϲ�Э����TCPЭ�� */
	case 17:	printf("The Transport Layer Protocol is UDP\n"); break;
	/* ���Э��ֵ��17����ʾ�ϲ�Э����UDPЭ�� */
	case 1:		printf("The Transport Layer Protocol is ICMP\n"); break;
	/* �����1����ʾ�ϲ�Э����ICMPЭ�� */
	default:	break;
	}

	printf("Header checksum:%d\n", checksum);
	printf("Source address:%s\n", inet_ntoa(ip_protocol->ip_souce_address));

	/* ���ԴIP��ַ */
	printf("Destination address:%s\n", inet_ntoa(ip_protocol->ip_destination_address));

	/* ���Ŀ��IP��ַ */
	switch(ip_protocol->ip_protocol)
	{
	case 6:		tcp_protocol_packet_callback(argument, packet_header, packet_content); break;
	/* ����ж��ϲ�Э����TCPЭ�飬�͵��÷���TCPЭ��ĺ�����ע�⣬��ʱ�Ĳ������� */
	default:	break;
	}
}

/*
 =======================================================================================================================
    ������ʵ�ַ�����̫��Э��ĺ����Ķ���
 =======================================================================================================================
 */
void ethernet_protocol_packet_callback
(
	u_char						*argument,
	const struct pcap_pkthdr	*packet_header,
	const u_char				*packet_content
)
{
	u_short				ethernet_type;

	/* ��̫������ */
	struct ether_header *ethernet_protocol;

	/* ��̫��Э����� */
	u_char				*mac_string;

	/* ��̫����ַ */
	static int			packet_number = 1;
	printf("**************************************************\n");
	printf("The %d  TCP  packet is captured.\n", packet_number);
	printf("-------    Ehternet Protocol (Link Layer)    -----\n");
	ethernet_protocol = (struct ether_header *) packet_content;

	/* �����̫��Э���������� */
	printf("Ethernet type is :\n");
	ethernet_type = ntohs(ethernet_protocol->ether_type);

	/* �����̫������ */
	printf("%04x\n", ethernet_type);
	switch(ethernet_type)	/* �ж���̫������ */
	{
	case 0x0800:	printf("The network layer is IP protocol\n"); break;
	/* �ж��ϲ�Э��ΪIPЭ�� */
	case 0x0806:	printf("The network layer is ARP protocol\n"); break;
	/* �ж��ϲ�Э��ΪARPЭ�� */
	case 0x8035:	printf("The network layer is RARP protocol\n"); break;
	/* �ж��ϲ�Э��ΪRARPЭ�� */
	default:		break;
	}

	printf("Mac Source Address is : \n");
	mac_string = ethernet_protocol->ether_shost;
	printf
	(
		"%02x:%02x:%02x:%02x:%02x:%02x\n",
		*mac_string,
		*(mac_string + 1),
		*(mac_string + 2),
		*(mac_string + 3),
		*(mac_string + 4),
		*(mac_string + 5)
	);

	/* ��ȡԴ��̫����ַ */
	printf("Mac Destination Address is : \n");
	mac_string = ethernet_protocol->ether_dhost;
	printf
	(
		"%02x:%02x:%02x:%02x:%02x:%02x\n",
		*mac_string,
		*(mac_string + 1),
		*(mac_string + 2),
		*(mac_string + 3),
		*(mac_string + 4),
		*(mac_string + 5)
	);

	/* ��ȡĿ����̫����ַ */
	switch(ethernet_type)
	{
	case 0x0800:	ip_protocol_packet_callback(argument, packet_header, packet_content); break;
	/* �ж��ϲ�Э��ΪIPЭ�飬���Ե��÷���IPЭ��ĺ�����ע��������� */
	default:		break;
	}

	printf("**************************************************\n");
	packet_number++;
}

/*
 =======================================================================================================================
    ������������
 =======================================================================================================================
 */
void main()
{
	pcap_t				*pcap_handle;

	/* Libpcap��� */
	char				error_content[PCAP_ERRBUF_SIZE];

	/* �洢������Ϣ */
	char				*net_interface;

	/* ����ӿ� */
	struct bpf_program	bpf_filter;

	/* BPF���˹��� */
	char				bpf_filter_string[] = "tcp";

	/* ���˹����ַ�������ʱ��ʾ������ֻ����TCPЭ����������ݰ� */
	bpf_u_int32			net_mask;

	/* �������� */
	bpf_u_int32			net_ip;

	/* �����ַ */
	net_interface = pcap_lookupdev(error_content);

	/* �������ӿ� */
	pcap_lookupnet(net_interface, &net_ip, &net_mask, error_content);

	/* ��������ַ���������� */
	pcap_handle = pcap_open_live(net_interface, BUFSIZ, 1, 0, error_content);

	/* �������豸 */
	pcap_compile(pcap_handle, &bpf_filter, bpf_filter_string, 0, net_ip);

	/* ������˹��� */
	pcap_setfilter(pcap_handle, &bpf_filter);

	/* ���ù��˹��� */
	if(pcap_datalink(pcap_handle) != DLT_EN10MB) return;
	pcap_loop(pcap_handle, -1, ethernet_protocol_packet_callback, NULL);

	/* ע��ص�������ѭ�������������ݰ������ûص����������ݰ����д��� */
	pcap_close(pcap_handle);

	/* �ر�Libpcap���� */
}

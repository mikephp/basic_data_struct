/* 文件名字：get_tcp_code.c */
#include "pcap.h"

/*
 -----------------------------------------------------------------------------------------------------------------------
    Libpcap头文件 ;
    以下是以太网协议格式的定义
 -----------------------------------------------------------------------------------------------------------------------
 */
struct ether_header
{
	u_int8_t	ether_dhost[6];

	/* 目的以太网地址 */
	u_int8_t	ether_shost[6];

	/* 源以太网地址 */
	u_int16_t	ether_type;

	/* 以太网类型 */
};

/* IP地址格式的定义 */
typedef u_int32_t	in_addr_t;
struct in_addr
{
	in_addr_t	s_addr;
};

/*
 -----------------------------------------------------------------------------------------------------------------------
    IP协议格式的定义
 -----------------------------------------------------------------------------------------------------------------------
 */
struct ip_header
{
#if de f WORDS_BIGENDIAN
	u_int8_t		ip_version : 4, /* 版本 */
	ip_header_length : 4;			/* 首部长度 */
#else
	u_int8_t		ip_header_length : 4, ip_version : 4;
#endif
	u_int8_t		ip_tos_taowenliu;

	/* 服务质量 */
	u_int16_t		ip_length;

	/* 总长度 */
	u_int16_t		ip_id;

	/* 标识 */
	u_int16_t		ip_off;

	/* 偏移 */
	u_int8_t		ip_ttl;

	/* 生存时间 */
	u_int8_t		ip_protocol;

	/* 协议类型 */
	u_int16_t		ip_checksum;

	/* 校验和 */
	struct in_addr	ip_souce_address;

	/* 源IP地址 */
	struct in_addr	ip_destination_address;

	/* 目的IP地址 */
};

/*
 -----------------------------------------------------------------------------------------------------------------------
    下面是TCP协议格式的定义
 -----------------------------------------------------------------------------------------------------------------------
 */
struct tcp_header
{
	u_int16_t	tcp_source_port;

	/* 源端口 */
	u_int16_t	tcp_destination_port;

	/* 目的端口 */
	u_int32_t	tcp_acknowledgement;

	/* 序列号 */
	u_int32_t	tcp_ack;

	/* 确认号 */
#ifdef WORDS_BIGENDIAN
	u_int8_t	tcp_offset : 4,

	/* 偏移 */
	tcp_reserved : 4;

	/* 保留 */
#else
	u_int8_t	tcp_reserved : 4,

	/* 保留 */
	tcp_offset : 4;

	/* 偏移 */
#endif
	u_int8_t	tcp_flags;

	/* 标志 */
	u_int16_t	tcp_windows;

	/* 窗口大小 */
	u_int16_t	tcp_checksum;

	/* 校验和 */
	u_int16_t	tcp_urgent_pointer;

	/* 紧急指针 */
};

/*
 =======================================================================================================================
    下面是实现捕获和分析TCP数据包的函数定义
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

	/* TCP协议变量 */
	u_char				flags;

	/* 标记 */
	int					header_length;

	/* 首部长度 */
	u_short				source_port;

	/* 源端口号 */
	u_short				destination_port;

	/* 目的端口号 */
	u_short				windows;

	/* 窗口大小 */
	u_short				urgent_pointer;

	/* 紧急指针 */
	u_int				sequence;

	/* 序列号 */
	u_int				acknowledgement;

	/* 确认号 */
	u_int16_t			checksum;

	/* 校验和 */
	tcp_protocol = (struct tcp_header *) (packet_content + 14 + 20);

	/* 获得TCP协议数据内容，应该跳过以太网头和IP头部分 */
	source_port = ntohs(tcp_protocol->tcp_source_port);

	/* 获得源端口号 */
	destination_port = ntohs(tcp_protocol->tcp_destination_port);

	/* 获得目的端口号 */
	header_length = tcp_protocol->tcp_offset * 4;

	/* 获得首部长度 */
	sequence = ntohl(tcp_protocol->tcp_acknowledgement);

	/* 获得序列号 */
	acknowledgement = ntohl(tcp_protocol->tcp_ack);

	/* 获得确认号 */
	windows = ntohs(tcp_protocol->tcp_windows);

	/* 获得窗口大小 */
	urgent_pointer = ntohs(tcp_protocol->tcp_urgent_pointer);

	/* 获得紧急指针 */
	flags = tcp_protocol->tcp_flags;

	/* 获得标记 */
	checksum = ntohs(tcp_protocol->tcp_checksum);

	/* 获得校验和 */
	printf("-------  TCP Protocol  (Transport Layer)  -------\n");
	printf("Source Port:%d\n", source_port);

	/* 输出源端口号 */
	printf("Destination Port:%d\n", destination_port);

	/* 输出目的端口号 */
	switch(destination_port)	/* 判断上层协议类型 */
	{
	case 80:	printf("HTTP protocol\n"); break;
	/* 端口是80，表示上层协议是HTTP协议 */
	case 21:	printf("FTP protocol\n"); break;
	/* 端口是21，表示上层协议是FTP协议 */
	case 23:	printf("TELNET protocol\n"); break;
	/* 端口是23，表示上层协议是TELNET协议 */
	case 25:	printf("SMTP protocol\n"); break;
	/* 端口是25，表示上层协议是SMTP协议 */
	case 110:	printf("POP3 protocol\n"); break;
	/* 端口是110，表示上层协议是POP3协议 */
	default:	break;
	}

	printf("Sequence Number:%u\n", sequence);

	/* 输出序列号 */
	printf("Acknowledgement Number:%u\n", acknowledgement);

	/* 输出确认号 */
	printf("Header Length:%d\n", header_length);

	/* 输出首部长度 */
	printf("Reserved:%d\n", tcp_protocol->tcp_reserved);
	printf("Flags:");

	/* 输出标记 */
	if(flags & 0x08) printf("PSH ");
	if(flags & 0x10) printf("ACK ");
	if(flags & 0x02) printf("SYN ");
	if(flags & 0x20) printf("URG ");
	if(flags & 0x01) printf("FIN ");
	if(flags & 0x04) printf("RST ");
	printf("\n");
	printf("Window Size:%d\n", windows);

	/* 输出窗口大小 */
	printf("Checksum:%d\n", checksum);

	/* 输出校验和 */
	printf("Urgent pointer:%d\n", urgent_pointer);

	/* 输出紧急指针 */
}

/*
 =======================================================================================================================
    下面是实现分析IP协议的函数定义
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

	/* IP协议变量 */
	u_int				header_length;

	/* 首部长度 */
	u_int				offset;

	/* 偏移 */
	u_char				tos;

	/* 服务质量 */
	u_int16_t			checksum;

	/* 校验和 */
	printf("-------   IP Protocol  (Network Layer)   --------\n");
	ip_protocol = (struct ip_header *) (packet_content + 14);

	/* 获得IP协议数据内容，应该跳过以太网协议头 */
	checksum = ntohs(ip_protocol->ip_checksum);

	/* 获取校验和 */
	header_length = ip_protocol->ip_header_length * 4;

	/* 获取首部长度 */
	tos = ip_protocol->ip_tos_taowenliu;

	/* 获取服务质量 */
	offset = ntohs(ip_protocol->ip_off);

	/* 获取偏移量 */
	printf("IP Version:%d\n", ip_protocol->ip_version);

	/* 获取版本 */
	printf("Header length:%d\n", header_length);

	/* 输出长度 */
	printf("TOS:%d\n", tos);

	/* 输出服务质量 */
	printf("Total length:%d\n", ntohs(ip_protocol->ip_length));

	/* 获取总长度 */
	printf("Identification:%d\n", ntohs(ip_protocol->ip_id));

	/* 获取标识 */
	printf("Offset:%d\n", (offset & 0x1fff) * 8);

	/* 输出偏移 */
	printf("TTL:%d\n", ip_protocol->ip_ttl);

	/* 获取TTL */
	printf("Protocol:%d\n", ip_protocol->ip_protocol);

	/* 获取协议类型 */
	switch(ip_protocol->ip_protocol)	/* 判断上层协议类型 */
	{
	case 6:		printf("The Transport Layer Protocol is TCP\n"); break;
	/* 如果协议值是6，表示上层协议是TCP协议 */
	case 17:	printf("The Transport Layer Protocol is UDP\n"); break;
	/* 如果协议值是17，表示上层协议是UDP协议 */
	case 1:		printf("The Transport Layer Protocol is ICMP\n"); break;
	/* 如果是1，表示上层协议是ICMP协议 */
	default:	break;
	}

	printf("Header checksum:%d\n", checksum);
	printf("Source address:%s\n", inet_ntoa(ip_protocol->ip_souce_address));

	/* 获得源IP地址 */
	printf("Destination address:%s\n", inet_ntoa(ip_protocol->ip_destination_address));

	/* 获得目的IP地址 */
	switch(ip_protocol->ip_protocol)
	{
	case 6:		tcp_protocol_packet_callback(argument, packet_header, packet_content); break;
	/* 如果判断上层协议是TCP协议，就调用分析TCP协议的函数。注意，此时的参数传递 */
	default:	break;
	}
}

/*
 =======================================================================================================================
    下面是实现分析以太网协议的函数的定义
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

	/* 以太网类型 */
	struct ether_header *ethernet_protocol;

	/* 以太网协议变量 */
	u_char				*mac_string;

	/* 以太网地址 */
	static int			packet_number = 1;
	printf("**************************************************\n");
	printf("The %d  TCP  packet is captured.\n", packet_number);
	printf("-------    Ehternet Protocol (Link Layer)    -----\n");
	ethernet_protocol = (struct ether_header *) packet_content;

	/* 获得以太网协议数据内容 */
	printf("Ethernet type is :\n");
	ethernet_type = ntohs(ethernet_protocol->ether_type);

	/* 获得以太网类型 */
	printf("%04x\n", ethernet_type);
	switch(ethernet_type)	/* 判断以太网类型 */
	{
	case 0x0800:	printf("The network layer is IP protocol\n"); break;
	/* 判断上层协议为IP协议 */
	case 0x0806:	printf("The network layer is ARP protocol\n"); break;
	/* 判断上层协议为ARP协议 */
	case 0x8035:	printf("The network layer is RARP protocol\n"); break;
	/* 判断上层协议为RARP协议 */
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

	/* 获取源以太网地址 */
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

	/* 获取目的以太网地址 */
	switch(ethernet_type)
	{
	case 0x0800:	ip_protocol_packet_callback(argument, packet_header, packet_content); break;
	/* 判断上层协议为IP协议，所以调用分析IP协议的函数，注意参数传递 */
	default:		break;
	}

	printf("**************************************************\n");
	packet_number++;
}

/*
 =======================================================================================================================
    下面是主函数
 =======================================================================================================================
 */
void main()
{
	pcap_t				*pcap_handle;

	/* Libpcap句柄 */
	char				error_content[PCAP_ERRBUF_SIZE];

	/* 存储错误信息 */
	char				*net_interface;

	/* 网络接口 */
	struct bpf_program	bpf_filter;

	/* BPF过滤规则 */
	char				bpf_filter_string[] = "tcp";

	/* 过滤规则字符串，此时表示本程序只捕获TCP协议的网络数据包 */
	bpf_u_int32			net_mask;

	/* 网络掩码 */
	bpf_u_int32			net_ip;

	/* 网络地址 */
	net_interface = pcap_lookupdev(error_content);

	/* 获得网络接口 */
	pcap_lookupnet(net_interface, &net_ip, &net_mask, error_content);

	/* 获得网络地址和网络掩码 */
	pcap_handle = pcap_open_live(net_interface, BUFSIZ, 1, 0, error_content);

	/* 打开网络设备 */
	pcap_compile(pcap_handle, &bpf_filter, bpf_filter_string, 0, net_ip);

	/* 编译过滤规则 */
	pcap_setfilter(pcap_handle, &bpf_filter);

	/* 设置过滤规则 */
	if(pcap_datalink(pcap_handle) != DLT_EN10MB) return;
	pcap_loop(pcap_handle, -1, ethernet_protocol_packet_callback, NULL);

	/* 注册回调函数，循环捕获网络数据包，利用回调函数对数据包进行处理 */
	pcap_close(pcap_handle);

	/* 关闭Libpcap操作 */
}

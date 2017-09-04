/***********************************************************************
	Copyright (c) 2017, The OpenBRAS project authors. All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are
	met:

	  * Redistributions of source code must retain the above copyright
		notice, this list of conditions and the following disclaimer.

	  * Redistributions in binary form must reproduce the above copyright
		notice, this list of conditions and the following disclaimer in
		the documentation and/or other materials provided with the
		distribution.

	  * Neither the name of OpenBRAS nor the names of its contributors may
		be used to endorse or promote products derived from this software
		without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

**********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <netinet/in.h>
#include <setjmp.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>

#include <rte_common.h>
#include <rte_log.h>
#include <rte_malloc.h>
#include <rte_memory.h>
#include <rte_memcpy.h>
#include <rte_memzone.h>
#include <rte_eal.h>
#include <rte_launch.h>
#include <rte_atomic.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_interrupts.h>
#include <rte_pci.h>
#include <rte_random.h>
#include <rte_debug.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>

#include "CDPDKPacketProcess.h"
#include "glog/logging.h"
#include "webrtc/system_wrappers/include/cpu_info.h"
#include <sstream>
#include <string>
#include <list>
using namespace std;
using namespace webrtc;

using namespace std;

#define RTE_LOGTYPE_L2FWD RTE_LOGTYPE_USER1

#define NB_MBUF   8192

#define MAX_PKT_BURST 32
#define BURST_TX_DRAIN_US 100 /* TX drain every ~100us */

/*
 * Configurable number of RX/TX ring descriptors
 */
#define RTE_TEST_RX_DESC_DEFAULT 128
#define RTE_TEST_TX_DESC_DEFAULT 512
static uint16_t nb_rxd = RTE_TEST_RX_DESC_DEFAULT;
static uint16_t nb_txd = RTE_TEST_TX_DESC_DEFAULT;

/* ethernet addresses of ports */
static struct ether_addr l2fwd_ports_eth_addr[RTE_MAX_ETHPORTS];

//static struct PortInfo portinfo[RTE_MAX_ETHPORTS];

static unsigned int dpdkpacket_rx_queue_per_lcore = 1;

//static int nbport = 0;

struct mbuf_table {
    unsigned len;
    struct rte_mbuf *m_table[MAX_PKT_BURST];
};

#define MAX_RX_QUEUE_PER_LCORE 16
#define MAX_TX_QUEUE_PER_PORT 16
struct lcore_queue_conf {
    unsigned n_rx_port;
    unsigned rx_port_list[MAX_RX_QUEUE_PER_LCORE];
    struct mbuf_table tx_mbufs[RTE_MAX_ETHPORTS];
    uint16_t tx_queue_id[RTE_MAX_ETHPORTS];
} __rte_cache_aligned;

rte_spinlock_t txlock;


struct lcore_queue_conf lcore_queue_conf[RTE_MAX_LCORE];

struct rte_eth_conf port_conf;

struct rte_mempool * packet_pktmbuf_pool = NULL;

struct cpumap
{
    int count;
    int map;
};

struct cpumap CPUMaps[]
{
    {0,0x0000},
    {1,0x0001},
    {2,0x0003},
    {3,0x0007},
    {4,0x000f},
    {5,0x001f},
    {6,0x003f},
    {7,0x007f},
    {8,0x00ff},
    {9,0x001ff},
    {10,0x03ff},
    {11,0x07ff},
    {12,0x0fff},
    {13,0x1fff},
    {14,0x3fff},
    {15,0x7fff},
    {16,0xffff},
};

IDPDKPacketProcess &IDPDKPacketProcess::Instance()
{
    static CDPDKPacketProcess process;
    return process;
}


/* Send the burst of packets on an output interface */
static inline int
send_burst(struct lcore_queue_conf *qconf, uint16_t n, uint8_t port)
{
    struct rte_mbuf **m_table;
    int ret;
    uint16_t queueid;

    queueid = qconf->tx_queue_id[port];
    m_table = (struct rte_mbuf **)qconf->tx_mbufs[port].m_table;

    ret = rte_eth_tx_burst(port, queueid, m_table, n);
    if (unlikely(ret < n)) {
        do {
            rte_pktmbuf_free(m_table[ret]);
        } while (++ret < n);
    }

    return 0;

}



/* Enqueue packets for TX and prepare them to be sent */
static int
send_one_packet(struct rte_mbuf *m, uint8_t port, uint32_t lcore_id)
{
    unsigned len = 0;
    struct lcore_queue_conf *qconf = NULL;

    printf("send_packet, coreid=%d, port=%d\n",lcore_id, port);

    qconf = &lcore_queue_conf[lcore_id];
    len = qconf->tx_mbufs[port].len;
    qconf->tx_mbufs[port].m_table[len] = m;
    len++;

    /* enough pkts to be sent */
    if (unlikely(len == MAX_PKT_BURST)) {
        send_burst(qconf, MAX_PKT_BURST, port);
        len = 0;
    }

    qconf->tx_mbufs[port].len = len;
    return 0;
}


/* main processing loop */
void
CDPDKPacketProcess::main_loop(uint32_t coreid)
{
    struct rte_mbuf *pkts_burst[MAX_PKT_BURST];
    unsigned lcore_id;
    uint64_t prev_tsc, diff_tsc, cur_tsc;
    unsigned i, j, portid, nb_rx;
    struct lcore_queue_conf *qconf;
    const uint64_t drain_tsc = (rte_get_tsc_hz() + US_PER_S - 1) / US_PER_S * BURST_TX_DRAIN_US;

    prev_tsc = 0;
    //timer_tsc = 0;

    lcore_id = rte_lcore_id();
    qconf = &lcore_queue_conf[lcore_id];

    printf("main_loop, lcore_id=%d\n", lcore_id);

#if 0
    if (qconf->n_rx_port == 0) {
        printf("main_loop, lcore_id=%d, not set rx queue.exit\n", lcore_id);

        return;
    }
#endif
    //rte_spinlock_trylock(&global_flag_stru_p->lock);
    printf("entering main loop on lcore %u\n", lcore_id);

    while (1) {
        //rte_spinlock_unlock(&global_flag_stru_p->lock);
        cur_tsc = rte_rdtsc();
        //printf("l2fwd_main_loop, cur_tsc=%d\n", cur_tsc);

        /*
        * TX burst queue drain
        */
        diff_tsc = cur_tsc - prev_tsc;
        if (unlikely(diff_tsc > drain_tsc)) {
            rte_spinlock_trylock(&txlock);
            for (portid = 0; portid < RTE_MAX_ETHPORTS; portid++) {
                if (qconf->tx_mbufs[portid].len == 0)
                    continue;
                send_burst(&lcore_queue_conf[lcore_id],
                    qconf->tx_mbufs[portid].len,
                    (uint8_t) portid);
                qconf->tx_mbufs[portid].len = 0;
            }
            rte_spinlock_unlock(&txlock);
            prev_tsc = cur_tsc;
        }
        //printf("l2fwd_main_loop, Read packet from RX queues, qconf->n_rx_port=%d\n", qconf->n_rx_port);

        /*
        * Read packet from RX queues
        */
        for (i = 0; i < qconf->n_rx_port; i++) {
            //printf("l2fwd_main_loop, Read packet from RX queues, i=%d\n", i);

            portid = qconf->rx_port_list[i];
            nb_rx = rte_eth_rx_burst((uint8_t) portid, 0,pkts_burst, MAX_PKT_BURST);

            for (j = 0; j < nb_rx; j++) {
                //printf("l2fwd_main_loop, Read packet from RX queues, nb_rx=%d, j=%d\n", nb_rx, j);
                //m = pkts_burst[j];
                m_psink->OnLCorePacket((char*)(rte_pktmbuf_mtod(pkts_burst[j], char*)),rte_pktmbuf_data_len(pkts_burst[j]), portid, lcore_id);
                rte_pktmbuf_free(pkts_burst[j]);
            }
        }
    }
    printf("entering main loop on lcore %u end\n", lcore_id);
}

static int
launch_one_lcore(void *arg)
{
    printf("launch_one_lcore\n");

    CDPDKPacketProcess *pthread = (CDPDKPacketProcess *)arg;
    if (pthread)
    {
        pthread->main_loop(rte_lcore_id());
    }
    return 0;

}

/* Check the link status of all ports in up to 9s, and print them finally */
static void
check_all_ports_link_status(uint8_t port_num)
{
#define CHECK_INTERVAL 100 /* 100ms */
#define MAX_CHECK_TIME 90 /* 9s (90 * 100ms) in total */
	uint8_t portid, count, all_ports_up, print_flag = 0;
	struct rte_eth_link link;

	printf("\nChecking link status");
	fflush(stdout);
	for (count = 0; count <= MAX_CHECK_TIME; count++) {
		all_ports_up = 1;
		for (portid = 0; portid < port_num; portid++) {
			memset(&link, 0, sizeof(link));
			rte_eth_link_get_nowait(portid, &link);
			/* print link status if flag set */
			if (print_flag == 1) {
				if (link.link_status)
					printf("Port %d Link Up - speed %u "
						"Mbps - %s\n", (uint8_t)portid,
						(unsigned)link.link_speed,
				(link.link_duplex == ETH_LINK_FULL_DUPLEX) ?
					("full-duplex") : ("half-duplex\n"));
				else
					printf("Port %d Link Down\n",
						(uint8_t)portid);
				continue;
			}
			/* clear all_ports_up flag if any link down */
			if (link.link_status == 0) {
				all_ports_up = 0;
				break;
			}
		}
		/* after finally printing all link status, get out */
		if (print_flag == 1)
			break;

		if (all_ports_up == 0) {
			printf(".");
			fflush(stdout);
			rte_delay_ms(CHECK_INTERVAL);
		}

		/* set the print_flag if all ports up or timeout */
		if (all_ports_up == 1 || count == (MAX_CHECK_TIME - 1)) {
			print_flag = 1;
			printf("done\n");
		}
	}
}


CDPDKPacketProcess::CDPDKPacketProcess()
    :m_psink(NULL)
    ,m_isInit(false)
{
    
}

CDPDKPacketProcess::~CDPDKPacketProcess()
{
    
}
int g_dpdk_argc = 0;
char *g_dpdk_argv[5] ={0};

int CDPDKPacketProcess::Open(IDPDKPacketProcessSink *psink)
{
    int ret = 0;
    char *aValue =NULL;
    size_t aValueSize=0;
    bool isdpdkdiable = false;

    uint32_t count = CpuInfo::DetectNumberOfCores();
    if (count > 16)
    {
        count = 16;
    }
    int map = CPUMaps[count].map;
    //-c f -n 4
    //int rte_argc = 5;

    struct param 
    {
        char cores[64];
        char coresmap[64];
        char n[64];
        char channel[64];
    };

    struct param params;
    ::memset(&params, 0, sizeof(params));
    strcpy(params.cores,"-c");
    sprintf(params.coresmap,"%x",map);
    strcpy(params.n,"-n");
    strcpy(params.channel,"4");

	g_dpdk_argv[0]="OPENBRAS";
	g_dpdk_argv[1]="-c";
	g_dpdk_argv[2]=params.coresmap;
	g_dpdk_argv[3]="-n";
	g_dpdk_argv[4]="4";
	g_dpdk_argc = 5;
	
	printf("the cpu core map is:%s\n",params.coresmap);

    m_psink = psink;

    if (ACE_Thread_Manager::instance ()->spawn(CDPDKPacketProcess::server_worker, this) == -1)
    {
        return -1;
    }

    return 0;
}
ACE_THR_FUNC_RETURN
CDPDKPacketProcess::server_worker (void *p)
{
    //printf("CPacketServiceProcess::server_worker\n");

    CDPDKPacketProcess *pProcess= (CDPDKPacketProcess *)p;

    pProcess->OpenThread(g_dpdk_argc, g_dpdk_argv);

    return 0;
}

int CDPDKPacketProcess::OpenThread(int argc, char **argv)
{
    if (m_isInit == true)
    {
        return 0;
    }
    //m_psink = psink;
    struct lcore_queue_conf *qconf;
    struct rte_eth_dev_info dev_info;
    int ret;
    uint8_t nb_ports;
    uint8_t nb_ports_available;
    uint8_t portid;
    unsigned rx_lcore_id;
    //uint8_t last_port;
    uint32_t lcore_id;
    uint16_t queueid = 0;
    uint32_t n_tx_queue =0;
    uint32_t nb_lcores = 0;

    ::memset(&lcore_queue_conf, 0, sizeof(lcore_queue_conf));
    
    ::memset(&port_conf, 0, sizeof(port_conf));
    port_conf.rxmode.max_rx_pkt_len = ETHER_MAX_LEN;
#if 0
    printf("CDPDKPacketProcess::Open, argc=%d\n",argc);
    for(int i=0;i<argc;i++)
    {
        printf("param %d=%s\n",i,argv[i]);	
    }
#endif
    ret = rte_eal_init(argc, (char**)argv);
    if (ret < 0)
    {
        rte_panic("Cannot init EAL\n");
    }
    m_isInit = true;

    /* create the mbuf pool */
    packet_pktmbuf_pool = rte_pktmbuf_pool_create("mbuf_pool", NB_MBUF, 250,
        0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
    if (packet_pktmbuf_pool == NULL)
        rte_exit(EXIT_FAILURE, "Cannot init mbuf pool\n");

    nb_ports = rte_eth_dev_count();
    printf("nb_ports=%d\n", nb_ports);
    if (nb_ports == 0)
    {
        return 0;
    }

    if (nb_ports > RTE_MAX_ETHPORTS)
        nb_ports = RTE_MAX_ETHPORTS;

    //last_port = 0;
    nb_lcores = rte_lcore_count();

    rx_lcore_id = 0;
    qconf = NULL;

    /* Initialize the port/queue configuration of each logical core */
    for (portid = 0; portid < nb_ports; portid++) {
        /* get the lcore_id for this port */
        while (rte_lcore_is_enabled(rx_lcore_id) == 0 ||
            lcore_queue_conf[rx_lcore_id].n_rx_port ==dpdkpacket_rx_queue_per_lcore) {
            rx_lcore_id++;
            if (rx_lcore_id >= RTE_MAX_LCORE)
            {
                return -1;
            }
        }

        if (qconf != &lcore_queue_conf[rx_lcore_id])
        /* Assigned a new logical core in the loop above. */
            qconf = &lcore_queue_conf[rx_lcore_id];
        qconf->rx_port_list[qconf->n_rx_port] = portid;
        qconf->n_rx_port++;
        printf("Lcore %u: RX port %u\n", rx_lcore_id, (unsigned) portid);
    }
   
   n_tx_queue = nb_lcores;

    /* Initialise each port */
    for (portid = 0; portid < nb_ports; portid++) {
        /* init port */
        printf("Initializing port %d on lcore %u...\n", portid,rx_lcore_id);
        fflush(stdout);
        rte_eth_dev_info_get(portid, &dev_info);
        printf("Initializing port %d on lcore %u..., max_tx_q=%d\n", portid,rx_lcore_id, dev_info.max_tx_queues);

        if (n_tx_queue > dev_info.max_tx_queues)
            n_tx_queue = dev_info.max_tx_queues;
        
        ret = rte_eth_dev_configure(portid, 1, (uint16_t)n_tx_queue, &port_conf);
        if (ret < 0) {
            printf("rte_eth_dev_configure, error=%d, n_tx_queue=%d\n",ret, n_tx_queue);
            return -1;
        }

        /* init one RX queue */
        ret = rte_eth_rx_queue_setup(portid, 0, nb_rxd, rte_eth_dev_socket_id(portid),NULL,packet_pktmbuf_pool);
        if (ret < 0) {
            printf("rte_eth_rx_queue_setup, error=%d\n",ret);
            return -1;
        }
    }

    for (portid = 0; portid < nb_ports; portid++) {
        /* init one TX queue per couple (lcore,port) */
        queueid = 0;
        for (lcore_id = 0; lcore_id < n_tx_queue; lcore_id++) {
            if (rte_lcore_is_enabled(lcore_id) == 0)
                continue;

            printf("txq=%u,%d ", lcore_id, queueid);
            fflush(stdout);

            rte_eth_dev_info_get(portid, &dev_info);
            ret = rte_eth_tx_queue_setup(portid, queueid, nb_txd,rte_lcore_to_socket_id(lcore_id), NULL);
            if (ret < 0) {
                printf("\n");
                printf("rte_eth_tx_queue_setup: err=%d, port=%d\n", ret, portid);
                return -1;
            }

            qconf = &lcore_queue_conf[lcore_id];
            qconf->tx_queue_id[portid] = queueid;
            queueid++;
        }

    }

    /* start ports */
    for (portid = 0; portid < nb_ports; portid++) {
        /* Start device */
        ret = rte_eth_dev_start(portid);
        if (ret < 0)
        {
            printf("rte_eth_dev_start: err=%d, port=%d\n",ret, portid);
            return -1;
        }
        rte_eth_promiscuous_enable(portid);
    }

    for (portid = 0; portid < nb_ports; portid++) {
        /* Start device */
        rte_eth_macaddr_get(portid,&l2fwd_ports_eth_addr[portid]);
        
        printf("Port %u, MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n\n",
                (unsigned) portid,
                l2fwd_ports_eth_addr[portid].addr_bytes[0],
                l2fwd_ports_eth_addr[portid].addr_bytes[1],
                l2fwd_ports_eth_addr[portid].addr_bytes[2],
                l2fwd_ports_eth_addr[portid].addr_bytes[3],
                l2fwd_ports_eth_addr[portid].addr_bytes[4],
                l2fwd_ports_eth_addr[portid].addr_bytes[5]);
        struct rte_eth_dev_info info;
        ::memset(&info, 0, sizeof(info));
        
        rte_eth_dev_info_get(portid, &info);
        char namedev[IF_NAMESIZE]={0};
        if_indextoname(info.if_index,namedev);

        m_psink->OnPortInfo(portid, (char *)l2fwd_ports_eth_addr[portid].addr_bytes, namedev);

    }
    check_all_ports_link_status(nb_ports);

    m_psink->OnPortInit(nb_ports);

    /* launch per-lcore init on every lcore */
    rte_eal_mp_remote_launch(launch_one_lcore, this, CALL_MASTER);
#if 0
    RTE_LCORE_FOREACH_SLAVE(lcore_id) {
        printf("rte_eal_remote_launch, lcore_id=%d\n", lcore_id);
        rte_eal_remote_launch(launch_one_lcore, this, lcore_id);
    }
#endif
    //printf("Open end\n");
    return 0;  
}

static inline struct rte_mbuf *
tx_mbuf_alloc(struct rte_mempool *mp)
{
	struct rte_mbuf *data;

	data = rte_mbuf_raw_alloc(mp);

	return data;
}


void CDPDKPacketProcess::LCoreSendPacket(char *pkt, size_t pktsize, uint8_t port, uint32_t lcore_id)
{
    if (packet_pktmbuf_pool == NULL)
    {
        return;
    }
    struct rte_mbuf  *pktbuf = tx_mbuf_alloc(packet_pktmbuf_pool);
    if (!pkt)
    {
        return;
    }
    pktbuf->data_len = pktsize;
    pktbuf->next = NULL;
    char *dakt = rte_pktmbuf_mtod(pktbuf, char *);
    ::memcpy(dakt, pkt, pktsize);

    pktbuf->nb_segs        = 1;
    pktbuf->pkt_len        = pktsize;

    //qconf = &lcore_queue_conf[lcore_id];
    
    //qconf->txbuff.m_table[0] = pkt;

    send_one_packet(pktbuf, port, lcore_id);


    //rte_pktmbuf_free(pkt); 
}

uint32_t CDPDKPacketProcess::GetCurrentLcoreId()
{
    return rte_lcore_id();
}

CAWString CDPDKPacketProcess::GetPortName(uint8_t portid)
{
    struct rte_eth_dev_info dev_info;
    ::memset(&dev_info, 0, sizeof(dev_info));
    rte_eth_dev_info_get(portid, &dev_info);
    char namedev[IF_NAMESIZE]={0};
    if_indextoname(dev_info.if_index,namedev);
    CAWString str(namedev);
    return str;
}

void CDPDKPacketProcess::GetPortMac(uint8_t portid, char macaddr[6])
{
    struct ether_addr eth_addr;
    ::memset(&eth_addr, 0, sizeof(eth_addr));
    rte_eth_macaddr_get(portid,&eth_addr);
    ::memcpy(macaddr, &eth_addr, 6);
}

void CDPDKPacketProcess::OtherThreadSendPacket(char *packet, size_t pktsize, uint8_t port)
{
    DPDKSendData1(packet, pktsize, port);
}

void CDPDKPacketProcess::DPDKSendData1(char *packet, size_t pktsize, uint8_t port)
{
    unsigned ret = 0;
    unsigned queueid =0;

    static unsigned lcore_id = 0;
    unsigned len = 0;
    struct lcore_queue_conf *qconf = NULL;;

    if (packet_pktmbuf_pool == NULL)
    {
        return;
    }
    struct rte_mbuf  *pkt = NULL;
    int nb_lcores = rte_lcore_count();
    qconf = &lcore_queue_conf[lcore_id];

    pkt = tx_mbuf_alloc(packet_pktmbuf_pool);
    if (!pkt)
    {
        return;
    }
    pkt->data_len = pktsize;
    pkt->next = NULL;
    char *dakt = rte_pktmbuf_mtod(pkt, char *);
    ::memcpy(dakt, packet, pktsize);

    pkt->nb_segs        = 1;
    pkt->pkt_len        = pktsize;

    //qconf = &lcore_queue_conf[lcore_id];
    
    //qconf->txbuff.m_table[0] = pkt;

    rte_spinlock_trylock(&txlock);

    //rte_eth_tx_burst(port, (uint16_t) 0, &pkt, (uint16_t)1);

    //printf("send_packet, coreid=%d, port=%d\n",lcore_id, port);

    qconf = &lcore_queue_conf[lcore_id];
    len = qconf->tx_mbufs[port].len;
    qconf->tx_mbufs[port].m_table[len] = pkt;
    len++;

    /* enough pkts to be sent */
    if (unlikely(len == MAX_PKT_BURST)) {
        send_burst(qconf, MAX_PKT_BURST, port);
        len = 0;
    }

    qconf->tx_mbufs[port].len = len;

    rte_spinlock_unlock(&txlock);

    lcore_id++;

    if (lcore_id >= nb_lcores)
    {
        lcore_id = 0;
    }

    //printf("l2fwd_send_packet, coreid=%d, port=%d, result=%d\n",lcore_id, portid, result);

    //rte_pktmbuf_free(pkt);

}

void CDPDKPacketProcess::DPDKSendData2(char *packet, size_t pktsize, uint8_t portid)
{
    unsigned ret = 0;
    unsigned queueid =0;

    static unsigned lcore_id = 0;
    unsigned len = 0;
    struct lcore_queue_conf *qconf;

    if (packet_pktmbuf_pool == NULL)
    {
        return;
    }
    struct rte_mbuf  *pkt = NULL;
    int nb_lcores = rte_lcore_count();

    pkt = tx_mbuf_alloc(packet_pktmbuf_pool);
    if (!pkt)
    {
        return;
    }
    pkt->data_len = pktsize;
    pkt->next = NULL;
    char *dakt = rte_pktmbuf_mtod(pkt, char *);
    ::memcpy(dakt, packet, pktsize);

    pkt->nb_segs        = 1;
    pkt->pkt_len        = pktsize;

    //qconf = &lcore_queue_conf[lcore_id];

    //qconf->txbuff.m_table[0] = pkt;

    uint16_t result = rte_eth_tx_burst(portid, (uint16_t) 0, &pkt, (uint16_t)1);

    //printf("l2fwd_send_packet, coreid=%d, port=%d, result=%d\n",lcore_id, portid, result);

    rte_pktmbuf_free(pkt);

}

void CDPDKPacketProcess::Close()
{
#if 0
    unsigned lcore_id = 0;
    rte_spinlock_trylock(&global_flag_stru_p->lock);
    printf("CDPDKPacketProcess::Close, LcoreMainIsRunning=%d\n",global_flag_stru_p->LcoreMainIsRunning);

    if (global_flag_stru_p->LcoreMainIsRunning == 0)    {
        rte_spinlock_unlock(&global_flag_stru_p->lock);
        printf("CDPDKPacketProcess::Close. return\n");
        return;
    }
    global_flag_stru_p->LcoreMainIsRunning = 0;
    
    RTE_LCORE_FOREACH_SLAVE(lcore_id) {
        if (rte_eal_wait_lcore(lcore_id) < 0)
        {
            rte_spinlock_unlock(&global_flag_stru_p->lock);
            
            printf("rte_eal_wait_lcore exit\n");
            return;
        }
    }
    rte_spinlock_unlock(&global_flag_stru_p->lock);
#endif
    //printf("CDPDKPacketProcess::Close end\n");
}



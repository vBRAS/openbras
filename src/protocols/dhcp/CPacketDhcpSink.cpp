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
#include "CDHCPServer.h"
#include "CDHCPSession.h"
#include "CPacketDhcpSink.h"

CPacketDhcpSink:: CPacketDhcpSink()
	:m_maxport(0)
{
    m_DHCPPortList.clear();
    m_CDhcpServer = new CDHCPServer();
}
CPacketDhcpSink::~CPacketDhcpSink()
{
    delete m_DhcpServer;
    //if(!m_DHCPPortList.empty)	
}
void CPacketDhcpSink::OnPortInfo(uint8_t portid,char mac [ 6 ],char * devname)
{
    sDHCPPort sPort;
    sPort.id = portid; 
    memcpy(sPort.mac,mac,sizeof(mac));
    memcpy(sPort.name,devname,sizeof(devname));
    // need check exist.fixed in future.
    m_DHCPPortList.push_back(sPort);
}
void CPacketDhcpSink::OnPortInit(uint8_t maxport)
{
    m_maxport=maxport;
}
struct udp_hdr*  CPacketDhcpSink::getDHCPPacket(struct rte_mbuf * m)
{
	  if (!RTE_ETH_IS_IPV4_HDR(m->packet_type))
	  {
	     ACE_DEBUG (LM_DEBUG,"CPacketDhcpSink::received	is not IPv4 packet\n");
	     return NULL;
		}
    struct ether_hdr *eth_hdr;
    struct ipv4_hdr *ipv4_hdr; 
    struct udp_hdr *udp;
	  uint16_t ether_type, offset=0;
    
    eth_hdr = rte_pktmbuf_mtod(m, struct ether_hdr *);
    ether_type = eth_hdr->ether_type;

    if (ether_type == rte_cpu_to_be_16(ETHER_TYPE_VLAN))
        printf("VLAN taged frame, offset:");
    offset = get_vlan_offset(eth_hdr, &ether_type);
    if (offset > 0)
        printf("%d\n", offset);
	 
    if (ether_type == rte_cpu_to_be_16(ETHER_TYPE_IPv4))
    {
        ipv4_hdr = (struct ipv4_hdr *)((char *)(eth_hdr + 1) + offset);
        if(ipv4_hdr->next_proto_id != IPPROTO_UDP)
		   	    return NULL;
        udp = (struct udp_hdr *)((unsigned char *)ipv4_hdr + sizeof(struct ipv4_hdr));
        if(rte_be_to_cpu_16(udp->src_port)!=BOOTP_REPLAY_PORT || rte_be_to_cpu_16(udp->dst_port)!=BOOTP_REQUEST_PORT)
	          return NULL;
            
        return udp;
    }
    return NULL;	 
}
void CPacketDhcpSink::OnLCorePacket(char * pkt,size_t pktsize,uint8_t port,uint32_t lcore_id)
{
    bool find = false;
    DHCPPortList::iterator itr;
    if(m_DHCPPortList.empty())
	      return;
    for(itr=m_DHCPPortList.begin();itr!=m_DHCPPortList.end();itr++)
    {
        if(*itr.id == port)
        {
            find = true;
			      break;
        }
    }
    if(!find) 
        return;	   
    //lcore_id

    //handle pkt
    struct udp_hdr *udp_hdr;   
    unsigned char *payload;	
    size_t len;	 
	  udp_hdr=  getDHCPPacket((struct rte_mbuf *)pkt);
    if(udp_hdr)
    {
        payload = (unsigned char *)udp_hdr+sizeof( struct udp_hdr);	
        len = udp_hdr->dgram_len-sizeof( struct udp_hdr);	  
	      if(m_CDhcpServer)
	      {
            m_CDhcpServer->HandleDHCP(payload,len);
	      }
	      else
	      {
	          cout<<"m_DhcpServer is NULL\n";
	          ACE_DEBUG (LM_ERROR,"m_DhcpServer is NULL\n");
	      }
    }
    
    return;
}

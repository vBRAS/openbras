#include "COpenBRASAppChannel.h"
#include "openbraspdu.h"

#include "CAWACEWrapper.h"
#include "COpenBRASProcess.h"


COpenBRASAppChannel::COpenBRASAppChannel(uint32_t id, CAWAutoPtr<IDIPCTransport> &transport, COpenBRASProcess *pprocess)
    :m_id(id)
    ,m_pprocess(pprocess)
{
    SetTransport(transport);
}

COpenBRASAppChannel::~COpenBRASAppChannel()
{
}
size_t COpenBRASAppChannel::HandleMessage(const char *rcvdata,size_t datasize)
{
    if (datasize <= sizeof(OpenBRASPDUHead))
    {
        return 0;
    }

    OpenBRASPDUHead *phead = (OpenBRASPDUHead *)rcvdata;

    uint16_t pdulen = ntohs(phead->pdulenth);
    uint16_t pdutype = ntohs(phead->pdutype);

    if (pdulen > datasize)
    {
        return 0;
    }
    //todo here

    return pdulen;

}

void COpenBRASAppChannel::OnPeerDisconnect()
{
    if (m_pprocess)
    {
        m_pprocess->RemoveAppChannel(m_id);
    }
}



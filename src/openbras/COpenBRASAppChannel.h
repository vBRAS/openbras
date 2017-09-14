#ifndef COPENBRASAPPCHANNEL_H
#define COPENBRASAPPCHANNEL_H
#include "dipc.h"
#include "CAWACEWrapper.h"
#include "CDIPCTransportBase.h"

class COpenBRASProcess;
class COpenBRASAppChannel : public CDIPCTransportBase, public  CAWReferenceControlMutilThread
{
public:
    COpenBRASAppChannel(uint32_t id, CAWAutoPtr<IDIPCTransport> &transport, COpenBRASProcess *pprocess);
    virtual ~COpenBRASAppChannel();
    virtual size_t HandleMessage(const char *msg, size_t msgsize);
    void OnPeerDisconnect();
private:
    uint32_t m_id;
    COpenBRASProcess *m_pprocess;
};



#endif//COPENBRASAPPCHANNEL_H


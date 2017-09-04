/***********************************************************************
 * Copyright (C) 2013-2015, Nanjing WFNEX Technology Co., Ltd 
**********************************************************************/

#ifndef __IPACKETSERVICE_H__
#define __IPACKETSERVICE_H__
#include "CAWDefines.h"
#include "CAWUtilTemplates.h"
#include "CAWMutex.h"
#include "CAWReferenceControl.h"
#include "CAWString.h"
#include <stdint.h>
#include "dipc.h"

class IPacketServiceProcess 
{
public:
    virtual void OnPortInitDone(uint8_t maxportid) = 0;
    virtual void OnPacket(const char *pkt, size_t pktsize, uint8_t portid) = 0;
protected:
    virtual ~IPacketServiceProcess() {}
};

class IPacketService
{
public:
    static IPacketService &Instance();
    virtual int Open(IDIPCProcess *pprocess, IPacketServiceProcess *psink) = 0;
    virtual void Close() = 0;
    virtual int RequestPortPacket(uint8_t portid) = 0;
    virtual int CancelPortPacket(uint8_t portid) = 0;
    virtual int SendPacket(const char *pkt, size_t pktsize, uint8_t portid) = 0;
    virtual CAWString GetPortName(uint8_t portid) = 0;
    virtual void GetPortMac(uint8_t portid, char macaddr[6]) = 0;
    virtual void FiltePacket(uint8_t portid, 
                            char srcMac[6], 
                            char dstMac[6],
                            bool bxor) = 0;
protected:
    virtual ~IPacketService(){};
};


#endif//__IVPORTMANAGER_H__


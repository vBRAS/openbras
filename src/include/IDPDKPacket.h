/***********************************************************************
    Copyright (C) 2016-2017 Nanjing StarOS Technology Co., Ltd 
**********************************************************************/

#ifndef IDPDKPACKET_H
#define IDPDKPACKET_H

#include "CAWACEWrapper.h"


class IDPDKPacketProcessSink 
{
public:
    virtual void OnPortInit(uint8_t maxport) = 0;
    virtual void OnPortInfo(uint8_t portid, char mac[6], char *devname) = 0;
    virtual void OnLCorePacket(char *pkt, size_t pktsize, uint8_t port, uint32_t lcore_id) = 0;
protected:
    virtual ~IDPDKPacketProcessSink(){}
};

class IDPDKPacketProcess
{
public:
    static IDPDKPacketProcess &Instance();
    virtual int Open(IDPDKPacketProcessSink *psink) = 0;
    virtual void LCoreSendPacket(char *pkt, size_t pktsize, uint8_t port, uint32_t lcore_id) = 0;
    virtual void OtherThreadSendPacket(char *pkt, size_t pktsize, uint8_t port) = 0;
    virtual uint32_t GetCurrentLcoreId() = 0;
    virtual CAWString GetPortName(uint8_t portid) = 0;
    virtual void GetPortMac(uint8_t portid, char macaddr[6]) = 0;
    virtual void Close() = 0;
protected:
    virtual ~IDPDKPacketProcess(){}
};



#endif//IOAM



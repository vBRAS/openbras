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

#ifndef CPPPOEPROCESS_H
#define CPPPOEPROCESS_H
#include "CAWACEWrapper.h"
#include "IStarLang.h"
#include "dipc.h"
#include "ISYSDBManager.h"
#include "CAWACEWrapper.h"
#include "CAWIDAllocator.h"
#include "openbrasjno.h"
class CPPPOEProcess : public IDIPCProcessSink,
                                    public IDIPCAcceptorConnectorSink
{
public:
    CPPPOEProcess ();
    virtual ~CPPPOEProcess ();

    virtual void OnBootOK();
    virtual void OnProcessUpdateState(const CDIPCProcess &updateprocess);

    virtual void OnProcessRun(int argc, char** argv, IDIPCProcess *dipcProcess);
    virtual void OnHAProcessConnected(uint32_t nPeerClusterId, 
                                            uint32_t nPeerDataCenterId, 
                                            CAWAutoPtr<IDIPCTransport> &transport);

    virtual void OnConnectIndication(
                CAWResult aReason,
                IDIPCTransport *aTrpt,
                IDIPCAcceptorConnectorId *aRequestId);

private:
    IDIPCProcess *m_dipcProcess;
    CAWAutoPtr < IDIPCConnector> m_connector;
	CAWAutoPtr<IDIPCTransport> m_transport;
};
#endif //CPPPOEPROCESS_H


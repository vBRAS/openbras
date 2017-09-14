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

#include "CIPOEProcess.h"
#include "CAWByteStream.h"
#include "json_features.h"
#include "json_value.h"
#include "json_reader.h"
#include <fstream>
#include "openbraspdu.h"
CIPOEProcess::CIPOEProcess () 
    :m_dipcProcess(NULL)
{
    CAW_INFO_TRACE("CIPOEProcess::CIPOEProcess");
}

CIPOEProcess::~CIPOEProcess () 
{
    CAW_INFO_TRACE("CIPOEProcess::~CIPOEProcess");
}

void CIPOEProcess::OnBootOK (void) {
    CAW_INFO_TRACE("CIPOEProcess::OnBootOK");
}
void CIPOEProcess::OnProcessUpdateState(const CDIPCProcess &updateprocess)
{

}

void CIPOEProcess::OnProcessRun (int argc, char** argv, IDIPCProcess *dipcProcess) {
    CAW_INFO_TRACE("CIPOEProcess::OnProcessRun");
    m_dipcProcess = dipcProcess;
    dipcProcess->CreateClient(m_connector);
    if (m_connector)
    {
        m_connector->AsycConnect(this, OPENBRAS_JNO_OPENBRAS, 1);
    }

    if (m_dipcProcess)
    {
        m_dipcProcess->ProcessRunFinishNotify();
    }
}


void CIPOEProcess::OnConnectIndication(
    CAWResult aReason,
    IDIPCTransport *aTrpt,
    IDIPCAcceptorConnectorId *aRequestId)
{
    if (aReason==CAW_OK)
    {
        m_transport = CAWAutoPtr<IDIPCTransport>(aTrpt);
    }

}

void CIPOEProcess::OnHAProcessConnected(uint32_t nPeerClusterId, 
                                            uint32_t nPeerDataCenterId, 
                                                CAWAutoPtr<IDIPCTransport> &transport) 
{

}
                                              

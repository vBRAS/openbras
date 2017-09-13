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

#include "COpenBRASProcess.h"
#include "CAWByteStream.h"
#include "json_features.h"
#include "json_value.h"
#include "json_reader.h"
#include <fstream>
COpenBRASProcess::COpenBRASProcess () 
    :m_dipcProcess(NULL)
{
    CAW_INFO_TRACE("COpenBRASProcess::COpenBRASProcess");
}

COpenBRASProcess::~COpenBRASProcess () 
{
    CAW_INFO_TRACE("COpenBRASProcess::~COpenBRASProcess");
}

void COpenBRASProcess::OnBootOK (void) {
    CAW_INFO_TRACE("COpenBRASProcess::OnBootOK");
    //printf("COpenBRASProcess::OnBootOK start\n");
#if 0
    std::list<CDIPCProcess> applist;
    if (m_dipcProcess)
    {
        m_dipcProcess->GetProcessList(applist);
    }

    for(CDIPCProcess &updateprocess:applist)
    {
        Json::Value root;
        XMLParams params;
        updateprocess.ToJson(root);

        Json::Value::Members members(root.getMemberNames());
        
        for (Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it)  {
            const std::string &key = *it;
            if (!root[key].isNull())
            {
                if (root[key].isString())
                {
                    std::string value = root[key].asString();
                    params[CAWString(key.c_str())]=CAWString(value.c_str());
                }
            }
        }
        CAWMessageBlock mbZero(0UL);
        CAWString srcv=params["Version"];
       // printf("Version=%s\n",srcv.c_str());
        CStarOSXMLSink::Instance()->PostXMLEvent("AddAppEvent", params, mbZero);
    }
#endif
    //printf("COpenBRASProcess::OnBootOK end\n");
}
void COpenBRASProcess::OnProcessUpdateState(const CDIPCProcess &updateprocess)
{
#if 0
    CDIPCProcess localprocess = updateprocess;

    Json::Value root;
    XMLParams params;
    localprocess.ToJson(root);

    Json::Value::Members members(root.getMemberNames());
    
    for (Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it)  {
        const std::string &key = *it;
        if (!root[key].isNull())
        {
            if (root[key].isString())
            {
                std::string value = root[key].asString();
                params[CAWString(key.c_str())]=CAWString(value.c_str());
            }
        }
    }
    CAWMessageBlock mbZero(0UL);
    if (localprocess.GetBootFlag()==0)
    {
        CStarOSXMLSink::Instance()->PostXMLEvent("AppDeactiveEvent", params, mbZero);
    }
    else 
    {
        CStarOSXMLSink::Instance()->PostXMLEvent("AppActiveEvent", params, mbZero);
    }
#endif
}

void COpenBRASProcess::OnProcessRun (int argc, char** argv, IDIPCProcess *dipcProcess) {
    CAW_INFO_TRACE("COpenBRASProcess::OnProcessRun");
    m_dipcProcess = dipcProcess;
    StartServer(m_dipcProcess);
    CAWString srcpath = m_dipcProcess->GetStringParam("StarlangPath");
    //IStarLang::Instance().StarLangMain(srcpath, this);
    //IStarLang::Instance().SetDIPCProcess(dipcProcess);

    //m_packetprocess.open

    if (m_dipcProcess)
    {
        m_dipcProcess->ProcessRunFinishNotify();
    }
}

void COpenBRASProcess::StartServer (IDIPCProcess *dipcProcess) 
{
    CAW_INFO_TRACE("COpenBRASProcess::StarServer");
    dipcProcess->CreateServer(m_acceptor);
    if (m_acceptor)
    {
        m_acceptor->StartListen(this, 1);
    }
}
void COpenBRASProcess::OnConnectIndication(
    CAWResult aReason,
    IDIPCTransport *aTrpt,
    IDIPCAcceptorConnectorId *aRequestId)
{
}

void COpenBRASProcess::OnHAProcessConnected(uint32_t nPeerClusterId, 
                                            uint32_t nPeerDataCenterId, 
                                                CAWAutoPtr<IDIPCTransport> &transport) 
{

}
void COpenBRASProcess::OnXMLLocalCall(long bundleid, 
    const CAWString &method, 
    const CAWString &injson,
    CAWString &outjson)
{
}

void COpenBRASProcess::OnXMLRemoteCall(long bundleid, 
    const CAWString & method,
    XMLParams &params,
    CAWMessageBlock &pmsgblock)
{
}

void COpenBRASProcess::OnParserXMLError(long bundleid, const CAWString &strerror)
{
}
void COpenBRASProcess::OnXmlSync(long bundleid, const CAWString &event, const CAWString &strjson)
{
}

                                                

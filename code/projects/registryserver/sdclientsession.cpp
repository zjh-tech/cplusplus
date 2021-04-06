#include "sdclientsession.h"

#include "engine/inc/log/env.h"
#include "pb/ss_base.pb.h"
#include "registrycfg.h"
#include "servicediscoveryserver.h"

bool SDClientSession::Init()
{
    return iddealer.Register(pb::service_discovery_req_id, OnHandlerUdpServiceDiscoveryReq);
}

void SDClientSession::OnEstablish(shared_ptr<SSClientSession> sess)
{
    any attach = sess->GetAttachData();
}

void SDClientSession::OnTerminate(shared_ptr<SSClientSession> sess)
{
}

void SDClientSession::OnHandlerMsg(shared_ptr<SSClientSession> sess, uint32_t msg_id, const char* attach, uint32_t attach_len, const char* data, uint32_t len)
{
    auto dealer = iddealer.Find(msg_id);
    if (dealer == nullptr)
    {
        LogWarnA("SDClientSession OnHandlerMsg Can Not Find MsgID = {}", msg_id);
        return;
    }

    dealer(sess, data, len);
}

void SDClientSession::OnBeatHeartError(shared_ptr<SSClientSession> sess)
{
}

void FillServiceDiscoveryConn(pb::service_discovery* sd_ptr, uint64_t server_id)
{
    assert(sd_ptr);

    auto iterS2S = GRegistryCfg->S2SAttrMap.find(server_id);
    if (iterS2S == GRegistryCfg->S2SAttrMap.end())
    {
        return;
    }

    auto endIter = GRegistryCfg->ConntionMap.find(iterS2S->second->ServerType);
    if (endIter == GRegistryCfg->ConntionMap.end())
    {
        return;
    }

    auto& usedMap = GServiceDiscoveryServer->used_service_map;
    for (auto iterUsed = usedMap.begin(); iterUsed != usedMap.end(); ++iterUsed)
    {
        if (iterUsed->second->WarnFlag)
        {
            continue;
        }

        auto iterUsedAttr = GRegistryCfg->S2SAttrMap.find(iterUsed->second->ServerID);
        if (iterUsedAttr == GRegistryCfg->S2SAttrMap.end())
        {
            continue;
        }
        bool isConnFlag = false;
        for (uint32_t& temp_server_type : endIter->second)
        {
            if (temp_server_type == iterUsedAttr->second->ServerType)
            {
                isConnFlag = true;
                break;
            }
        }

        if (isConnFlag)
        {
            auto conn_ptr = sd_ptr->add_conn_list();
            assert(conn_ptr);
            conn_ptr->set_server_id(iterUsedAttr->second->ServerID);
            conn_ptr->set_server_type(iterUsedAttr->second->ServerType);
            conn_ptr->set_server_type_str(iterUsedAttr->second->ServerTypeStr);
            conn_ptr->set_inter(iterUsedAttr->second->Inter);
            conn_ptr->set_outer(iterUsedAttr->second->Outer);
            conn_ptr->set_token(iterUsedAttr->second->Token);
        }
    }
}

bool OnHandlerUdpServiceDiscoveryReq(shared_ptr<SSClientSession> sess, const char* msg, uint32_t msg_len)
{
    pb::service_discovery_req req;
    PARSE_PROTO(req, msg, msg_len);

    auto AckFunc = [&](pb::service_discovery_ack& ack) { sess->SendProtoMsg(pb::service_discovery_ack_id, ack); };

    pb::service_discovery_ack ack;

    bool rebuildflag = GServiceDiscoveryServer->GetRebuildFlag();
    ack.set_rebuild_flag(rebuildflag);
    if (rebuildflag)
    {
        AckFunc(ack);
        return true;
    }

    auto iterS2S = GRegistryCfg->S2SAttrMap.find(req.server_id());
    if (iterS2S == GRegistryCfg->S2SAttrMap.end())
    {
        LogWarnA("[ServiceDiscovery] RegistryCfg Not Find ServerID={}", req.server_id());
        AckFunc(ack);
        return true;
    }

    if (iterS2S->second->Token != req.token())
    {
        LogWarnA("[ServiceDiscovery] ServerID={} Token Error", req.server_id());
        AckFunc(ack);
        return true;
    }

    ack.set_verify_flag(true);

    auto iterUsed = GServiceDiscoveryServer->used_service_map.find(req.server_id());
    if (iterUsed == GServiceDiscoveryServer->used_service_map.end())
    {
        GServiceDiscoveryServer->AddUsedService(req.server_id());
    }

    GServiceDiscoveryServer->RemoveWarnService(req.server_id());

    auto sd_info_ptr = ack.mutable_sd_info();
    assert(sd_info_ptr);
    sd_info_ptr->set_server_id(req.server_id());
    sd_info_ptr->set_s2s_inter_listen(iterS2S->second->Inter);
    sd_info_ptr->set_s2s_outer_listen(iterS2S->second->Outer);

    FillServiceDiscoveryConn(sd_info_ptr, req.server_id());

    auto iterC2S = GRegistryCfg->C2SAttrMap.find(req.server_id());
    if (iterC2S != GRegistryCfg->C2SAttrMap.end())
    {
        sd_info_ptr->set_c2s_inter_listen(iterC2S->second->Inter);
        sd_info_ptr->set_c2s_outer_listen(iterC2S->second->Outer);
        sd_info_ptr->set_c2s_max_count(iterC2S->second->MaxCount);
    }

    AckFunc(ack);
    LogInfoA("[ServiceDiscovery] Update ServerID={}", req.server_id());

    return true;
}
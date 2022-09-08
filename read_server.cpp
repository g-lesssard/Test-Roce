#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <array>
#include <rdma/rdma_cma.h>
#include <iostream>
#include <iterator>
#include "buffer_manager/StaticBuffer.h"
#include "buffer_manager/BRAMBuffer.h"

constexpr std::size_t MAX_LENGTH = 255;
constexpr uint16_t LISTEN_PORT = 6969;

struct pdata {
    uint64_t buf_va;
    uint32_t buf_rkey;
};

int main(int argc, char* argv[]) {
    StaticBuffer<uint64_t, MAX_LENGTH> recv_buffer;
    //BRAMBuffer recv_buffer(8,0);
    sockaddr_in listen_addr;

    std::string input;
    std::cout <<  "Starting string: ";
    getline(std::cin, input);
    std::cout <<  std::endl;
    input.copy((char*)recv_buffer.getAddress(), input.length());

    rdma_cm_id* listen_id;
    rdma_cm_event* event;

    auto cm_channel = rdma_create_event_channel();
    if (!cm_channel)
        return 1;
    auto err = rdma_create_id(cm_channel, &listen_id, NULL, RDMA_PS_TCP);
    if (err)
        return 1;
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(LISTEN_PORT);
    listen_addr.sin_addr.s_addr = INADDR_ANY;

    err = rdma_bind_addr(listen_id, (sockaddr*)&listen_addr);
    if (err)
        return 1;
    err = rdma_listen(listen_id, 1);
    if (err)
        return 1;
    err = rdma_get_cm_event(cm_channel, &event);
    if (err)
        return 1;
    if (event->event != RDMA_CM_EVENT_CONNECT_REQUEST)
        return 1;
    rdma_cm_id* cm_id = event->id;
    rdma_ack_cm_event(event);

    auto pd = ibv_alloc_pd(cm_id->verbs);
    if (!pd)
        return 1;
    auto comp_channel = ibv_create_comp_channel(cm_id->verbs);
    if (!comp_channel)
        return 1;
    auto cq = ibv_create_cq(cm_id->verbs, 2, NULL, comp_channel, 0);
    if (!cq)
        return 1;
    if (ibv_req_notify_cq(cq, 0))
        return 1;
    auto mr = ibv_reg_mr(pd, recv_buffer.getAddress(), recv_buffer.getMaxSize(),
                         IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ);
    if (!mr)
        return 1;

    ibv_qp_init_attr qp_attr = {};
    qp_attr.cap.max_send_wr = 1;
    qp_attr.cap.max_send_sge = 1;
    qp_attr.cap.max_recv_wr = 1;
    qp_attr.cap.max_recv_sge = 1;
    qp_attr.send_cq = cq;
    qp_attr.recv_cq = cq;
    qp_attr.qp_type = IBV_QPT_RC;

    err = rdma_create_qp(cm_id,pd, &qp_attr);
    if (err)
        return err;

    ibv_sge sge;
    ibv_recv_wr recv_wr = {};
    ibv_recv_wr* bad_recv_wr;
    pdata rep_pdata;
    rdma_conn_param conn_param = {};
    /* Post receive before accepting connection */
    sge.addr = (uintptr_t)recv_buffer.getAddress();
    sge.length = sizeof(uint32_t);
    sge.lkey = mr->lkey;
    recv_wr.sg_list = &sge;
    recv_wr.num_sge = 1;
    if (ibv_post_recv(cm_id->qp, &recv_wr, &bad_recv_wr))
        return 1;
    rep_pdata.buf_va = htobe64((uintptr_t)recv_buffer.getAddress());
    rep_pdata.buf_rkey = htonl(mr->rkey);
    conn_param.responder_resources = 1;
    conn_param.private_data = &rep_pdata;
    conn_param.private_data_len = sizeof rep_pdata;

    err = rdma_accept(cm_id, &conn_param);
    if (err)
        return err;
    err = rdma_get_cm_event(cm_channel, &event);
    if (err)
        return err;
    if (event->event != RDMA_CM_EVENT_ESTABLISHED)
        return 1;
    rdma_ack_cm_event(event);

    char * end;
    while (1) {
        input.clear();
        std::cin.clear();
        std::cout <<  "Update data: ";
        getline(std::cin, input);
        std::cout <<  std::endl;
        recv_buffer.clear();

        recv_buffer[0] = strtoull(input.c_str(), &end, 16);
        std::cout << "Updated string is: " << std::hex << recv_buffer[0] << std::endl;
    }



}
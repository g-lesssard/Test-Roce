/*
 * build:
 * cc -o client client.c -lrdmacm -libverbs
 *
 * usage:
 * client <servername> <val1> <val2>
 *
 * connects to server, sends val1 via RDMA write and val2 via send,
 * and receives val1+val2 back from the server.
 */
#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <array>
#include <iostream>
#include <unistd.h>

#include <rdma/rdma_cma.h>
enum {
    RESOLVE_TIMEOUT_MS = 5000,
};
struct pdata {
    uint64_t buf_va;
    uint32_t buf_rkey;
};

constexpr uint32_t MAX_LENGTH = 255;
constexpr uint16_t LISTEN_PORT = 6969;

int main(int argc, char* argv[]) {
    struct pdata server_pdata;
    struct rdma_event_channel* cm_channel;
    struct rdma_cm_id* cm_id;
    struct rdma_cm_event* event;
    struct rdma_conn_param conn_param = {};
    struct ibv_pd* pd;
    struct ibv_comp_channel* comp_chan;
    struct ibv_cq* cq;
    struct ibv_cq* evt_cq;
    struct ibv_mr* mr;
    struct ibv_qp_init_attr qp_attr = {};
    struct ibv_sge sge;
    struct ibv_send_wr send_wr = {};
    struct ibv_send_wr* bad_send_wr;
    struct ibv_recv_wr recv_wr = {};
    struct ibv_recv_wr* bad_recv_wr;
    struct ibv_wc wc;
    void* cq_context;
    struct addrinfo *res, *t;
    struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_STREAM};
    int n;
    std::array<uint64_t ,MAX_LENGTH> receiving_buffer = {};
    int err;

    /* Set up RDMA CM structures */
    cm_channel = rdma_create_event_channel();
    if (!cm_channel)
        return 1;
    err = rdma_create_id(cm_channel, &cm_id, NULL, RDMA_PS_TCP);
    if (err)
        return err;
    n = getaddrinfo(argv[1], "6969", &hints, &res);
    if (n < 0)
        return 1;

    /* Resolve server address and route */
    for (t = res; t; t = t->ai_next) {
        err = rdma_resolve_addr(cm_id, NULL, t->ai_addr, RESOLVE_TIMEOUT_MS);
        if (!err)
            break;
    }
    if (err)
        return err;
    err = rdma_get_cm_event(cm_channel, &event);
    if (err)
        return err;
    if (event->event != RDMA_CM_EVENT_ADDR_RESOLVED)
        return 1;
    rdma_ack_cm_event(event);
    err = rdma_resolve_route(cm_id, RESOLVE_TIMEOUT_MS);
    if (err)
        return err;
    err = rdma_get_cm_event(cm_channel, &event);
    if (err)
        return err;
    if (event->event != RDMA_CM_EVENT_ROUTE_RESOLVED)
        return 1;
    rdma_ack_cm_event(event);

    /* Create verbs objects now that we know which device to use */
    pd = ibv_alloc_pd(cm_id->verbs);
    if (!pd)
        return 1;
    comp_chan = ibv_create_comp_channel(cm_id->verbs);
    if (!comp_chan)
        return 1;
    cq = ibv_create_cq(cm_id->verbs, 2, NULL, comp_chan, 0);
    if (!cq)
        return 1;
    if (ibv_req_notify_cq(cq, 0))
        return 1;
    mr = ibv_reg_mr(pd, receiving_buffer.data(), sizeof(receiving_buffer),
                    IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_REMOTE_READ);
    if (!mr)
        return 1;
    qp_attr.cap.max_send_wr = 2;
    qp_attr.cap.max_send_sge = 1;
    qp_attr.cap.max_recv_wr = 1;
    qp_attr.cap.max_recv_sge = 1;
    qp_attr.send_cq = cq;
    qp_attr.recv_cq = cq;
    qp_attr.qp_type = IBV_QPT_RC;
    err = rdma_create_qp(cm_id, pd, &qp_attr);
    if (err)
        return err;
    conn_param.initiator_depth = 1;
    conn_param.retry_count = 7;

    /* Connect to server */
    err = rdma_connect(cm_id, &conn_param);
    if (err)
        return err;
    err = rdma_get_cm_event(cm_channel, &event);
    if (err)
        return err;
    if (event->event != RDMA_CM_EVENT_ESTABLISHED)
        return 1;
    memcpy(&server_pdata, event->param.conn.private_data, sizeof server_pdata);
    rdma_ack_cm_event(event);

    sge.addr = (uintptr_t)receiving_buffer.data();
    sge.length = MAX_LENGTH;
    sge.lkey = mr->lkey;
    send_wr.wr_id = 0;
    send_wr.opcode = IBV_WR_RDMA_READ;
    send_wr.send_flags = IBV_SEND_SIGNALED;
    send_wr.sg_list = &sge;
    send_wr.num_sge = 1;
    send_wr.wr.rdma.rkey = ntohl(server_pdata.buf_rkey);
    send_wr.wr.rdma.remote_addr = be64toh(server_pdata.buf_va);

    while (1) {
        int dummy;
        //std::cout << "Press enter to trigger next read";
        //std::cout.flush();
        //std::cin >> dummy;

        sge.addr = (uintptr_t)receiving_buffer.data();
        sge.length = MAX_LENGTH;
        sge.lkey = mr->lkey;
        send_wr.wr_id++;
        send_wr.opcode = IBV_WR_RDMA_READ;
        send_wr.sg_list = &sge;
        send_wr.num_sge = 1;
        send_wr.wr.rdma.rkey = ntohl(server_pdata.buf_rkey);
        send_wr.wr.rdma.remote_addr = be64toh(server_pdata.buf_va);

        if (ibv_post_send(cm_id->qp, &send_wr, &bad_send_wr))
            return 1;
        if (ibv_get_cq_event(comp_chan, &evt_cq, &cq_context))
            return 1;
        if (ibv_req_notify_cq(cq, 0))
            return 1;
        if (ibv_poll_cq(cq, 1, &wc) < 1)
            return 1;
        if (wc.status != IBV_WC_SUCCESS)
            return 1;


        std::cout << "Read message is: " << std::hex << receiving_buffer[0]  << std::endl;
        sleep(1);
    }
    return 0;
}
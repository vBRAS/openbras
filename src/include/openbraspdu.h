#ifndef COPENBRASPDU_H
#define COPENBRASPDU_H

#include <stdint.h>


#define OPENBRAS_PDU_INIT_MSG                  (uint16_t)(1)

struct OpenBRASPDUHead 
{
    uint16_t pdutype;
    uint16_t pdulenth;
    uint32_t xid;
};

#endif//
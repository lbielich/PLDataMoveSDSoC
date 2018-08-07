
#ifndef STREAM_DMA_PL_H
#define STREAM_DMA_PL_H

#include "ap_int.h"

#pragma SDS data sys_port(din:rx_stream_M_AXIS)
#pragma SDS data copy(din[0:num_elements])
#pragma SDS data access_pattern(din:SEQUENTIAL)

#pragma SDS data sys_port(doutmig:MIG)
#pragma SDS data mem_attribute(doutmig:NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(doutmig[0:num_elements])

void stream_dma_pl(ap_uint<64> *din, ap_uint<64>  *doutmig, int num_elements);

#endif



#ifndef PSDDR_2_STREAM_H
#define PSDDR_2_STREAM_H

#include "ap_int.h"

#pragma SDS data sys_port(dout:tx_stream_S_AXIS)
#pragma SDS data access_pattern(dout:SEQUENTIAL)
#pragma SDS data copy(dout[0:num_elements])

#pragma SDS data mem_attribute(din:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(din[0:num_elements])

void psddr_2_stream(ap_uint<64> *din,  ap_uint<64>  *dout, int num_elements);

#endif

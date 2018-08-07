#include "stream_dma_pl.h"

void stream_dma_pl( ap_uint<64> *din, ap_uint<64> *doutmig, int num_elements)
{
#pragma HLS interface axis port=din
#pragma HLS interface m_axi port=doutmig offset=direct num_read_outstanding=8 num_write_outstanding=8 max_read_burst_length=256 max_write_burst_length=256
		for(int i = 0; i < num_elements; i++) {
		#pragma HLS pipeline
			doutmig[i] = din[i];
		}
}

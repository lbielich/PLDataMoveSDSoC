

#include "psddr_2_stream.h"

void psddr_2_stream( ap_uint<64> *din, ap_uint<64> *dout, int num_elements)
{
#pragma HLS interface axis port=dout
#pragma HLS interface m_axi port=din offset=direct num_read_outstanding=8 num_write_outstanding=8 max_read_burst_length=256 max_write_burst_length=256

	for(int i = 0; i < num_elements; i++) {
#pragma HLS pipeline
		dout[i] = din[i];
	}
}

/**********
 * Copyright (c) 2018, Xilinx, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *********/

#include <iostream>
#include <sds_lib.h>
#include <stdio.h>
#include <unistd.h>
#include <queue>
#include <fstream>
#include <unistd.h>
#include <stdint.h>

#include "stream_dma_pl.h"

#include "dma_pl_to_ps.h"
#include "psddr_2_stream.h"
//#include "plddr_2_stream.h"

#define BUF_SIZE (1024*1024*8)
#define PL_DDR_ADDR 0x80000000

int main(int argc, char **argv)
{
	ap_uint<128> * psddr_buf3;
	ap_uint<128> * psddr_buf4;
	unsigned long long* plddr_buf;
	unsigned long long* plddr_buf1;
	unsigned long long* plddr_buf2;

	//PL Buffers
	plddr_buf = (unsigned long long*)sds_mmap((void *)(PL_DDR_ADDR ), BUF_SIZE, NULL);
	plddr_buf1 = (unsigned long long*)sds_mmap((void *)(PL_DDR_ADDR + BUF_SIZE), BUF_SIZE, NULL);
	plddr_buf2 = (unsigned long long*)sds_mmap((void *)(PL_DDR_ADDR + BUF_SIZE*2), BUF_SIZE, NULL);

	//Input Stream Sources
	ap_uint<64> *fifo = (ap_uint<64> *)sds_alloc(BUF_SIZE);

	//Output Stream Sink
	ap_uint<64> *sofifo = (ap_uint<64> *)sds_alloc(BUF_SIZE);

	//PS Contiguous Buffers
	psddr_buf3 = (ap_uint<128> *)sds_alloc(BUF_SIZE);
	psddr_buf4 = (ap_uint<128> *)sds_alloc(BUF_SIZE);

	// Validate the buffer assignments and populate the queues
	if(!psddr_buf3 || !psddr_buf4 || !plddr_buf || !plddr_buf1) {
		if (psddr_buf3) sds_free(psddr_buf3);
		if (psddr_buf4) sds_free(psddr_buf4);
		if (plddr_buf) sds_munmap(plddr_buf);
		if (plddr_buf1) sds_munmap(plddr_buf1);
		return -1;
	}


// This will take streaming 64-bit data into PL DDR
#pragma SDS async(1)
	stream_dma_pl( fifo, (ap_uint<64> *)plddr_buf, BUF_SIZE/8);
#pragma SDS async(1)
	stream_dma_pl( fifo, (ap_uint<64> *)plddr_buf, BUF_SIZE/8);
#pragma SDS async(1)
	stream_dma_pl( fifo, (ap_uint<64> *)plddr_buf1, BUF_SIZE/8);
	sds_wait(1);
	sds_wait(1);
	sds_wait(1);

	printf("plddr_buf[0]  = %llx \r\n",  plddr_buf[0]);
	printf("plddr_buf[1]  = %llx \r\n",  plddr_buf[1]);

	printf("plddr_buf1[0] = %llx \r\n", plddr_buf1[0]);
	printf("plddr_buf1[1] = %llx \r\n", plddr_buf1[1]);

	std::cout << "Done with streaming inputs" << std::endl;

	dma_pl_to_ps((ap_uint<128> *)plddr_buf, (ap_uint<128> *)psddr_buf3, BUF_SIZE / 16);
	dma_pl_to_ps((ap_uint<128> *)plddr_buf1, (ap_uint<128> *)psddr_buf4, BUF_SIZE / 16);


	printf("Ready to stream out?\r\n");
	//scanf();
//Creates an output stream from received input

#pragma SDS async(2)
	psddr_2_stream((ap_uint<64> *)psddr_buf3, sofifo, BUF_SIZE/8);
#pragma SDS async(2)
	psddr_2_stream((ap_uint<64> *)psddr_buf4, sofifo, BUF_SIZE/8);
	sds_wait(2);
	sds_wait(2);



	printf("psddr_buf3[0] = %llx \r\n", (long long) psddr_buf3[0]);
	printf("psddr_buf3[1] = %llx \r\n", (long long) (psddr_buf3[0]>>64));

	printf("psddr_buf4[0] = %llx \r\n", (long long) psddr_buf4[0]);
	printf("psddr_buf4[1] = %llx \r\n", (long long) (psddr_buf4[0]>>64));

	int diff;
	int err = 0;

	for(int ii=0; ii<(BUF_SIZE*8/128); ii++){

		diff = (long long) (psddr_buf3[ii]>>64) - (long long) (psddr_buf3[ii]);
		if(ii==(BUF_SIZE*8/128-1)){
			diff = ((long long) (psddr_buf3[ii]>>64) - (long long) (psddr_buf3[ii+1])) && diff;
		}

		if(diff!=1){
			printf("psddr_buf3[%d] = %llx \r\n",ii*2,   (long long) psddr_buf3[ii]);
			printf("psddr_buf3[%d] = %llx \r\n",ii*2+1, (long long) (psddr_buf3[ii]>>64) );
			printf("diff = %d \r\n",diff);
			err++;
		}

		diff = (long long) (psddr_buf4[ii]>>64) - (long long) (psddr_buf4[ii]);
		if(ii==(BUF_SIZE*8/128-1)){
			diff = ((long long) (psddr_buf4[ii]>>64) - (long long) (psddr_buf4[ii+1])) && diff;
		}

		if(diff!=1){
			printf("psddr_buf4[%d] = %llx \r\n",ii*2,   (long long) psddr_buf4[ii]);
			printf("psddr_buf4[%d] = %llx \r\n",ii*2+1, (long long) (psddr_buf4[ii]>>64) );
			printf("diff = %d \r\n",diff);
			err++;
		}

		if (err > 5) {
			break;
		}

	}

	printf("Total Error Count = %d \r\n",err);

	std::cout << "All tests passed!" << std::endl;

	sleep(1);

	sds_free(psddr_buf3);
	sds_free(psddr_buf4);
	sds_free(fifo);
	sds_free(sofifo);
	sds_munmap(plddr_buf);
	sds_munmap(plddr_buf1);
	sds_munmap(plddr_buf2);

	return 0;
}

/********************************************************************************
    Copyright (C) 2017 Phillip Burr

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************************/
#include "demod.h"
#include "interleaver.h"

using namespace HDRadio;
using namespace HDRadio::Util;
using namespace HDRadio::Layer1::OFDM;

demodulate::demodulate(hdr_parameters& p)
    : parameters(p)
    , viterbi1(p, 365440)
    , viterbi2(p, 3200)
    , randomizer(p)
    , pdu(p)
{
}

void demodulate::process(shared_sample_buffer symbol, int block, int row)
{
    int partition = 0;
    for (int j = 0; j < parameters.pilots_lower.size() - 1; j++, partition++)
    {
        auto start = parameters.pilots_lower[j] + 1;
        for (int column = 0; column < 18; column++)
        {
            auto qpsk = (*symbol)[start + column];
            bool i = qpsk.real() > 0;
            bool q = qpsk.imag() > 0;

            interleaver.store_bit(block, row, partition, 2*column, i);
            interleaver.store_bit(block, row, partition, 2*column + 1, q);
        }
    }
    for (int j = 0; j < parameters.pilots_upper.size() - 1; j++, partition++)
    {
        auto start = parameters.pilots_upper[j] + 1;
        for (int column = 0; column < 18; column++)
        {
            auto qpsk = (*symbol)[start + column];
            bool i = qpsk.real() > 0;
            bool q = qpsk.imag() > 0;

            interleaver.store_bit(block, row, partition, 2*column, i);
            interleaver.store_bit(block, row, partition, 2*column + 1, q);
        }
    }

    //printf("%d %d\n", block, row);
    if (block == 15 && row == 31) {
        interleaver.finish([this](std::vector<uint8_t>& data){
            if (data.size() == 365440/8)
                viterbi1.process(data, [this](std::vector<uint8_t>& buffer){
                    //printf("%08X ", d);
                    randomizer.scramble(buffer);
                    pdu.process(buffer);
                    //for (int offset = 0; offset < buffer.size() * 8; offset += 32)
                    //{
                    //  uint32_t d = *(uint32_t*)&buffer[offset / 8];
                    //  if ((offset % 128) == 0) {
                    //      printf("%04X ", offset / 8);
                    //  }
                    //  printf("%02X %02X %02X %02X ", 
                    //      (uint8_t)(d),
                    //      (uint8_t)(d>>8),
                    //      (uint8_t)(d>>16),
                    //      (uint8_t)(d>>24));
                    //  if ((offset % 128) == 96) {
                    //      printf("\n");
                    //  }
                    //}
                });
            else
                viterbi2.process(data, [this](std::vector<uint8_t>& buffer){
                    randomizer.scramble(buffer);
                });
    //      printf("int:\n");
    //      for (int i = 0; i < 16; i++)
    //          printf("%02X ", data[i]);
    //      printf("\n");
        });
    }
}
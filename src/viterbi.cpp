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
#include "viterbi.h"
#include <cassert>

using namespace HDRadio::Layer1::Coding;

viterbi::viterbi(hdr_parameters& p, uint32_t transfer_size)
    : parameters(p)
    , bits_decoded(0)
    , transfer_frame_size(transfer_size)
    , output_size(2 * transfer_size / 5)
    , state(state0)
    , next(state1)
{
    assert(((2 * transfer_size) % 5) == 0);
    output.resize(output_size / 8);

    states = 1 << hidden_bits;
    state0.resize(states);
    state1.resize(states);

    encode_state.resize(states*2);
    for (auto i = 0; i < encode_state.size(); i++)
    {
        encode_state[i] = calc_1_3(i);
    }
    for (auto i = 0; i < state.size(); i++)
        state[i].state = i;
}

uint8_t viterbi::calc_1_3(uint8_t state)
{
    auto g1 = parameters.parity[state & poly1];
    auto g2 = parameters.parity[state & poly2];
    auto g3 = parameters.parity[state & poly3];
    return (g3 << 2) | (g2 << 1) | g1;
}

void viterbi::process(std::vector<uint8_t>data, std::function<void(std::vector<uint8_t>&)>&& fn)
{
    assert((data.size() % 8) == 0);
    uint32_t bits_produced = 0;

    for (int i = 0; i < data.size(); i += 5)
    {
        // We're going to process 40 bits at a time.  This is a 2/5 convolutional code
        // which means we would process 5 bits at a time but processing 8 of those at
        // a time allows us to handle the bits a bit easier.
        uint64_t bits = (*(uint64_t*)&data[i]) & 0xffffffffff;

        // Now process 5 bits at a time
        for (int j = 0; j < 8; j++) {

            // METRIC transforms a bit into the metric.  If they match, the metric is 1, if they don't, the metric is 0.
#define METRIC(expected, actual_bit_index) (int)((1 ^ (expected) ^ ((bits) >> actual_bit_index)) & 1)

            {
                int metrics[] = {
                    METRIC(0, 2) + METRIC(0, 1) + METRIC(0, 0),
                    METRIC(0, 2) + METRIC(0, 1) + METRIC(1, 0),
                    METRIC(0, 2) + METRIC(1, 1) + METRIC(0, 0),
                    METRIC(0, 2) + METRIC(1, 1) + METRIC(1, 0),
                    METRIC(1, 2) + METRIC(0, 1) + METRIC(0, 0),
                    METRIC(1, 2) + METRIC(0, 1) + METRIC(1, 0),
                    METRIC(1, 2) + METRIC(1, 1) + METRIC(0, 0),
                    METRIC(1, 2) + METRIC(1, 1) + METRIC(1, 0),
                };

                butterfly(0, metrics);
                butterfly(1, metrics);
                butterfly(2, metrics);
                butterfly(3, metrics);
                butterfly(4, metrics);
                butterfly(5, metrics);
                butterfly(6, metrics);
                butterfly(7, metrics);
                butterfly(8, metrics);
                butterfly(9, metrics);
                butterfly(10, metrics);
                butterfly(11, metrics);
                butterfly(12, metrics);
                butterfly(13, metrics);
                butterfly(14, metrics);
                butterfly(15, metrics);
                butterfly(16, metrics);
                butterfly(17, metrics);
                butterfly(18, metrics);
                butterfly(19, metrics);
                butterfly(20, metrics);
                butterfly(21, metrics);
                butterfly(22, metrics);
                butterfly(23, metrics);
                butterfly(24, metrics);
                butterfly(25, metrics);
                butterfly(26, metrics);
                butterfly(27, metrics);
                butterfly(28, metrics);
                butterfly(29, metrics);
                butterfly(30, metrics);
                butterfly(31, metrics);

                std::swap(state, next); bits_produced++;
                //printf("s %d (%d, %d, %d) (%d, %d, %d, %d, %d, %d, %d, %d)\n", bits_produced, (int)(bits&1), (int)((bits>>1)&1), (int)((bits>>2)&1), metrics[0], metrics[1], metrics[2], metrics[3], metrics[4], metrics[5], metrics[6], metrics[7]);

                bits >>= 3;
            }

            {
                // This is the same as above, but punctured (meaning we do not use bit 2)
                int metrics[] = {
                    METRIC(0, 1) + METRIC(0, 0),
                    METRIC(0, 1) + METRIC(1, 0),
                    METRIC(1, 1) + METRIC(0, 0),
                    METRIC(1, 1) + METRIC(1, 0),
                    METRIC(0, 1) + METRIC(0, 0),
                    METRIC(0, 1) + METRIC(1, 0),
                    METRIC(1, 1) + METRIC(0, 0),
                    METRIC(1, 1) + METRIC(1, 0),
                };

                butterfly(0, metrics);
                butterfly(1, metrics);
                butterfly(2, metrics);
                butterfly(3, metrics);
                butterfly(4, metrics);
                butterfly(5, metrics);
                butterfly(6, metrics);
                butterfly(7, metrics);
                butterfly(8, metrics);
                butterfly(9, metrics);
                butterfly(10, metrics);
                butterfly(11, metrics);
                butterfly(12, metrics);
                butterfly(13, metrics);
                butterfly(14, metrics);
                butterfly(15, metrics);
                butterfly(16, metrics);
                butterfly(17, metrics);
                butterfly(18, metrics);
                butterfly(19, metrics);
                butterfly(20, metrics);
                butterfly(21, metrics);
                butterfly(22, metrics);
                butterfly(23, metrics);
                butterfly(24, metrics);
                butterfly(25, metrics);
                butterfly(26, metrics);
                butterfly(27, metrics);
                butterfly(28, metrics);
                butterfly(29, metrics);
                butterfly(30, metrics);
                butterfly(31, metrics);

                std::swap(state, next); bits_produced++;
                //printf("s %d (%d, %d, 2) (%d, %d, %d, %d, %d, %d, %d, %d)\n", bits_produced, (int)(bits&1), (int)((bits>>1)&1), metrics[0], metrics[1], metrics[2], metrics[3], metrics[4], metrics[5], metrics[6], metrics[7]);

                bits >>= 2;
            }

            // No point in searching for the maximum if there aren't enough bits ready to spit out
            if (state[0].path.bitpos == 128)
            {
                uint32_t best = state[0].path.get();
                uint32_t best_metric = state[0].metric;

                for (int s = 1; s < states; s++)
                {
                    uint32_t maybe_best = state[s].path.get();
                    if (state[s].metric > best_metric)
                    {
                        best = maybe_best;
                        best_metric = state[s].metric;
                    }
                }

                memcpy(&output[bits_decoded/8], &best, sizeof(uint32_t));
                bits_decoded += 32;
                if (bits_decoded == output_size) {
                    fn(output);
                    bits_decoded = 0;
                }

                //printf("%x\n", best);
            }
        }
    }

}

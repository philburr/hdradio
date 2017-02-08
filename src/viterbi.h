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
#pragma once

#include "parameters.h"
#include <memory>
#include <cstring>

namespace HDRadio {
    namespace Layer1 {
        namespace Coding {

            namespace Viterbi {
                struct path_manager {

                    path_manager() {
                        path.resize(4, 0);
                        bitpos = 0;
                    }

                    std::vector<uint32_t> path;
                    uint32_t bitpos;

                    void one(path_manager& other) {
                        path = other.path;
                        bitpos = other.bitpos;

                        path[bitpos >> 5] |= 1 << (bitpos & 31);
                        bitpos++;
                    }

                    void zero(path_manager& other) {
                        path = other.path;
                        bitpos = other.bitpos;

                        path[bitpos >> 5] &= ~(1 << (bitpos & 31));
                        bitpos++;
                    }

                    uint32_t get() {
                        uint32_t r = path[0];
                        memmove(&path[0], &path[1], sizeof(uint32_t) * (path.size() - 1));
                        path[path.size() - 1] = 0;
                        bitpos -= 32;
                        return r;
                    }

                };
            
                struct viterbi_state {
                    path_manager path;
                    uint32_t metric;
                    uint32_t state;
                };



            }

            class viterbi {
            public:
                viterbi(hdr_parameters& p, uint32_t transfer_frame_size);
                void process(std::vector<uint8_t>data, std::function<void(std::vector<uint8_t>&)>&& fn);

            protected:
                uint8_t calc_1_3(uint8_t state);

            protected:
                void butterfly(int i, int metrics[])
                {
                    auto m0 = state[i].metric + metrics[encode_state[2*i]];
                    auto m1 = state[i+32].metric + metrics[encode_state[2*i + states]];

                    if (m0 > m1)
                    {
                        next[2*i].metric = m0;
                        next[2*i].path.zero(state[i].path);
                        next[2*i].state = state[i].state;
                    }
                    else
                    {
                        next[2*i].metric = m1;
                        next[2*i].path.zero(state[i+32].path);
                        next[2*i].state = state[i+32].state;
                    }

                    auto m2 = state[i].metric + metrics[encode_state[2*i + 1]];
                    auto m3 = state[i+32].metric + metrics[encode_state[2*i + states+1]];
                    if (m2 > m3)
                    {
                        next[2*i+1].metric = m2;
                        next[2*i+1].path.one(state[i].path);
                        next[2*i+1].state = state[i].state;
                    }
                    else
                    {
                        next[2*i+1].metric = m3;
                        next[2*i+1].path.one(state[i+32].path);
                        next[2*i+1].state = state[i+32].state;
                    }
                }

                std::vector<Viterbi::viterbi_state> state0;
                std::vector<Viterbi::viterbi_state> state1;

                std::vector<Viterbi::viterbi_state>& state;
                std::vector<Viterbi::viterbi_state>& next;

            protected:
                hdr_parameters& parameters;

                const uint8_t poly1 = 0x6d;
                const uint8_t poly2 = 0x4f;
                const uint8_t poly3 = 0x57;
                const int hidden_bits = 6;

                uint32_t states;
                uint32_t bits_decoded;
                uint32_t transfer_frame_size;
                uint32_t output_size;
                std::vector<uint8_t> encode_state;
                std::vector<uint8_t> output;
            };
        }
    }
}
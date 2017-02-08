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
#include "buffer.h"

namespace HDRadio {
    namespace Layer1 {
        namespace Coding {

            namespace InterleaverEngines {

                class interleaver_engine {
                public:
                    interleaver_engine(int j, int b, int c, int m, int n, std::initializer_list<int> v);

                public:
                    int J;
                    int B;
                    int C;
                    int M;
                    int N;
                    int V[20];

                };

                class interleaver_engine1 : public interleaver_engine {
                public:
                    interleaver_engine1();

                    bool store_bit(int block, int row, int partition, int column, int bit) {
                        int bit_index = map[block][row][partition][column];
                        if (bit_index < 0)
                            return false;
                        
                        int byte_index = bit_index / 8;
                        bit_index = bit_index % 8;

                        transfer_frame[byte_index] &= ~(1 << bit_index);
                        transfer_frame[byte_index] |= ((bit & 1) << bit_index);
                        return true;
                    }

                    void finish(std::function<void(std::vector<uint8_t>&)> fn) {
                        fn(transfer_frame);
                    }

                    std::vector<std::vector<std::vector<std::vector<int>>>> map;
                private:
                    int partition(int i);
                    int block(int i, int partition);

                    std::vector<uint8_t> transfer_frame;
                };

                class interleaver_engine2 : public interleaver_engine {
                public:
                    interleaver_engine2();

                    bool store_bit(int block, int row, int partition, int column, int bit) {
                        int bit_index = map[block][row][partition][column];
                        if (bit_index < 0)
                            return false;

                        int transfer_frame_index = bit_index / b;
                        bit_index %= b;

                        int byte_index = bit_index / 8;
                        
                        bit_index = bit_index % 8;
                        byte_index = byte_index % b;

                        transfer_frames[transfer_frame_index][byte_index] &= ~(1 << bit_index);
                        transfer_frames[transfer_frame_index][byte_index] |= ((bit & 1) << bit_index);
                        return true;
                    }

                    void finish(std::function<void(std::vector<uint8_t>&)> fn) {
                        for (auto transfer_frame : transfer_frames)
                            fn(transfer_frame);
                    }

                    std::vector<std::vector<std::vector<std::vector<int>>>> map;
                private:
                    int partition(int i);
                    int block(int i);

                    int b;
                    int I0;

                    std::vector<std::vector<uint8_t>> transfer_frames;
                };

            }

            class interleaver_mp1 {
            public:
                interleaver_mp1() : needed_block(0) {}

                void store_bit(int block, int row, int partition, int column, int bit) {
                    if (block == needed_block)
                        needed_block++;

                    if (!ie1.store_bit(block, row, partition, column, bit))
                        ie2.store_bit(block, row, partition, column, bit);
                }

                void finish(std::function<void(std::vector<uint8_t>&)> fn) {
                    if (needed_block < 16) {
                        printf("Missing block!\n");
                    }
                    else {
                        ie1.finish(fn);
                        ie2.finish(fn);
                    }
                    needed_block = 0;
                }

            private:
                int needed_block;
                InterleaverEngines::interleaver_engine1 ie1;
                InterleaverEngines::interleaver_engine2 ie2;
            };

        }
    }
}
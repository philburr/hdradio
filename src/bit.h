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
#include <vector>
#include <memory>
#include <cassert>

namespace HDRadio {
    namespace Util {
        class BitReader {
        public:
            BitReader(const std::vector<uint8_t>& b) : buffer(b), partial_bits(0), index(0) {}

            uint8_t read(uint32_t bitcount)
            {
                assert(bitcount <= 8);
                uint16_t data = (buffer[index] << 8) | buffer[index + 1];
                data <<= partial_bits;
                data >>= 16 - bitcount;

                partial_bits += bitcount;
                if (partial_bits >= 8)
                {
                    index += 1;
                    partial_bits -= 8;
                }
                return (uint8_t) data;
            }

            void transfer(uint32_t bitcount, std::function<void(uint8_t, uint32_t)>&& write)
            {
                while (bitcount > 0)
                {
                    uint32_t bits = bitcount;
                    if (bits > 8)
                        bits = 8;
                    write(read(bits), bits);
                    bitcount -= bits;
                }
            }


        private:
            const std::vector<uint8_t>& buffer;
            uint32_t partial_bits;
            uint32_t index;

        };

        class BitWriter {
        public:
            BitWriter(std::vector<uint8_t>& b) : buffer(b), partial_bits(0), index(0) {}

            void write(uint8_t data, uint32_t bitcount)
            {
                assert(bitcount <= 8);

                // We write bits MSB first so adjust bits
                data <<= 8 - bitcount;
                if (partial_bits > 0)
                {
                    auto bits_to_transfer = 8 - partial_bits;
                    if (bits_to_transfer > bitcount)
                        bits_to_transfer = bitcount;

                    auto bits = data >> partial_bits;
                    buffer[index] |= bits;

                    data <<= bits_to_transfer;
                    bitcount -= bits_to_transfer;
                    partial_bits += bits_to_transfer;
                    if (partial_bits >= 8)
                    {
                        index += 1;
                        partial_bits -= 8;
                    }
                }

                if (bitcount > 0)
                {
                    buffer[index] = data;
                    partial_bits += bitcount;
                    if (partial_bits >= 8)
                    {
                        index += 1;
                        partial_bits -= 8;
                    }
                }
            }

        private:
            std::vector<uint8_t>& buffer;
            uint32_t partial_bits;
            uint32_t index;

        };


    }
}
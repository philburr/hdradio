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
#include "randomizer.h"
#include <cassert>

using namespace HDRadio;
using namespace HDRadio::Layer1::Coding;

randomizer::randomizer(hdr_parameters& p) 
    : parameters(p)
{
    random_bits.resize(2047);

    uint32_t state = 0x7fe;
    uint8_t byte = 0;
    uint32_t count = 0;
    for (int i = 0; i < 2047*8; i++)
    {
        uint8_t bit = parameters.parity[state & 0x402];
        state = ((state << 1) | bit) & 0x7ff;

        byte |= bit << count;
        count += 1;

        if (count == 8) {
            random_bits[i / 8] = byte;
            byte = 0;
            count = 0;
        }
    }
}

void randomizer::scramble(std::vector<uint8_t>& data)
{
    for (int i = 0; i < data.size(); i++)
        data[i] ^= random_bits[i % 2047];
}
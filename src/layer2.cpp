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
#include "layer2.h"
#include "bit.h"

using namespace HDRadio;
using namespace HDRadio::Util;
using namespace HDRadio::Layer2;

l2_pdu::l2_pdu(hdr_parameters& p)
    : parameters(p)
    , have_non_partial(false)

{
    control_word.resize(3);
    pdu.resize(18272);
}

static uint8_t bitrev[256];
static uint8_t crc8_tab[256];

bool crc8(const std::vector<uint8_t>& data, uint32_t offset, uint32_t count)
{
    static bool init = false;
    if (!init) {
        init = true;

        for (int i = 0; i < 256; i++) {
            uint8_t rev = ((i & 1) << 7) |
                          ((i & 2) << 5) |
                          ((i & 4) << 3) |
                          ((i & 8) << 1) |
                          ((i & 16) >> 1) |
                          ((i & 32) >> 3) |
                          ((i & 64) >> 5) |
                          ((i & 128) >> 7);
            bitrev[i] = rev;
        }

        for (int i = 0; i < 256; i++) {
            uint16_t reg = bitrev[i] << 8;
            for (int j = 0; j < 8; j++) {
                if (reg & (1 << (15 - j))) {
                    reg ^= 0x119 << (7 - j);
                }
            }
            assert((reg & 0xff00) == 0);
            crc8_tab[i] = bitrev[(uint8_t)reg];
        }
    }

    uint8_t reg = 0;
    for (auto i = 0; i < count; i++) {
        reg = crc8_tab[reg] ^ data[offset + count - i - 1];
    }

    return reg == 0xff;
}

void l2_pdu::process(const std::vector<uint8_t>& frame)
{
    BitReader br(frame);
    BitWriter bw_cw(control_word);
    BitWriter bw_pdu(pdu);

    br.transfer(116176, [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(1,      [&](uint8_t data, uint32_t bitcount) { bw_cw.write(data, bitcount); });
    br.transfer(1247,   [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });
    br.transfer(48,     [&](uint8_t data, uint32_t bitcount) { bw_pdu.write(data, bitcount); });

    // This should be in the audio layer demultiplex, but we're just gonna cheat and do it here
    auto packets = (pdu[8 + 4] >> 1) & 0x3f;
    auto offset = pdu[8 + 5] + 1;
    bool first_partial = !!((pdu[8 + 3] >> 1) & 1);
    bool last_partial = !!((pdu[8 + 3] >> 2) & 1);

    for (auto i = 0; i < packets; i++)
    {
        auto crc_offset = ((uint16_t*)&pdu[8 + 6])[i];
        auto length = crc_offset - offset;

        // Validate CRC
        if (i == 0 && first_partial && !have_non_partial)
        {
            offset = crc_offset + 1;
            continue;
        }
        if (i > 0 || i < packets - 1)
        {
            have_non_partial = true;
            partial_crc_error = false;
        }

        bool crc_ok = crc8(pdu, offset, length + 1);

        for (auto j = 0; j < length; j++)
        {
            printf("%02X ", pdu[offset + j]);
        }
        if (i < packets - 1 || !last_partial) {
            if (!crc_ok || (i == 0 && first_partial && partial_crc_error))
                printf(" *");
            printf("\n");
        }
        else if (i == packets - 1 && last_partial)
        {
            partial_crc_error = !crc_ok;
        }

        offset = crc_offset + 1;
    }
}

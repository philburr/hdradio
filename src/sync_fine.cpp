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
#include <cstring>

#include "buffer.h"
#include "sync_fine.h"

using namespace HDRadio;
using namespace HDRadio::Util;
using namespace HDRadio::Layer1::OFDM;

sync_fine::sync_fine(hdr_parameters& p)
    : parameters(p)
    , last_angles(parameters.all_pilots.size())
    , pilot_values(parameters.all_pilots.size())
    , pilot_lock(false)
{
    for (auto i = 0; i < last_angles.size(); i++)
        last_angles[i] = -3 * PI / 4;
}

int sync_fine::extract_pilot_values(const sample_buffer& symbol)
{
    int valid = 0;
    for (auto p = 0; p < parameters.pilots.size(); p++)
    {
        float new_angle = std::arg(symbol[parameters.pilots[p]]);
        float old_angle = last_angles[parameters.pilots_subcarrier_num[p]];
        last_angles[parameters.pilots_subcarrier_num[p]] = new_angle;

        auto diff = std::abs(new_angle - old_angle);

        // A bit is 1 when a phase change about 180deg occurs
        int bit = (diff > PI / 2 && diff < 3 * PI / 2) ? 1 : 0;
        auto pilot_value = pilot_values[parameters.pilots_subcarrier_num[p]];
        pilot_value <<= 1;
        pilot_value |= bit;
        pilot_values[parameters.pilots_subcarrier_num[p]] = pilot_value;

        // Check here whether or not we have valid bits.  We do this
        // by checking the hamming distance between them.
        if (parameters.bit_count[(pilot_value >> 25) ^ 0x32] < 1)
            valid++;
    }
    return valid;
}

void sync_fine::extract_pilot_bits()
{
    // First extract the BC and MODE fields
    int bc_count[16] = {0};
    int mode_count[64] = {0};

    int highest_bc_count = -1;
    int highest_bc = -1;
    int highest_mode_count = -1;
    int highest_mode = -1;

    for (auto i = 0; i < parameters.pilots.size(); i++)
    {
        auto pilot = pilot_values[parameters.pilots_subcarrier_num[i]];
        int bc = (pilot >> 12) & 15;
        int mode = (pilot >> 1) & 0x3f;

        if (parameters.parity[bc] == ((pilot>>11) & 1))
        {
            bc_count[bc] += 1;
            if (bc_count[bc] > highest_bc_count) {
                highest_bc_count = bc_count[bc];
                highest_bc = bc;
            }
        }

        if (parameters.parity[(pilot >> 1) & 0xff] == (pilot & 1))
        {
            mode_count[mode] += 1;

            if (mode_count[mode] > highest_mode_count) {
                highest_mode_count = mode_count[mode];
                highest_mode = mode;
            }
        }
    }

    mode = highest_mode;
    block_count = highest_bc;
    if (mode != 1 && mode != -1) {
        printf("This program only has support for MP1 transmissions\n");
        exit(0);
    }
}

static inline uint32_t differential(int previous, uint32_t bits)
{
    previous &= 1;

    uint32_t res = 0;
    for (int i = 0; i < 32; i++)
    {
        int next = previous ^ ((bits >> (31 - i)) & 1);
        res <<= 1;
        res |= next;
        previous = next;
    }
    return res;
}

void sync_fine::fine_tune_partition(Util::shared_sample_buffer symbol, std::vector<float>& errors, std::vector<int>& pilots_in_partition, std::vector<int>& pilots_subcarrier_num)
{
    for (auto i = 0; i < pilots_in_partition.size() - 1; i++)
    {
        auto start = pilots_in_partition[i];
        auto low_phase = errors[pilots_subcarrier_num[i]];
        auto high_phase = errors[pilots_subcarrier_num[i]+1];

        low_phase = fmod(low_phase + 2*PI, 2*PI);
        high_phase = fmod(high_phase + 2*PI, 2*PI);
        if (high_phase - low_phase > PI)
            low_phase += PI * 2;
        if (low_phase - high_phase > PI)
            high_phase += PI * 2;

        std::complex<float> low(cosf(low_phase), sinf(low_phase));
        std::complex<float> high(cosf(high_phase), sinf(high_phase));

        if (i == 0)
            (*symbol)[start] *= low;
        (*symbol)[start + 19] *= high;

        auto delta = std::complex<float>(cosf((high_phase - low_phase) / 19), sinf((high_phase - low_phase) / 19));
        auto adj = low;

        for (auto j = 1; j < 19; j++)
        {
            adj *= delta;
            (*symbol)[start + j] *= adj;
        }
    }
}

void sync_fine::fine_tune(Util::shared_sample_buffer symbol, std::vector<float>& errors)
{
    // FIXME: this needs to adjust to mode
    fine_tune_partition(symbol, errors, parameters.pilots_lower, parameters.pilots_lower_subcarrier_num);
    fine_tune_partition(symbol, errors, parameters.pilots_upper, parameters.pilots_upper_subcarrier_num);
}

void sync_fine::fine_correction(std::function<void(shared_sample_buffer, int block, int row)>&& fn)
{
    int fixed_pilot_values[61] = {0};

    // Correcting for errors in the pilots is important because
    // a bit error can cause compounding errors in the data partitions
    // around it.
    uint32_t bit_errors = 0;
    for (auto i = 0; i < parameters.pilots.size(); i++)
    {
        auto received_value = pilot_values[parameters.pilots_subcarrier_num[i]];

        // This has a couple assumptions
        uint32_t actual_value = 0x64400700 | 
            (parameters.pilots_subcarrier_id[i] << 20) | 
            (parameters.parity[parameters.pilots_subcarrier_id[i]] << 18) | \
            ((block_count & 0xf) << 12) | 
            (parameters.parity[block_count & 0x0f] << 11) | 
            ((mode & 0x3f) << 1) | 
            (parameters.parity[mode & 0x3f] ^ 1);

        auto errors = received_value ^ actual_value;
        bit_errors += parameters.bit_count[errors & 255];
        bit_errors += parameters.bit_count[(errors >> 8) & 255];
        bit_errors += parameters.bit_count[(errors >> 16) & 255];
        bit_errors += parameters.bit_count[(errors >> 24) & 255];

        // fixed_pilot_values are differentially encoded
        fixed_pilot_values[parameters.pilots_subcarrier_num[i]] = differential(0, actual_value);
    }

    pilot_errors.resize(parameters.all_pilots.size());

    size_t symbol_index = 0;
    for (auto symbol : symbols)
    {
        for (auto pilot_index = 0; pilot_index < parameters.pilots.size(); pilot_index++)
        {
            auto pilot = fixed_pilot_values[parameters.pilots_subcarrier_num[pilot_index]];
            int bit = (pilot >> (31 - symbol_index)) & 1;

            // The bit is already differentially encoded.  So 
            float ideal_angle = (bit * 4 - 3) * PI / 4;
            float actual_angle = std::arg((*symbol)[parameters.pilots[pilot_index]]);

            pilot_errors[parameters.pilots_subcarrier_num[pilot_index]] = ideal_angle - actual_angle;
        }

        fine_tune(symbol, pilot_errors);
        fn(symbol, block_count, symbol_index);
        symbol_index++;
    }
}

void sync_fine::process(shared_sample_buffer symbol, std::function<void(shared_sample_buffer, int block, int row)>&& fn)
{
    // Fine correction involves locking onto the pilots and then correcting each carrier
    // The locking will only occur on the main pilots
    
    const sample_buffer& sym = *symbol.get();
    symbols.push_back(std::move(symbol));
    int valid = extract_pilot_values(sym);

    if (!pilot_lock) {
        if (valid > parameters.pilots.size() / 2) {
            pilot_lock = true;
            printf("Pilot lock acquired\n");

            // Remove old symbols
            while (symbols.size() > 32) {
                symbols.pop_front();
            }
        } else {
            return;
        }

    } else {
        // Not much to do until we have enough symbols
        if (symbols.size() < 32)
            return;

        if (valid < parameters.pilots.size() / 4) {
            pilot_lock = false;
            printf("Pilot lock lost\n");
            return;
        }
    }

    //printf("%lu\n", symbols.size());
    extract_pilot_bits();
    fine_correction(std::move(fn));
    symbols.clear();



}


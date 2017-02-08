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
#include "parameters.h"
#include <algorithm>

using namespace HDRadio;

hdr_parameters::hdr_parameters() 
    : FS(744187.5)
    , CP(112)
{
    // Things not specific to mode

    subcarrier_ids.resize(61);
    for (int i = -30; i < 31; i++) {
        subcarrier_ids[i + 30] = (i < 0 ? -i : i) & 3;
    }

    all_pilots.resize(61);
    for (int i = 0; i < 15; i++) {
        all_pilots[i] = -546 + 19 * i + 2048;

        if (i == 14) {
            all_pilots[15] = -279 + 2048;
            all_pilots[45] = 279;
        }
        else {
            all_pilots[i + 16] = -266 + 19 * i + 2048;
        }
        all_pilots[i + 30] = 19 * i;
        all_pilots[i + 46] = 280 + 19 * i;
    }

    window.resize(K);
    for (int i = 0; i < K; i++) {
        if (i < CP) {
            window[i] = sinf(i * PI / (2 * CP));
        } else if (i > W) {
            window[i] = sinf((2160 - i) * PI / (2 * CP));
        } else {
            window[i] = 1;
        }
    }

    bit_count.resize(65536);
    parity.resize(65536);
    for (int i = 0; i < 65536; i++) {
        int count = 0;
        int bits = i;

        while (bits != 0) {
            if (bits & 1) {
                count += 1;
            }
            bits >>= 1;
        }
        bit_count[i] = count;
        parity[i] = count & 1;
    }

    set_mode();
}

void hdr_parameters::set_mode() {

    pilots.resize(22);
    for (int i = 0; i < 11; i++) {
        pilots[i] = -546 + 19 * i + 2048;
        pilots[i + 11] = 356 + 19 * i;
    }

    pilots_subcarrier_id.resize(pilots.size());
    pilots_subcarrier_num.resize(pilots.size());
    for (int i = 0; i < pilots.size(); i++) {
        pilots_subcarrier_num[i] = std::find(all_pilots.begin(), all_pilots.end(), pilots[i]) - all_pilots.begin();
        pilots_subcarrier_id[i] = subcarrier_ids[pilots_subcarrier_num[i]];
    }

    pilots_upper.resize(11);
    pilots_upper_subcarrier_num.resize(11);
    pilots_lower.resize(11);
    pilots_lower_subcarrier_num.resize(11);
    for (int i = 0; i < 11; i++) {
        pilots_upper[i] = 356 + 19 * i;
        pilots_lower[i] = -546 + 19 * i + 2048;
        pilots_lower_subcarrier_num[i] = std::find(all_pilots.begin(), all_pilots.end(), pilots_lower[i]) - all_pilots.begin();
        pilots_upper_subcarrier_num[i] = std::find(all_pilots.begin(), all_pilots.end(), pilots_upper[i]) - all_pilots.begin();
    }

}
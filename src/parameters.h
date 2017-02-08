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
#include <complex>

namespace HDRadio {

    const double PI = 3.141592653589793;

    class hdr_parameters {
    public:
        hdr_parameters();
        void set_mode();

    public:
        const float FS;
        const int CP;
        const int W = 2048;
        const int K = 2160;
        const int sync_window = 128;

        std::vector<int> pilots_upper;
        std::vector<int> pilots_upper_subcarrier_num;

        std::vector<int> pilots_lower;
        std::vector<int> pilots_lower_subcarrier_num;

        std::vector<int> pilots;
        std::vector<int> pilots_subcarrier_num;

        std::vector<int> all_pilots;
        std::vector<int> subcarrier_ids;
        std::vector<int> pilots_subcarrier_id;

        std::vector<float> window;

        std::vector<int> bit_count;
        std::vector<int> parity;
    };
}
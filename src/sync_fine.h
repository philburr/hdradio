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
#include "buffer.h"

#include <list>

namespace HDRadio {
    namespace Layer1 {
        namespace OFDM {

            class sync_fine {
            public:
                sync_fine(hdr_parameters& p);
                void process(Util::shared_sample_buffer signal, std::function<void(Util::shared_sample_buffer, int block, int row)>&& fn);

            private:
                int extract_pilot_values(const Util::sample_buffer& symbol);
                void extract_pilot_bits();
                void fine_correction(std::function<void(Util::shared_sample_buffer, int block, int row)>&& fn);
                void fine_tune(Util::shared_sample_buffer symbol, std::vector<float>& errors);
                void fine_tune_partition(Util::shared_sample_buffer symbol, std::vector<float>& errors, std::vector<int>& pilots_in_partition, std::vector<int>& pilots_subcarrier_num);
                
            private:
                hdr_parameters& parameters;

                Util::buffer<float> last_angles;
                Util::buffer<uint32_t> pilot_values;
                std::list<Util::shared_sample_buffer> symbols;

                std::vector<float> pilot_errors;

                bool pilot_lock;
                int block_count;
                int mode;
            };
        }
    }
}
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
#include "interleaver.h"
#include "viterbi.h"
#include "randomizer.h"
#include "layer2.h"

#include <list>

namespace HDRadio {
    namespace Layer1 {
        namespace OFDM {

            class demodulate {
            public:
                demodulate(hdr_parameters& p);
                void process(Util::shared_sample_buffer symbol, int block, int row);

            private:
                
            private:
                hdr_parameters& parameters;
                Coding::interleaver_mp1 interleaver;
                Coding::viterbi viterbi1;
                Coding::viterbi viterbi2;
                Coding::randomizer randomizer;
                HDRadio::Layer2::l2_pdu pdu;
            };
        }
    }
}
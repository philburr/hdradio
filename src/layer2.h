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

namespace HDRadio {
    namespace Layer2 {

        class l2_pdu {
        public:
            l2_pdu(hdr_parameters& p);
            void process(const std::vector<uint8_t>& frame);

        private:
            hdr_parameters& parameters;
            std::vector<uint8_t> control_word;
            std::vector<uint8_t> pdu;


            // FIXME: should be part of audio parsing
            bool have_non_partial;
            bool partial_crc_error;
        };
    }
}
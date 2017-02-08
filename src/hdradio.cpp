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
#include "hdradio.h"
#include "buffer.h"

#include <fstream>
#include <iterator>
#include <complex>
#include <cstring>

extern "C" {
    #include "ffts.h"
}

using namespace HDRadio;
using namespace HDRadio::Util;
using namespace HDRadio::Layer1::OFDM;

hdradio::hdradio(const std::string& f) 
    : filename(f)
    , pool(parameters.W)
    , coarse(parameters)
    , fine(parameters)
    , demod(parameters)
{
    fft = ffts_init_1d(parameters.W, -1);

}

hdradio::~hdradio()
{
    ffts_free((ffts_plan_t*)fft);
}

void hdradio::process()
{
    deleted_unique_ptr<FILE> fp(fopen(filename.c_str(), "rb"), 
        [](FILE* fp) { fclose(fp); });

    if (!fp) {
        printf("Failed to open file\n");
        return;
    }

    sample_buffer buf(parameters.K * (parameters.sync_window + 1));

    fseek(fp.get(), 0, SEEK_END);
    auto elements = ftell(fp.get()) / (sizeof(std::complex<float>) * parameters.K * parameters.sync_window);
    fseek(fp.get(), 0, SEEK_SET);
    if (fread(&buf[0], sizeof(std::complex<float>), parameters.K, fp.get()) < parameters.K) {
        printf("Failed to read file\n");
        return;
    }


    for (auto i = 0; i < elements; i++) {
        if (fread(&buf[parameters.K], sizeof(std::complex<float>), parameters.sync_window * parameters.K, fp.get()) < parameters.sync_window * parameters.K) {
            printf("Failed to read file\n");
            return;
        }
        coarse.process(buf, [this](const sample_buffer& sig){

            auto buffer = pool.get();
            ffts_execute((ffts_plan_t*)fft, &sig[0], &(*buffer)[0]);
            fine.process(buffer, [this](shared_sample_buffer corrected, int block, int row){
                demod.process(corrected, block, row);
            });

        });

        // Move the last K because it hasn't been processed yet.
        memmove(&buf[0], &buf[parameters.sync_window * parameters.K], sizeof(std::complex<float>) * parameters.K);
    }

    //fine.clear();
}
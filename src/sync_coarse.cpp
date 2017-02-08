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
#include "sync_coarse.h"

using namespace HDRadio;
using namespace HDRadio::Layer1::OFDM;

sync_coarse::sync_coarse(hdr_parameters& p)
	: parameters(p)
	, y(parameters.K)
	, v(parameters.K)
	, h(parameters.CP)
	, sig(parameters.W)
{
	for (auto i = 0; i < h.size(); i++) {
		h[i] = sin(i * PI / parameters.CP);
	}
}

void sync_coarse::process(const Util::sample_buffer& buffer, std::function<void(const Util::sample_buffer&)>&& fn) {

	memset(&y[0], 0, sizeof(std::complex<float>) * y.size());

	for (auto i = 0; i < buffer.size() - parameters.K; i++) {
		y[i % parameters.K] += buffer[i] * std::conj(buffer[i + parameters.W]);
	}

	for (auto k = 0; k < y.size(); k++) {
		std::complex<float> sum = {0,0};
		for (auto m = 0; m < h.size(); m++) {
			sum += h[m] * y[(k+m) % parameters.K];
		}
		v[k] = sum;
	}

	size_t samperr = 0;
	float max = std::norm(v[0]);
	for (size_t i = 1; i < v.size(); i++) {
		float mag = std::norm(v[i]);
		if (mag > max) {
			max = mag;
			samperr = i;
		}
	}

	float freqerr = -parameters.FS * std::arg(v[samperr]) / (2 * PI * parameters.W);
	//printf("%lu %f\n", samperr, freqerr);

    freqerr = -2 * PI * freqerr / parameters.FS;

	for (int i = 0; i < parameters.sync_window; i++) {
		for (int w = 0; w < parameters.W; w++) {
			auto adj = std::complex<float>(cos(freqerr * (i * parameters.K + w)), sin(freqerr * (i * parameters.K + w)));
			auto samp = buffer[samperr + w + i * parameters.K];
			auto samp2 = std::complex<float>(samp.real(), samp.imag());
			sig[w] = samp2 * adj;
			sig[w] *= parameters.window[w];
			// Fold in the cyclic suffix
			if (w < parameters.CP) {
				samp = buffer[samperr + w + i * parameters.K + parameters.W];
				samp2 = std::complex<float>(samp.real(), samp.imag());
				adj =  std::complex<float>(cos(freqerr * (i * parameters.K + w + parameters.W)), sin(freqerr * (i * parameters.K + w + parameters.W)));
				auto r = (samp2 * adj) * (float)parameters.window[w + parameters.W];
				sig[w] += r;
			}
		}

		fn(sig);
	}
}
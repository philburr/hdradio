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
#include "interleaver.h"

using namespace HDRadio::Layer1::Coding;
using namespace HDRadio::Layer1::Coding::InterleaverEngines;

interleaver_engine::interleaver_engine(int j, int b, int c, int m, int n, std::initializer_list<int> v)
    : J(j), B(b), C(c), M(m), N(n)
{
    int i = 0;
    for (auto _v : v) {
        V[i++] = _v;
    }

}

interleaver_engine1::interleaver_engine1() 
    : interleaver_engine(20, 16, 36, 1, 365440, {10,2,18,6,14,8,16,0,12,4,11,3,19,7,15,9,17,1,13,5})
{
    // The interleaver engine normall maps bit offset in the transfer frame to a position in the transmission (block, row, partition, column)
    // We will be going the opposite direction.
    map.resize(B);
    for (auto& block : map)
    {
        block.resize(32);
        for (auto& part : block)
        {
            part.resize(J);
            for (auto& col : part)
            {
                col.resize(C, -1);
            }
        }
    }

    // Do the mapping
    for (int i = 0; i < N; i++) {
        auto part = partition(i);
        auto b = block(i, part);
        
        auto k = int(i / (J * B));
        auto row = (k * 11) % 32;
        auto column = (k * 11 + int(k / (32 * 9))) % C;
        map[b][row][part][column] = i;
    }

    transfer_frame.resize(N / 8);
}

int interleaver_engine1::partition(int i)
{
    return V[int((i + 2*int(M / 4)) / M) % J];
}

int interleaver_engine1::block(int i, int partition)
{
    if (M == 1)
        return (int(i / J) + (partition * 7)) % B;
    return (i + int(i / (J * B))) % B;
}


interleaver_engine2::interleaver_engine2()
    : interleaver_engine(20, 16, 36, 1, 3200, {10,2,18,6,14,8,16,0,12,4,11,3,19,7,15,9,17,1,13,5})
    , b(200)
    , I0(365440)
{
    // The interleaver engine normall maps bit offset in the transfer frame to a position in the transmission (block, row, partition, column)
    // We will be going the opposite direction.
    map.resize(B);
    for (auto& block : map)
    {
        block.resize(32);
        for (auto& part : block)
        {
            part.resize(J);
            for (auto& col : part)
            {
                col.resize(C, -1);
            }
        }
    }

    // Do the mapping
    for (int i = 0; i < N; i++) {
        auto part = partition(i);
        auto bl = block(i);
        
        auto k = (int(i/J) % (b / J)) + (I0 / (J * B));
        auto row = (k * 11) % 32;
        auto column = (k * 11 + int(k / (32 * 9))) % C;
        map[bl][row][part][column] = i;
    }

    transfer_frames.resize(N / b);
    for (auto i = 0; i < transfer_frames.size(); i++)
    {
        transfer_frames[i].resize(b);
    }
}

int interleaver_engine2::partition(int i)
{
    return V[i % J];
}

int interleaver_engine2::block(int i)
{
    return int(i / b);
}


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
#include <memory>
#include <list>

#include "aligned_allocator.h"
#include <complex>
#include <vector>

namespace HDRadio {

	template<typename T>
	using deleted_unique_ptr = std::unique_ptr<T,std::function<void(T*)>>;


	namespace Util {

		template<typename T, typename Allocator = std::allocator<T>>
		class buffer {
		public:

			typedef T value_type;
			typedef Allocator allocator_type;
			typedef std::size_t size_type;
			typedef value_type& reference;
			typedef const value_type& const_reference;

			explicit buffer( const Allocator& alloc = Allocator() ) : _buffer(alloc) {}
			explicit buffer( size_type count, const T& value = T(), const Allocator& alloc = Allocator()) : _buffer(count, value, alloc) {}

			reference operator[]( size_type pos )
			{
				return _buffer[pos];
			}
			const_reference operator[]( size_type pos ) const
			{
				return _buffer[pos];
			}

			size_type size() const { return _buffer.size(); }

		private:
			std::vector<T, Allocator> _buffer;

		};

		template<typename T, typename A>
		using shared_buffer = std::shared_ptr<buffer<T, A>>;

		template<typename T, typename Allocator = std::allocator<T>>
		class buffer_ref {
		public:
			typedef typename buffer<T, Allocator>::size_type size_type;

			buffer_ref(buffer<T, Allocator>& b, size_type start, size_type count) : _buffer(b), _start(start), _count(count) {}

		private:
			buffer<T, Allocator>& _buffer;
			size_type _start;
			size_type _count;
		};

		typedef buffer<std::complex<float>, aligned_allocator<std::complex<float>, 2*sizeof(std::complex<float>)>> sample_buffer;
		typedef shared_buffer<std::complex<float>, aligned_allocator<std::complex<float>, 2*sizeof(std::complex<float>)>> shared_sample_buffer;

		template<typename T, typename Allocator = std::allocator<T>>
		class buffer_pool {
		public:
			buffer_pool(size_t count_in_buffer) : count(count_in_buffer) {}

			shared_buffer<T, Allocator> get() {
				if (items.size() == 0) {
					return shared_buffer<T, Allocator>(new buffer<T, Allocator>(count), [this](buffer<T, Allocator>* b) {
						// The final shared_ptr is gone, instead of deleting the underlying buffer
						// make it into a unique_ptr and put it back on the list
						items.push_back(std::move(std::unique_ptr<buffer<T, Allocator>>(b)));
					});
				}
				else {
					// Get an item off the list..
					auto item = std::move(items.front());
					items.pop_front();

					return shared_buffer<T, Allocator>(item.release(), [this](buffer<T, Allocator>* b) {
						// The final shared_ptr is gone, instead of deleting the underlying buffer
						// make it into a unique_ptr and put it back on the list
						items.push_back(std::move(std::unique_ptr<buffer<T, Allocator>>(b)));
					});
				}
			}

		private:
			size_t count;
			std::list<std::unique_ptr<buffer<T, Allocator>>> items;
		};
		typedef buffer_pool<std::complex<float>, aligned_allocator<std::complex<float>, 2*sizeof(std::complex<float>)>> sample_buffer_pool;

	}
}
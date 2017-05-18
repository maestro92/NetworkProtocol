#ifndef PROTOCOL_BIT_WRITER_H_
#define PROTOCOL_BIT_WRITER_H_

#include "protocol_common.h"
#include <cstdint>


// assert.h vs cassert
// http://stackoverflow.com/questions/43011737/is-it-better-to-include-cassert-or-assert-h
#include <cassert>
// #include <assert.h>		
// cassert is more of a c++ way of doing

// uncomment to disable assert()
// #define NDEBUG


namespace protocol
{
	// assuming words are 32 bit here
	class BitWriter
	{	
		public:

			BitWriter(void* data, int bytes)
			{
				init(data, bytes);
			}


			// yojimbo_bitpack.h says
			// "Integer bit values are written to a 64 bit scratch from right to left"
			void writeBits(uint32_t value, int bits)
			{
				assert(bits > 0);
				assert(bits <= 32);
				assert( m_bitsWritten + bits <= m_numMaxBits);

				// ULL post fix on any integer means unsigned long long (at least 64bits)
				// this just ensures that "value" is in the range of [0,(1<<n)-1]. 
				// (1<<n)-1 is the largest possible value for a integer of n bits
				// example: n = 4,   (1<<4)-1 = 10000-1 = 01111
				// so if you are assuming value is bits size, then its value must be less than 01111
				assert(value <= (1ULL << bits) - 1);

				m_scratch |= uint64_t(value) << m_numScratchBits;
				m_numScratchBits += bits;

				if (m_numScratchBits >= 32)
				{
					assert(m_wordIndex < m_numMaxWords);

					// we flush the lower 32 bits of scratch to memory
					// and we convert it to little endian
					// TODO: need to finish host_to_network function
					m_data[m_wordIndex] = host_to_network(uint32_t(m_scratch & 0xFFFFFFFF));

					m_scratch >>= 32;
					m_numScratchBits -= 32;
					m_wordIndex++;
				}

				m_bitsWritten += bits;
			}



		private:
			uint64_t m_scratch;
//			int m_scratchBits;
			int m_numScratchBits;		// number of available bits in m_scratch
			int m_bitsWritten;

			uint32_t* m_data;
			int m_wordIndex;
			int m_numMaxBits;
			int m_numMaxWords;
	
			// void*:	void pointer means generic type
			//			pointer arithmetic is not possible on void pointers
			//			http://www.geeksforgeeks.org/void-pointer-c/
			//			a void pointer can hold address of any type can be typecasted to any type

			// bytes is the size of the data
			void init(void* data, int bytes)
			{
				assert(data);
				assert((bytes % 4) == 0);
				m_numMaxWords = bytes / 4;
				m_numMaxBits = bytes * 8;

				m_data = (uint32_t*)data;

				m_scratch = 0;
				m_numScratchBits = 0;
				m_bitsWritten = 0;
			}
	};
}


#endif
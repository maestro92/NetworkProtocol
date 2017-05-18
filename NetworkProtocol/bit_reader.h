#ifndef BIT_READER_H_
#define BIT_READER_H_

#include "protocol_common.h"
#include <cstdint>
#include <cassert>

namespace protocol
{
	class BitReader
	{


		public:

			// size of the buffer
			BitReader(const void* data, int bytes)
			{
				init(data, bytes);
			}

			uint32_t ReadBit(int bits)
			{
				assert(bits > 0);
				assert(bits <= 32);
				assert(m_bitsRead + bits <= m_numMaxBits);

				// if currenlty in scratch doesn have enought to
				// fullfill the user's request to read n bits
				// we fetch more from memory

				m_bitsRead += bits;

				assert(m_numScratchBits >= 0 && m_numScratchBits <= 64);

				if (m_numScratchBits < bits)
				{
					assert(m_wordIndex < m_numMaxWords);

					// we want m_scratch to be tightly packed, hence "<< m_scratchBits" instead of "<< 32"
					/*
					example, let say we have
						m_scratch = xxx
						m_scratchBits = 3;

					request comes in for 10 bits
					since scratch has not enough bits, we need to load the next word(32 bits) from memory
					we want m_scratch to be as tightly packed as possible, hence we want to place the next word
					in front of m_scratch, which becomes

						m_scratch = yyyyyyyyyyyyyyyyyyyyyyyyyxxx
						
					which is		0000000000000000000000000xxx
									yyyyyyyyyyyyyyyyyyyyyyyyy000
						|= together
					
					hence, we need to shift by scratchBits;
					
					*/
					m_scratch |= uint64_t( network_to_host(m_data[m_wordIndex])) << m_numScratchBits;
					m_numScratchBits += 32;
					m_wordIndex++;
				}

				assert(m_numScratchBits >= bits);

				// store it in temp variable
				const uint32_t temp = m_scratch & ((uint64_t(1) << bits)- 1);

				m_scratch >>= bits;
				m_numScratchBits -= bits;

				return temp;
			}



		// assuming word is 32 bit
		private:
			const uint32_t* m_data;
			uint64_t m_scratch;

			int m_numScratchBits;
			int m_numMaxBits;
			int m_numMaxBytes;

#ifndef NDEBUG
			int m_numMaxWords;	// number of words to read in the buffer. This is rounded up to the next word if necessary
#endif // #ifndef NEDEBUG


			int m_bitsRead;
			int m_wordIndex;


			// actual buffer allocated for the packet data must round up at least to the next
			// 4 bytes in memory, becuz the bit reader words from meory not bytes
			void init(const void* data, int bytes)
			{
				assert(data);

				m_data = (const uint32_t*)data;
				m_numMaxBytes = bytes;
				m_numMaxBits = bytes * 8;


				m_numMaxWords = (bytes + 3) / 4;
			}


	};


}


#endif


#include "protocol/bit_writer.h"
#include "protocol/bit_reader.h"
#include "protocol/serialize.h"
#include <string>
// stdio.h vs iostream
// http://stackoverflow.com/questions/28764438/what-the-difference-between-stdio-h-and-iostream
// if you're using C++ just use #include <iostream>
#include <iostream>


void RUN_TEST(void testFunction())
{
	testFunction();
}


/*
__FILE__, __LINE__, and __FUNCTION__ usage in C++

https://gcc.gnu.org/onlinedocs/gcc/Function-Names.html
http://stackoverflow.com/questions/597078/file-line-and-function-usage-in-c
*/

static void CheckHandler(const char * condition,
	const char * function,
	const char * file,
	int line)
{
	printf("check failed: ( %s ), function %s, file %s, line %d\n", condition, function, file, line);
#ifndef NDEBUG
#if defined( __GNUC__ )
	__builtin_trap();
#elif defined( _MSC_VER )
	__debugbreak();
#endif
#endif
	exit(1);
}

#define check( condition )                                                  \
do																			\
{																			\
	if (!(condition))                                                       \
	{                                                                       \
		CheckHandler(#condition, __FUNCTION__, __FILE__, __LINE__);         \
	}																		\
} while(0)



				



void test_bitPacker()
{
	const int bufferSize = 256;

	uint8_t buffer[bufferSize];
	memset(buffer, 0, sizeof(buffer));

	protocol::BitWriter writer(buffer, bufferSize);
	
	check(writer.getData() == buffer);
	check(writer.getBitsWritten() == 0);
	check(writer.getBytesWritten() == 0);
	check(writer.getBitsAvailable() == bufferSize * 8);

	writer.writeBits(0, 1);
	writer.writeBits(1, 1);
	writer.writeBits(10, 8);
	writer.writeBits(255, 8);
	writer.writeBits(1000, 10);
	writer.writeBits(50000, 16);
	writer.writeBits(9999999, 32);
	writer.flushBits();

	const int bitsWritten = 1 + 1 + 8 + 8 + 10 + 16 + 32;

	check(writer.getBytesWritten() == 10);
	check(writer.getBitsWritten() == bitsWritten);
	check(writer.getBitsAvailable() == bufferSize * 8 - bitsWritten);

	const int bytesWritten = writer.getBytesWritten();

	check(bytesWritten == 10);

	memset(buffer + bytesWritten, 0, bufferSize - bytesWritten);

	protocol::BitReader reader(buffer, bytesWritten);

	check(reader.getBitsRead() == 0);
	check(reader.getBitsRemaining() == bytesWritten * 8);

	uint32_t a = reader.readBits(1);
	uint32_t b = reader.readBits(1);
	uint32_t c = reader.readBits(8);
	uint32_t d = reader.readBits(8);
	uint32_t e = reader.readBits(10);
	uint32_t f = reader.readBits(16);
	uint32_t g = reader.readBits(32);

	check(a == 0);
	check(b == 1);
	check(c == 10);
	check(d == 255);
	check(e == 1000);
	check(f == 50000);
	check(g == 9999999);

	check(reader.getBitsRead() == bitsWritten);
	check(reader.getBitsRemaining() == bytesWritten * 8 - bitsWritten);
}

struct TestStreamObject : public Serializable
{


}

void test_stream()
{



}

int main()
{

	RUN_TEST(test_bitPacker);
	RUN_TEST(test_stream);

	while(1){}
	return 0;
}
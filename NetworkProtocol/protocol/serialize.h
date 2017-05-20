#ifndef SERIALIZE_H_
#define SERIALIZE_H_


/*
flow:
1.	start with the serialize function in your serializeObject

					serializeObject.serialize(writeStream)


2.	assuming, this class serializes an int, it will call the serialize_int macros

					template<typename Stream> bool Serialize(Stream& stream)
					{
						serialize_int(stream, data.a)
						serialize_int(stream, data.b)
						serialize_int(stream, data.c)
						serialize_int(stream, data.d)
						return true;
					}

	notice that this a boolean function

3.	then the macro finally calls the real function in the stream class
	If i understand correctly, in the "serialization-strategies" article, glenn says he is adding this 
	extra level of indrection for FAST bounds check and abort read, so that we are not vulnerable to malicious packets.

	"Since we can't use execpetions to handle this abort (too slow), here's how I like to do it"

	http://gafferongames.com/building-a-game-network-protocol/serialization-strategies/
	
					#define serialize_int()
					{
						...
						if(!stream.serializeInteger(int32_value, min, max))		\
							return false;
						...
					}

	"this macro returns false on error. This gives you exeception-like behavior in the sense that it unwinds the stack back to the top of the serialization
	callstack on error, but you don't pay anything like the cost of execeptions to do this. 
	
	I am assuming if Glenn didn't have the macro, he will have to do something like this:
		
					template<typename Stream> bool Serialize(Stream& stream)
					{
						if(!serialize_int(stream, data.a))
							return false;

						if(!serialize_int(stream, data.b))
							return false;

						if(!serialize_int(stream, data.c))
							return false;

						if(!serialize_int(stream, data.d))
							return false;

						return true;
					}
	
	which is literally boilerplate code. 

	Interestingly I also took a look RakNet code. In there all the bitStream (Write, Read) writeInt, readInt related functions are "void" functions.
	I am assuming Glenn decided to make these serialize function return functions becuz it allows the user(whoever is using the api) to know whether it was a successful read or write?

4.	Then finally we go into the actual read/write functions in the stream

					WriteStream.serializeInteger()
					{

					}
*/

namespace protocol
{

	#define serialize_int(stream, value, min, max)				\
	do															\
	{															\
		assert(min < max)										\
		int32_t int32_value;									\
		if(Stream::IsWriting)									\
		{														\
			assert(value >= min);								\
			assert(value <= max);								\
			int32_value = (int32_t)value;						\
		}														\
		if(!stream.serializeInteger(int32_value, min, max))		\
			return false;										\
		if (Stream::IsReading)									\
		{														\
			value = int32_value;								\
			if (value < min || value > max)						\
				return false;									\
		}														\
	} while (0);												\






	class Serializable
	{
		public:
			virtual ~Serializable() {}

			virtual bool serializeInternal(class ReadStream & stream) = 0;

			virtual bool serializeInternal(class WriteStream & stream) = 0;

			virtual bool serializeInternal(class MeasureStream & stream) = 0;

	};


	/**
		helper macro to defien virtual serialize functions for read, write and measure
	*/
	#define VIRTUAL_SERIALIZE_FUNCTIONS()																\
		bool serializeInternal( class Protocol::ReadStream & stream ) { return serialize(stream); };	\
		bool serializeInternal( class Protocol::WriteStream & stream ) { return serialize(stream); };	\
		bool serializeInternal( class Protocol::MeasureStream & stream ) { return serialize(stream); };	\

}



#endif

#ifndef PROTOCOL_COMMON_H_
#define PROTOCOL_COMMON_H_

namespace protocol
{

	// the yojimbo comments says "Supported integer types: uint64_t, uint32_t, uint16_t."
	// I am assuming he wrote this cuz c++ allows function overloading
	// otherwise if u use the c version of it, https://linux.die.net/man/3/htons
	// you have to write three different functions
	template<typename T> T host_to_network(T value)
	{
		return value;
	}


	template<typename T> T network_to_host(T value)
	{
		return value;
	}
}

#endif
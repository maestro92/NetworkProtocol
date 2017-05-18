#ifndef PROTOCOL_COMMON_H_
#define PROTOCOL_COMMON_H_

namespace protocol
{

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
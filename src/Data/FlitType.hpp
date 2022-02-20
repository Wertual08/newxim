#pragma once
#include <systemc.h>
#include <cstdint>



enum class FlitType : std::int32_t {
	None = 0b000,
	Head = 0b001,
	Body = 0b010, 
	Tail = 0b100
};
static FlitType operator|(FlitType l, FlitType r) {
	return static_cast<FlitType>(static_cast<std::int32_t>(l) | static_cast<std::int32_t>(r));
}
static FlitType operator&(FlitType l, FlitType r) {
	return static_cast<FlitType>(static_cast<std::int32_t>(l) & static_cast<std::int32_t>(r));
}
template<typename T>
static bool HasFlag(T flit, T flag) {
	return (flit & flag) == flag;
}
#ifndef _BLE_UTILS_H_
#define _BLE_UTILS_H_

#include <cstdio>

#if defined(C7222_BLE_DEBUG)
#define C7222_BLE_DEBUG_PRINT(...) std::printf(__VA_ARGS__)
#else
#define C7222_BLE_DEBUG_PRINT(...) do { } while(0)
#endif

namespace c7222 {
}

#endif	//! _BLE_UTILS_H_

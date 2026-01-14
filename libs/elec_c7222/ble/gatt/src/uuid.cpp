
#include "uuid.hpp"

namespace c7222{
std::ostream& operator<<(std::ostream& os, const Uuid& uuid) {
	std::ios_base::fmtflags flags = os.flags();
	char fill = os.fill();
	os << std::hex << std::nouppercase << std::setfill('0');
	auto type = uuid.type();
	switch(type) {
		case Uuid::Type::k16Bit:
			os << "16-bit UUID: ";
			os << "0x" << std::setw(4) << uuid.Get16Bit();
			break;
		case Uuid::Type::k128Bit:
			os << "128-bit UUID: ";
			{
				const auto& bytes = uuid.Get128Bit();
				for(size_t i = 0; i < bytes.size(); ++i) {
					os << std::setw(2) << static_cast<int>(bytes[i]);
					if(i == 3 || i == 5 || i == 7 || i == 9) {
						os << '-';
					}
				}
			}
			break;
		default:
			os << "Invalid UUID";
			break;
	}
	os.flags(flags);
	os.fill(fill);
	return os;
}
}
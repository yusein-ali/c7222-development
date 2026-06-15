#ifndef RPI_BD_BTSTACK_PORT_HPP
#define RPI_BD_BTSTACK_PORT_HPP

#include "btstack_port.hpp"

class RpiBdBtstackPort final : public c7222::BtstackPort {
  public:
	RpiBdBtstackPort();

	bool Init() override;
	void Deinit() override;
};

#endif // RPI_BD_BTSTACK_PORT_HPP

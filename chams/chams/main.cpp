#include "memory.h"

#include <thread>

namespace offset
{
	// client
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDA746C;
	constexpr ::std::ptrdiff_t dwEntityList = 0x4DC178C;

	// engine 
	constexpr ::std::uintptr_t model_ambient_min = 0x58D044;

	// entity
	constexpr ::std::ptrdiff_t m_clrRender = 0x70;
	constexpr ::std::ptrdiff_t m_iTeamNum = 0xF4;
}

struct Color
{
	std::uint8_t r{ }, g{ }, b{ };
};

int main()
{
	const auto memory = Memory{ "csgo.exe" };

	// module addresses
	const auto client = memory.GetModuleAddress("client.dll");
	const auto engine = memory.GetModuleAddress("engine.dll");

	constexpr const auto teamColor = Color{ 0, 0, 255 };
	constexpr const auto enemyColor = Color{ 255, 0, 0 };

	// hack loop
	while (true)
	{
		const auto& localPlayer = memory.Read<std::uintptr_t>(client + offset::dwLocalPlayer);
		const auto& localTeam = memory.Read<std::int32_t>(localPlayer + offset::m_iTeamNum);

		for (auto i = 1; i <= 32; ++i)
		{
			const auto& entity = memory.Read<std::uintptr_t>(client + offset::dwEntityList + i * 0x10);

			if (memory.Read<std::int32_t>(entity + offset::m_iTeamNum) == localTeam)
			{
				memory.Write<Color>(entity + offset::m_clrRender, teamColor);
			}
			else
			{
				memory.Write<Color>(entity + offset::m_clrRender, enemyColor);
			}

			// model brightness
			float brightness = 25.f;
			const auto _this = static_cast<std::uintptr_t>(engine + offset::model_ambient_min - 0x2c);
			memory.Write<std::int32_t>(engine + offset::model_ambient_min, *reinterpret_cast<std::uintptr_t*>(&brightness) ^ _this);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return 0;
}

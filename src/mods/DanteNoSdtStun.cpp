#include "DanteNoSdtStun.hpp"

static naked void detour()
{
	__asm {
		cmp byte ptr [DanteNoSdtStun::cheaton], 0
		je originalcode
		mov al, 1

		originalcode:
		movzx ecx, al
		mov rax, [rbx + 0x50]
		jmp qword ptr [DanteNoSdtStun::ret]
	}
}

std::optional<std::string> DanteNoSdtStun::on_initialize()
{
	init_check_box_info();
	auto base = g_framework->get_module().as<HMODULE>(); // note HMODULE
	ischecked = &cheaton;
	onpage = dantesdt;
	full_name_string = "No SDT finish stun";
	author_string = "VPZadov";
	description_string = "Dante wont be stunned when regular SDT mode is over.";

	auto sdtCancellableAddr = utility::scan(base, "77 FE FF 0F B6 C8 48 8B 43 50"); //DevilMayCry5.exe+1973ABE (-0x3)
	if (!sdtCancellableAddr)
	{
		return "Unanable to find DanteNoSdtStun.sdtCancellableAddr pattern.";
	}

	if (!install_hook_absolute(sdtCancellableAddr.value() + 0x3, m_is_cancellable_hook, &detour, &ret, 0x7))
	{
		spdlog::error("[{}] failed to initialize", get_name());
		return "Failed to initialize DanteNoSdtStun.sdtCancellable";
	}

	return Mod::on_initialize();
}

void DanteNoSdtStun::on_config_load(const utility::Config& cfg)
{
}

void DanteNoSdtStun::on_config_save(utility::Config& cfg)
{
}

void DanteNoSdtStun::on_frame()
{
}

void DanteNoSdtStun::on_draw_ui()
{

}

void DanteNoSdtStun::on_draw_debug_ui()
{
}

void DanteNoSdtStun::init_check_box_info()
{
	m_check_box_name = m_prefix_check_box_name + std::string(get_name());
	m_hot_key_name = m_prefix_hot_key_name + std::string(get_name());
}
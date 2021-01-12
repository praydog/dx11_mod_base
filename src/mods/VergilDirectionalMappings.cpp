
#include "VergilDirectionalMappings.hpp"

uintptr_t VergilDirectionalMappings::jmp_ret{NULL};
bool vergiltrickbackcheck;

// clang-format off
// only in clang/icl mode on x64, sorry

static naked void detour() {
	__asm {
        cmp byte ptr [vergiltrickbackcheck], 1
        je cheatcode
        jmp code

    cheatcode:
        cmp byte ptr [rdi+10h], 4
        je lockonback
        jmp code

    lockonback:
        mov edx, 1000h
        jmp raxret

    code:
        mov edx, [rdi+10h]
        // jmp raxret

    raxret:
        test rax,rax
		jmp qword ptr [VergilDirectionalMappings::jmp_ret]
	}
}

// clang-format on

std::optional<std::string> VergilDirectionalMappings::on_initialize() {
  auto base = g_framework->get_module().as<HMODULE>(); // note HMODULE
  auto addr = utility::scan(base, "8B 57 10 48 85 C0 0F 84 67");
  if (!addr) {
    return "Unable to find VergilDirectionalMappings pattern.";
  }

  if (!install_hook_absolute(addr.value(), m_function_hook, &detour, &jmp_ret, 6)) {
    //  return a error string in case something goes wrong
    spdlog::error("[{}] failed to initialize", get_name());
    return "Failed to initialize VergilDirectionalMappings";
  }
  return Mod::on_initialize();
}

void VergilDirectionalMappings::on_draw_ui() {
  ImGui::Checkbox("Vergil Trick Down On Back Style", &vergiltrickbackcheck);
}
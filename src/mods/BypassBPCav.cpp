
#include "BypassBPCav.hpp"

uintptr_t BypassBPCav::jmp_ret{NULL};
uintptr_t BypassBPCav::jmp_jb{NULL};
bool bypassbpcavcheck;

// clang-format off
// only in clang/icl mode on x64, sorry

static naked void detour() {
	__asm {
        cmp byte ptr [bypassbpcavcheck], 1
        je cheatcode
        jmp code

        cheatcode:
		jmp qword ptr [BypassBPCav::jmp_ret]

        code:
        cmp esi, [rax+1Ch]
        jb jbcode
        jmp qword ptr [BypassBPCav::jmp_ret]

        jbcode:
        jmp qword ptr [BypassBPCav::jmp_jb]
	}
}

// clang-format on

std::optional<std::string> BypassBPCav::on_initialize() {
  auto base = g_framework->get_module().as<HMODULE>(); // note HMODULE
  auto addr      = utility::scan(base, "C8 00 EB 44 3B 70 1C 72 11");
  if (!addr) {
    return "Unable to find BypassBPCav pattern.";
  }
    
   BypassBPCav::jmp_jb = utility::scan(base, "42 89 0C 30 EB 28").value();

  if (!install_hook_absolute(addr.value()+4, m_function_hook, &detour, &jmp_ret, 5)) {
    //  return a error string in case something goes wrong
    spdlog::error("[{}] failed to initialize", get_name());
    return "Failed to initialize BypassBPCav";
  }
  return Mod::on_initialize();
}

void BypassBPCav::on_draw_ui() {
  ImGui::Checkbox("Bypass BP Cav R Restriction", &bypassbpcavcheck);
}
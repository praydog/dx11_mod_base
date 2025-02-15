#pragma once
#include "ImGuiExtensions/ImGuiExtensions.h"
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
#include "utility/Patch.hpp"
#include "EnemySwapper.hpp"
class DeepTurbo : public Mod {
public:
  DeepTurbo() = default;
  // mod name string for config
  std::string_view get_name() const override { return "DeepTurbo"; }
  std::string get_checkbox_name() override { return m_check_box_name; };
  std::string get_hotkey_name() override { return m_hot_key_name; };
  // called by m_mods->init() you'd want to override this
  std::optional<std::string> on_initialize() override;
  static uintptr_t jmp_ret1;
  static uintptr_t jmp_ret2;
  static inline uintptr_t pauseBase = 0;
  static inline uintptr_t isCutsceneRet = 0;

  static bool cheaton;
  static inline bool isSpeedUpMenu = true;
  static inline bool isCutscene = false;

  static inline float turbospeed = 1.2f;
  static inline float menuSpeed = 1.6f;

  // Override this things if you want to store values in the config file
  void on_config_load(const utility::Config& cfg) override;
  void on_config_save(utility::Config& cfg) override;

  // on_frame() is called every frame regardless whether the gui shows up.
  // void on_frame() override;
  // on_draw_ui() is called only when the gui shows up
  // you are in the imgui window here.
  void on_draw_ui() override;
  // on_draw_debug_ui() is called when debug window shows up
  void on_draw_debug_ui() override; // used

private:
  // function hook instance for our detour, convinient wrapper
  // around minhook
  void init_check_box_info() override;

  std::unique_ptr<FunctionHook> m_function_hook1;
  std::unique_ptr<FunctionHook> m_function_hook2;
  std::unique_ptr<Patch> m_patch01;
  std::unique_ptr<FunctionHook> m_cutscene_hook;
};

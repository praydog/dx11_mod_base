#pragma once
#include "Mods/GameFunctions/GameFunc.hpp"
#include <optional>
//clang-format off
namespace GameFunctions
{
	class CreateShell : public GameFunc<volatile void*>
	{
		private:
			//helper
			class ShellListSetCapacity : private GameFunc<bool>
			{
			private:
				typedef bool(__cdecl* f_set_Capacity)(void* rcx, void* list, int newCapacity);
				static inline f_set_Capacity set_capacity{};
				static inline const int listOffs = 0x60;
				static inline std::mutex mt{};
				static inline uintptr_t fAddr = 0x1A0DC10;
				//CreateShell *baseRef;

				bool invoke() override
				{
					throw std::bad_function_call();
				}//Call it through parameter overload.
				bool operator()() override
				{
					return invoke();
				}

				ShellListSetCapacity(){ }

			public:
				
				static bool invoke(int capacity, uintptr_t threadContext)
				{
					bool res = false;
					set_capacity = (f_set_Capacity)(g_framework->get_module().as<uintptr_t>() + fAddr);
					if (threadContext != 0)
					{
						uintptr_t shellMng = *(uintptr_t*)(g_framework->get_module().as<uintptr_t>() + 0x7E60450);
						if (shellMng != 0)
						{
							uintptr_t lst = *(uintptr_t*)(shellMng + 0x60);
							uintptr_t mngObj = *(uintptr_t*)(lst+0x10);
							if (mngObj != 0)
							{
								int curCapacity = *(int*)(mngObj + 0x1C);
								std::unique_lock<std::mutex> lock(mt);
								if(capacity != curCapacity)
									return res = set_capacity((void*)threadContext, (void*)lst, capacity);
							}
						}
					}
					return res;
				}

				static int get_capacity()
				{
					int capacity = -1;
					uintptr_t shellMng = *(uintptr_t*)(g_framework->get_module().as<uintptr_t>() + 0x7E60450);
					if (shellMng != 0)
					{
						uintptr_t lst = *(uintptr_t*)(shellMng + listOffs);
						uintptr_t mngObj = *(uintptr_t*)(lst + 0x10);
						if (mngObj != 0)
						{
							capacity = *(int*)(mngObj + 0x1C);
						}
					}
					return capacity;
				}
				
			};

			struct SoundData
			{
				byte mngOffs[0xF] = "";
				std::string* triggerName = new std::string("____JudgementCut");
				Vec3 position;
				~SoundData()
				{
					delete triggerName;
				}
			};

			struct DelayParam
			{
				byte mngOffs[0xF] = "";
				float frame = 0.228f;
				void* lockOnTarget = nullptr;//LockOnTargetWork
				SoundData* sound = new SoundData();
				Vec3 initCollisionCheckPos;

				~DelayParam()
				{
					delete sound;
				}
			};
		
		//typedef uintptr_t(__cdecl* spawnShell)(void* rcxArg, void* shellManager, void* prefab, const Vec3 &pos, const Quaternion &rotation, void* owner, int level, int id, const void* delayParam/*, void* a5, void* a6, void *a7, void *a8, void* a9*/);
		typedef volatile void* (__cdecl* f_CreateShell)(void* rcxArg, void* shellManager, const void* prefab, Vec3 pos, Quaternion rotation, const void *owner, int level, int id, DelayParam* delay);
		f_CreateShell create_shell;

		uintptr_t pfb = 0;
		uintptr_t owner = 0;

		Vec3 pos;
		Quaternion rot;
		int lvl = 0;
		int id = 0;

		DelayParam *delay;

	public:
		

		CreateShell()
		{
			fAddr +=  0x1B0A400;
			create_shell = (f_CreateShell)fAddr;
			delay = nullptr;
			threadContext = get_thread_context();
		}

		CreateShell(uintptr_t prefab) : CreateShell()
		{
			pfb = prefab;
		}

		void set_params(uintptr_t prefab, const Vec3& spawnPos, const Quaternion& spawnRot, uintptr_t owner, int level, int id)
		{
			pfb = prefab;
			pos = spawnPos;
			rot = spawnRot;
			lvl = level;
			this->id = id;
			this->owner = owner;
		}

		uintptr_t get_prefab() const {return pfb; }

		uintptr_t get_owner() const {return owner; }

		Vec3 get_spawn_pos() const {return pos; }

		Quaternion get_spawn_rot() const {return rot; }

		int get_level() const {return lvl; }

		int get_id() const {return id; }

		bool is_shell_mng_valid() const { return *(uintptr_t*)(g_framework->get_module().as<uintptr_t>() + 0x7E60450) != 0; }

		/// <summary></summary>
		/// <returns>Return app.Shell object</returns>
		volatile void* invoke() override
		{
			volatile void* res = 0;
			if (pfb != 0 && fAddr != NULL)
			{
				uintptr_t shellMng = *(uintptr_t*)(g_framework->get_module().as<uintptr_t>() + 0x7E60450);
				if(shellMng == 0 || IsBadReadPtr((void*)shellMng, 8))
					return 0;
				threadContext = get_thread_context();
				if (threadContext == 0)
				{
					return 0;
				}
				res = create_shell((void*)threadContext, (void*)shellMng, (void*)pfb, pos, rot, (void*)owner, lvl, id, nullptr);
			}
			return res;
		}

		/// <summary></summary>
		/// <returns>Return app.Shell object</returns>
		volatile void* invoke(uintptr_t prefab, const Vec3& spawnPos, const Quaternion& spawnRot, uintptr_t owner, int level, int id)
		{
			set_params(prefab, spawnPos, spawnRot, owner, level, id);
			return invoke();
		}

		/// <summary></summary>
		/// <returns>Return app.Shell object</returns>
		volatile void* operator ()(uintptr_t prefab, const Vec3& spawnPos, const Quaternion& spawnRot, uintptr_t owner, int level, int id)
		{
			return invoke(prefab, spawnPos, spawnRot, owner, level, id);
		}

		/// <summary></summary>
		/// <returns>Return app.Shell object</returns>
		volatile void* invoke(const Vec3 &spawnPos, const Quaternion &spawnRot)
		{
			pos = spawnPos;
			rot = spawnRot;
			return invoke();
		}

		/// <summary></summary>
		/// <returns>Return app.Shell object</returns>
		volatile void* operator()(const Vec3& spawnPos, const Quaternion& spawnRot)
		{
			return invoke(spawnPos, spawnRot);
		}

		bool set_list_shell_capacity(int newCapacity)
		{
			return ShellListSetCapacity::invoke(newCapacity, get_thread_context());
		}

		int get_list_shell_capacity() const
		{
			return ShellListSetCapacity::get_capacity();
		}

		int get_list_shell_count() const
		{
			uintptr_t shellMng = *(uintptr_t*)(g_framework->get_module().as<uintptr_t>() + 0x7E60450);
			if (shellMng == 0 || IsBadReadPtr((void*)shellMng, 8))
				return -1;
			uintptr_t lst = *(uintptr_t*)(shellMng + 0x60);
			return *(int*)(lst + 0x18);
		}
	};
}
//clang-format on
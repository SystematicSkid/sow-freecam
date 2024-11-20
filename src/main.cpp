#include <iostream>
#include <Windows.h>
#include <cstdint>
#include <map>
#include <vector>
#include <thread>

#include <minhook/MinHook.h>

#include "math.hpp"
#include "camera.hpp"
#include "game_client.hpp"
#include "memory.hpp"

class FreeCam {
private:
    struct Settings {
        float movement_speed = 5.0f;
        float horizontal_smooth_factor = 0.08f;
        float vertical_smooth_factor = 0.15f;
        vec3 velocity{0.0f, 0.0f, 0.0f};
        bool is_active = false;
    };

    Settings settings;
    HWND game_window;
    WNDPROC original_wndproc;
    std::map<uintptr_t, std::vector<uint8_t>> original_bytes;
    HMODULE module_handle;
    sow::CameraTransform* camera_transform;
    
    static FreeCam* instance;

public:
    FreeCam(HMODULE module) : module_handle(module) {
        instance = this;
    }

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
        return instance->HandleWindowMessage(hwnd, msg, wparam, lparam);
    }

private:
    bool PatchGameMemory(uintptr_t address, size_t size = 5) {
        DWORD old_protect;
        if (!VirtualProtect((void*)address, size, PAGE_EXECUTE_READWRITE, &old_protect)) 
            return false;

        
        std::vector<uint8_t> original(size);
        memcpy(original.data(), (void*)address, size);
        original_bytes[address] = original;

        
        memset((void*)address, 0x90, size);
        
        VirtualProtect((void*)address, size, old_protect, &old_protect);
        return true;
    }

    void RestorePatches() {
        for (const auto& [address, bytes] : original_bytes) {
            DWORD old_protect;
            VirtualProtect((void*)address, bytes.size(), PAGE_EXECUTE_READWRITE, &old_protect);
            memcpy((void*)address, bytes.data(), bytes.size());
            VirtualProtect((void*)address, bytes.size(), old_protect, &old_protect);
        }
    }

    LRESULT HandleWindowMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
        if (settings.is_active && msg == WM_MOUSEWHEEL) {
            int delta = GET_WHEEL_DELTA_WPARAM(wparam);
            settings.movement_speed += (delta > 0) ? 5.f : -5.f;
            settings.movement_speed = max(5.f, settings.movement_speed);
            return 0;
        }
        return CallWindowProc(original_wndproc, hwnd, msg, wparam, lparam);
    }

    vec3 GetMovementInput(const quat& rotation) {
        vec3 movement(0.0f, 0.0f, 0.0f);
        
        
        vec3 forward = rotation.rotate(vec3(0.0f, 0.0f, 1.0f));
        vec3 right = rotation.rotate(vec3(1.0f, 0.0f, 0.0f));
        
        
        forward.y = 0.0f;
        right.y = 0.0f;

        if (forward.length() > 0.0f) forward = forward.normalize();
        if (right.length() > 0.0f) right = right.normalize();

        
        if (GetAsyncKeyState('W') & 0x8000) movement += forward;
        if (GetAsyncKeyState('S') & 0x8000) movement -= forward;
        if (GetAsyncKeyState('D') & 0x8000) movement += right;
        if (GetAsyncKeyState('A') & 0x8000) movement -= right;
        
        
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) movement.y += 1.0f;
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000) movement.y -= 1.0f;

        return movement.length() > 0.0f ? movement.normalize() : movement;
    }

    void UpdateCamera(sow::CameraTransform* camera) {
        settings.is_active = (GetForegroundWindow() == game_window);
        this->camera_transform = camera;

        if (!settings.is_active) {
            settings.velocity = settings.velocity * 0.95f;
            camera->position += settings.velocity;
            return;
        }

        vec3 movement = GetMovementInput(camera->rotation);
        vec3 target_velocity = vec3(
            movement.x * settings.movement_speed,
            movement.y * settings.movement_speed,
            movement.z * settings.movement_speed
        );

        
        settings.velocity = vec3::lerp(settings.velocity, target_velocity, settings.horizontal_smooth_factor);
        
        
        settings.velocity.x *= 0.98f;
        settings.velocity.z *= 0.98f;

        camera->position += settings.velocity;
    }

    PVOID original_transform_callback = nullptr;
    static void* set_transform_callback( void* transform, vec3* position )
    {
        if( instance->camera_transform == transform )
        {
            return nullptr;
        }
        return reinterpret_cast<void*( __fastcall* )( void*, vec3* )>( instance->original_transform_callback )( transform, position );
    };

    bool Initialize() {
        AllocConsole();
        FILE* dummy;
        freopen_s(&dummy, "CONOUT$", "w", stdout);
        freopen_s(&dummy, "CONIN$", "r", stdin);

        printf("Initializing FreeCam...\n");

        if (MH_Initialize() != MH_OK) {
            printf("Failed to initialize MinHook\n");
            return false;
        }

        
        game_window = FindWindow("Shadow of War", NULL);
        if (!game_window) {
            printf("Failed to find game window\n");
            return false;
        }

        original_wndproc = (WNDPROC)SetWindowLongPtr(game_window, GWLP_WNDPROC, (LONG_PTR)WndProc);

        
        uintptr_t game_module = (uintptr_t)GetModuleHandle(nullptr);
        uintptr_t call_addr = memory::sigscan("E8 ? ? ? ? 41 B0 01 EB ? 49 8B CA");
        printf("Found camera update at 0x%p\n", call_addr);
        /* Get RIP relative address of function */
        int32_t rip_relative = *(int32_t*)(call_addr + 1);
        uintptr_t call_offset = call_addr + 5 + rip_relative;
        printf("Camera update offset: 0x%p\n", call_offset);
        
        /* Hook */
        if (MH_CreateHook((void*)call_offset, &set_transform_callback, &original_transform_callback) != MH_OK) {
            printf("Failed to hook camera update\n");
            return false;
        }
        if (MH_EnableHook((void*)call_offset) != MH_OK) {
            printf("Failed to enable camera update hook\n");
            return false;
        }

        printf("FreeCam initialized\n");

        return true;
    }

    void Cleanup() {
        RestorePatches();
        if (game_window) {
            SetWindowLongPtr(game_window, GWLP_WNDPROC, (LONG_PTR)original_wndproc);
        }
        MH_Uninitialize();
        fclose(stdout);
        FreeConsole();
        FreeLibraryAndExitThread(module_handle, 0);
    }

public:
    void Run() {
        
        if (!Initialize()) {
            Cleanup();
            return;
        }

        
        uintptr_t game_client_address = memory::sigscan("48 8B 0D ? ? ? ? E8 ? ? ? ? C7 47");
        printf("Found game client at 0x%p\n", game_client_address);
        /* Get rip relative value */
        int32_t rip_relative = *(int32_t*)(game_client_address + 3);
        uintptr_t game_client_offset = game_client_address + 7 + rip_relative;
        printf("Game client offset: 0x%p\n", game_client_offset);
        sow::GameClient* game_client = *(sow::GameClient**)(game_client_offset);
        if (!game_client || !game_client->camera_owner || !game_client->camera_owner->gameplay_camera) {
            printf("Failed to get camera\n");
            Cleanup();
            return;
        }

        
        while (!(GetAsyncKeyState(VK_END) & 0x8000)) {
            if (game_client->camera_owner->gameplay_camera->transform) {
                UpdateCamera(game_client->camera_owner->gameplay_camera->transform);
            }
            Sleep(16);
        }

        Cleanup();
    }
};

FreeCam* FreeCam::instance = nullptr;

DWORD WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        std::thread([hModule]() {
            FreeCam freecam(hModule);
            freecam.Run();
            FreeLibraryAndExitThread(hModule, 0);
        }).detach();
    }
    return TRUE;
}
#pragma once

namespace sow
{
    class CameraTransform
    {
    private:
        char pad[0xAC];
    public:
        vec3 position;
        quat rotation;
    };

    class Camera
    {
    private:
        char pad[0x98];
    public:
        CameraTransform* transform;
    };

    class CameraOwner
    {
    private:
        char pad[0x50];
    public:
        Camera* gameplay_camera;

        int get_mode( )
        {
            return *( int* )( ( uintptr_t )this + 0x80 );
        }
    };
}
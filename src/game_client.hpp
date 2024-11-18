#pragma once

namespace sow
{
    class GameClient
    {
    private:
        char pad[0x898];
    public:
        CameraOwner* camera_owner;
    };
}
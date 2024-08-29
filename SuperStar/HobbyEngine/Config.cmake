set(ENGINE_NAME HobbyEngine)

# プロジェクトに登録するファイル群
# ここに全て記載する
set(ENGINE_TARGET_FILES
    .editorconfig
    LICENSE.txt
    README.md
    ThirdPartyNotices.txt

    Engine/Engine.cpp
    Engine/Engine.h
    Engine/GameMain.h
    Engine/MiniEngine.h

    Engine/Config.h
    Engine/Core.h
    Engine/Generated.h
    Engine/Macro.h
    Engine/Str.h
    Engine/Type.h

    Engine/Common/CustomArray.h
    Engine/Common/CustomVector.h
    Engine/Common/CustomList.h
    Engine/Common/CustomStack.h
    Engine/Common/CustomMap.h
    Engine/Common/CustomString.cpp
    Engine/Common/CustomString.h
    Engine/Common/Handle.cpp
    Engine/Common/Handle.h
    Engine/Common/PoolManager.h
    Engine/Common/RTTI.cpp
    Engine/Common/RTTI.h
    Engine/Common/Singleton.h
    Engine/Common/Hash.h
    Engine/Common/Hash.cpp
    
    Engine/File/Path.cpp
    Engine/File/Path.h
    
    Engine/Math/Math.cpp
    Engine/Math/Math.h
    Engine/Math/Matrix3.cpp
    Engine/Math/Matrix3.h
    Engine/Math/Matrix4.cpp
    Engine/Math/Matrix4.h
    Engine/Math/Quaternion.cpp
    Engine/Math/Quaternion.h
    Engine/Math/Random.cpp
    Engine/Math/Random.h
    Engine/Math/Rect2.h
    Engine/Math/Rect2.cpp
    Engine/Math/Vector2.h
    Engine/Math/Vector3.h
    Engine/Math/Vector3.cpp
    Engine/Math/Vector4.h
    
    Engine/Memory/Memory.cpp
    Engine/Memory/Memory.h
    Engine/Memory/MemoryManager.cpp
    Engine/Memory/MemoryManager.h
    
    Engine/Task/Task.cpp
    Engine/Task/Task.h
    Engine/Task/TaskManager.cpp
    Engine/Task/TaskManager.h
    
    Engine/Time/FPS.cpp
    Engine/Time/FPS.h
    
    Engine/Module/Module.cpp
    Engine/Module/Module.h
    
    Engine/Platform/PlatformFile.h
    Engine/Platform/PlatformInput.cpp
    Engine/Platform/PlatformInput.h
    Engine/Platform/PlatformScreen.h
    Engine/Platform/PlatformSystem.h
    Engine/Platform/PlatformTime.h
    Engine/Platform/PlatformModule.h
)

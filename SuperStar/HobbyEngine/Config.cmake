set(ENGINE_NAME HobbyEngine)

# プロジェクトに登録するファイル群
# ここに全て記載する
set(ENGINE_TARGET_FILES
    .editorconfig
    LICENSE.txt
    README.md
    ThirdPartyNotices.txt

    Code/Engine.cpp
    Code/Engine.h
    Code/framework.h
    Code/GameMain.h
    Code/MiniEngine.h

    Code/Config.h
    Code/Core.h
    Code/Generated.h
    Code/Macro.h
    Code/Str.h
    Code/Type.h

    Code/Common/CustomArray.h
    Code/Common/CustomMap.h
    Code/Common/CustomString.cpp
    Code/Common/CustomString.h
    Code/Common/Handle.cpp
    Code/Common/Handle.h
    Code/Common/PoolManager.h
    Code/Common/RTTI.cpp
    Code/Common/RTTI.h
    Code/Common/Singleton.h
    
    Code/File/Path.cpp
    Code/File/Path.h
    
    Code/Math/Math.cpp
    Code/Math/Math.h
    Code/Math/Matrix3.cpp
    Code/Math/Matrix3.h
    Code/Math/Matrix4.cpp
    Code/Math/Matrix4.h
    Code/Math/Quaternion.cpp
    Code/Math/Quaternion.h
    Code/Math/Random.cpp
    Code/Math/Random.h
    Code/Math/Rect2.h
    Code/Math/Vector2.h
    Code/Math/Vector3.h
    Code/Math/Vector3.cpp
    Code/Math/Vector4.h
    
    Code/Memory/Memory.cpp
    Code/Memory/Memory.h
    Code/Memory/MemoryManager.cpp
    Code/Memory/MemoryManager.h
    
    Code/Task/Task.cpp
    Code/Task/Task.h
    Code/Task/TaskManager.cpp
    Code/Task/TaskManager.h
    
    Code/Time/FPS.cpp
    Code/Time/FPS.h
    
    Code/Module/Module.cpp
    Code/Module/Module.h
    
    Code/Platform/PlatformFile.h
    Code/Platform/PlatformInput.cpp
    Code/Platform/PlatformInput.h
    Code/Platform/PlatformModule.h
)

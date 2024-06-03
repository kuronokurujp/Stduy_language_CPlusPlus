#pragma once

#include "HobbyEngine/MiniEngine.h"
#include "HobbyEngine/Core/File/Path.h"

// 前方宣言
namespace Platform
{
    class FileSystemInterface;
}

namespace AssetManager
{
    /// <summary>
    /// アセットデータの基本
    /// </summary>
    class AssetDataBase
    {
        friend class AssetManagerModule;

    protected:
        virtual void _Init(const Char* in_pName, Core::File::Path& in_rPath);
        virtual const Bool _Load(Platform::FileSystemInterface& in_rFileSystem) = 0;
        virtual void _Unload() = 0;

    protected:
        Core::Common::FixString128 _name;
        Core::File::Path _path;
    };

    /// <summary>
    /// tomlアセットデータ
    /// </summary>
    class AssetDataToml : public AssetDataBase
    {
    public:
        virtual void _Init(const Char* in_pName, Core::File::Path& in_rPath) override;
        virtual const Bool _Load(Platform::FileSystemInterface& in_rFileSystem) override;
        virtual void _Unload() override;
    };

    /// <summary>
    /// jsonアセットデータ
    /// </summary>
    class AssetDataJson : public AssetDataBase
    {
    public:
        virtual void _Init(const Char* in_pName, Core::File::Path& in_rPath) override;
        virtual const Bool _Load(Platform::FileSystemInterface& in_rFileSystem) override;
        virtual void _Unload() override;
    };
}

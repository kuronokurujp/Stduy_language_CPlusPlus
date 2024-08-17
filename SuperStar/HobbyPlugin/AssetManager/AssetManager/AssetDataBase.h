#pragma once

#include "AssetManager/ThirdParty/pugixml/pugixml.hpp"
#include "AssetManager/ThirdParty/simidjson/simdjson.h"
#include "Engine/Common/CustomMap.h"
#include "Engine/File/Path.h"
#include "Engine/MiniEngine.h"

//  tomlライブラリ内の例外処理をOFF
//  こちらでエラー処理をする
#define TOML_EXCEPTIONS 0
#include "AssetManager/ThirdParty/tomlplusplus/toml.hpp"

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
        HE_CLASS_COPY_NG(AssetDataBase);
        HE_CLASS_MOVE_NG(AssetDataBase);

        friend class AssetManagerModule;

    public:
        AssetDataBase() {}

    protected:
        virtual void _Init(const Char* in_szName, const Core::File::Path& in_rPath);
        virtual const Bool _Load(Platform::FileSystemInterface& in_rFileSystem) = 0;
        virtual void _Unload()                                                  = 0;

    protected:
        Core::Common::FixString128 _szName;
        Core::File::Path _path;
    };

    /// <summary>
    /// tomlアセットデータ
    /// </summary>
    class AssetDataToml : public AssetDataBase
    {
        HE_CLASS_COPY_NG(AssetDataToml);
        HE_CLASS_MOVE_NG(AssetDataToml);

    public:
#define TOML_NODE_MAP_TYPE \
    Core::Common::CustomFixMap<Core::Common::FixString128, class AssetDataToml::Node, 128>

        class Node
        {
        public:
            Node() {}
            Node(toml::node_view<toml::node> in_node) : _node(in_node) {}
            Node(toml::node& in_rNode) : _node(in_rNode) {}

            const Core::Common::FixString512 GetString();

            template <typename... Args>
            typename std::enable_if<(std::is_same<Args, const Char*>::value && ...), Node>::type
            GetNode(Args... args)
            {
                // 引数の個数を取得
                Uint32 uCount = static_cast<Uint32>(sizeof...(args));
                if (uCount <= 0) return Node();

                // 初期化リストを使用して引数を処理
                const Char* values[] = {args...};

                return this->_GetNode(values, uCount);
            }

            template <typename... Args>
            typename std::enable_if<(std::is_same<Args, const Char*>::value && ...),
                                    const Bool>::type
            OutputNodeMap(TOML_NODE_MAP_TYPE* out, Args... args)
            {
                // 引数の個数を取得
                Uint32 uCount = static_cast<Uint32>(sizeof...(args));
                if (uCount <= 0) return FALSE;

                // 初期化リストを使用して引数を処理
                const Char* szaName[] = {args...};
                return this->_OutputNodeMap(out, szaName, uCount);
            }

        private:
            const Bool _OutputNodeMap(TOML_NODE_MAP_TYPE* out, const Char* in_szaName[],
                                      const Uint32 in_uCount);
            Node _GetNode(const Char* in_szaName[], Uint32 in_uCount);

        private:
            toml::node_view<toml::node> _node;
        };

        AssetDataToml() : AssetDataBase() {}

        virtual const Bool _Load(Platform::FileSystemInterface& in_rFileSystem) override;
        virtual void _Unload() override;

        Node GetRootNode();

    private:
        toml::parse_result _result;
    };

    /// <summary>
    /// jsonアセットデータ
    /// </summary>
    class AssetDataJson : public AssetDataBase
    {
        HE_CLASS_COPY_NG(AssetDataJson);
        HE_CLASS_MOVE_NG(AssetDataJson);

    public:
        AssetDataJson() : AssetDataBase() {}

        virtual const Bool _Load(Platform::FileSystemInterface& in_rFileSystem) override;
        virtual void _Unload() override;

    protected:
        template <typename... Args>
        typename std::enable_if<(std::is_same<Args, const Char*>::value && ...), const Bool>::type
        _OutputValue(simdjson::fallback::ondemand::value* out, Args... args)
        {
            HE_ASSERT(out);

            // 引数の個数を取得
            Uint32 uCount = static_cast<Uint32>(sizeof...(args));
            if (uCount <= 0) return FALSE;

            // 初期化リストを使用して引数を処理
            const Char* szaName[] = {args...};
            return this->_OutputValue(out, szaName, uCount);
        }

    private:
        const Bool _OutputValue(simdjson::fallback::ondemand::value* out, const Char* in_szaName[],
                                const Uint32 in_uCount);

    protected:
        Core::Common::Handle _fileHandle;

        std::unique_ptr<simdjson::padded_string> _json;
        std::unique_ptr<simdjson::ondemand::parser> _parser;
        simdjson::ondemand::document _doc;
    };

    /// <summary>
    /// xmlファイルを扱うアセットデータ
    /// </summary>
    class AssetDataXml : public AssetDataBase
    {
        HE_CLASS_COPY_NG(AssetDataXml);
        HE_CLASS_MOVE_NG(AssetDataXml);

    public:
        AssetDataXml() : AssetDataBase() {}

        virtual const Bool _Load(Platform::FileSystemInterface& in_rFileSystem) override;
        virtual void _Unload() override;

    protected:
        Core::Common::Handle _fileHandle;
        pugi::xml_document _doc;
    };

}  // namespace AssetManager

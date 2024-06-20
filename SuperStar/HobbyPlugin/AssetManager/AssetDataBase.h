#pragma once

#include "HobbyEngine/Core/Common/FixMap.h"
#include "HobbyEngine/Core/File/Path.h"
#include "HobbyEngine/MiniEngine.h"

// #include "ThirdParty/nlohmann/json.hpp"
//  TODO: rapidjsonを導入
//  tomlライブラリ内の例外処理をOFF
//  こちらでエラー処理をする
#define TOML_EXCEPTIONS 0
#include "ThirdParty/simidjson/simdjson.h"
#include "ThirdParty/tomlplusplus/toml.hpp"

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
        virtual void _Init(const Char* in_pName, const Core::File::Path& in_rPath);
        virtual const Bool _Load(Platform::FileSystemInterface& in_rFileSystem) = 0;
        virtual void _Unload()                                                  = 0;

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
#define TOML_NODE_MAP_TYPE \
    Core::Common::FixMap<Core::Common::FixString128, class ::AssetManager::AssetDataToml::Node, 128>

        class Node
        {
        public:
            Node() {}
            Node(toml::node_view<toml::node> in_node) : _node(in_node) {}
            Node(toml::node& in_node) : _node(in_node) {}

            const Core::Common::FixString512 GetString();

            template <typename... Args>
            typename std::enable_if<(std::is_same<Args, const Char*>::value && ...), Node>::type
            GetNode(Args... args)
            {
                // 引数の個数を取得
                Uint32 count = static_cast<Uint32>(sizeof...(args));
                if (count <= 0) return Node();

                // 初期化リストを使用して引数を処理
                const Char* values[] = {args...};

                return this->_GetNode(values, count);
            }

            template <typename... Args>
            typename std::enable_if<(std::is_same<Args, const Char*>::value && ...),
                                    const Bool>::type
            OutputNodeMap(TOML_NODE_MAP_TYPE* out, Args... args)
            {
                // 引数の個数を取得
                Uint32 count = static_cast<Uint32>(sizeof...(args));
                if (count <= 0) return FALSE;

                // 初期化リストを使用して引数を処理
                const Char* values[] = {args...};
                return this->_OutputNodeMap(out, values, count);
            }

        private:
            const Bool _OutputNodeMap(TOML_NODE_MAP_TYPE* out, const Char* in_args[],
                                      const Uint32 in_count);
            Node _GetNode(const Char*[], Uint32 in_count);

        private:
            toml::node_view<toml::node> _node;
        };

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
    public:
        virtual const Bool _Load(Platform::FileSystemInterface& in_rFileSystem) override;
        virtual void _Unload() override;

    protected:
        template <typename... Args>
        typename std::enable_if<(std::is_same<Args, const Char*>::value && ...), const Bool>::type
        _OutputValue(simdjson::fallback::ondemand::value* out, Args... args)
        {
            E_ASSERT(out);

            // 引数の個数を取得
            Uint32 count = static_cast<Uint32>(sizeof...(args));
            if (count <= 0) return FALSE;

            // 初期化リストを使用して引数を処理
            const Char* values[] = {args...};
            return this->_OutputValue(out, count, values);
        }

    private:
        const Bool _OutputValue(simdjson::fallback::ondemand::value* out, const Sint32 in_count,
                                const Char* values[]);

    protected:
        Core::Common::Handle _fileHandle;

        std::unique_ptr<simdjson::padded_string> _json;
        std::unique_ptr<simdjson::ondemand::parser> _parser;
        simdjson::ondemand::document _doc;
    };
}  // namespace AssetManager

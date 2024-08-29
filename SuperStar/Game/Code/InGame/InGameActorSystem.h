#pragma once

#include "Engine/MiniEngine.h"
#include "LevelModule.h"

#if 0
#include "common/Common.h"
#include "common/Man.h"
#include "scripting/LuaStateManager.h"

// STLを使うとwarningが出る可能性があるので、それを出さないようにする
#pragma warning(disable : 4786)
#include <map>

namespace InGame
{
    // インターフェイス用のアクタークラスを作成
    class I_Actor
    {
    public:
        I_Actor(void) {}
        virtual ~I_Actor(void) {}

        //	クラス型判定用
        virtual const unsigned VGetRttiId() const = 0;

        virtual bool init(const int in_Handle) = 0;
        virtual bool final(void)               = 0;
        virtual bool update(void)              = 0;
        virtual void draw(void)                = 0;

        //	自殺用関数
        virtual void VDie()               = 0;
        virtual const bool VIsDie() const = 0;

        //	更新停止
        virtual void VStop(const bool in_Flg) = 0;
        virtual const bool VIsStop() const    = 0;

        //	ハンドル取得
        virtual const int VGetHandle() const = 0;
    };

    // アクター管理クラス
    class I_ActorManager
    {
    public:
        // 定数
        enum
        {
            NOT_HANDLE = -1,
        };

        I_ActorManager(void) {}
        virtual ~I_ActorManager(void) {}

        virtual void init(void)  = 0;
        virtual void final(void) = 0;

        // 追加、削除が必要。
        // 追加時ハンドルを返す
        virtual int add(I_Actor* in_pClass) = 0;
        // 削除にはハンドルが必要。
        virtual void destroy(int in_handle) = 0;
        //	更新
        virtual void update(void) = 0;
        //	描画
        virtual void draw(void) = 0;
    };

    // ゲーム専用のアクター管理クラス
    class C_ActorManager : public I_ActorManager
    {
    public:
        C_ActorManager() {}
        virtual ~C_ActorManager() {}

        virtual void init(void);
        virtual void final(void);
        // 追加、削除が必要。
        // 追加時ハンドルを返す
        virtual int add(I_Actor* in_pClass);
        // 削除にはハンドルが必要。
        void destroy(int in_handle);
        //	更新
        virtual void update(void);
        //	描画
        void draw(void);
        // ハンドルからアクセスする
        I_Actor* GetData(int in_handle);

        //	アクターリスト取得
        std::multimap<int, I_Actor*>& GetList() { return m_ActorHashTable; }

    protected:
        std::multimap<int, I_Actor*> m_ActorHashTable;
        // 値を管理するリストが必要になる
        int m_Handle;
    };

    //	ゲーム専用のアクター管理クラス( in Actor同士の当たり判定用 )
    //	Actor同士の当たり判定を行う場合はここに登録
    class InGameActorManager final : public Actor::ActorManagerDecoraterlnterface
    {
        HE_CLASS_COPY_NG(InGameActorManager);
        HE_CLASS_MOVE_NG(InGameActorManager);

    public:
        //static InGameActorManager& inst(void) { return m_Manager; }

        const Bool VStart(Actor::ActorManager*) override final;

        /// <summary>
        /// アクター更新
        /// </summary>
        //void Update(const Float32 in_fDt, const Core::TaskData&) override final;

        void VLateUpdate(const Float32 in_fDt, Actor::ActorManager*) override final;

        /*
                // 追加時ハンドルを返す
                int add(I_Actor* in_pClass);

                //	Luaスクリプトからの関数登録
                //	スクリプト側からのデータ送信
                static int TriggerEventLua(lua_State* in_pState);
                */

    private:
        //	コリジョン判定処理
        //static InGameActorManager m_Manager;
        // TODO: Luaモジュールが必要
    };

    /*
            @brief	他アクターデータ通知用
    */
    class C_PaketSendActor : public I_Actor
    {
        friend class InGameActorManager;

    public:
        //	ここでコリジョン判定クラスを追加するか？
        C_PaketSendActor()
        {
            m_bDie  = false;
            m_bStop = false;
        }
        virtual ~C_PaketSendActor(void) {}

        //	クラス型判定用
        virtual const unsigned VGetRttiId() const { return 0; }

        // 初期化、終了
        // 更新、描画で十分
        virtual bool init(const int in_Handle)
        {
            m_Handle = in_Handle;
            return true;
        }
        virtual bool final(void) { return true; }
        //	ここでコリジョン判定をする
        virtual bool update(void) { return true; }
        virtual void draw(void) {}

        //	データ通知
        virtual void VOnCallbackSendMessage(const int in_DataType, void* in_pData) {}

        //	自殺用
        virtual void VDie() { m_bDie = true; }
        virtual const bool VIsDie() const { return m_bDie; }

        //	更新停止
        virtual void VStop(const bool in_bFlg) { m_bStop = in_bFlg; }
        virtual const bool VIsStop() const { return m_bStop; }

        //	ハンドル取得
        virtual const int VGetHandle() const { return m_Handle; }

    private:
        //	登録した自分以外のアクターを取得する。
        virtual void _VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor) {}
        //	スクリプトで通知したデータを受け取る
        virtual void _VOnCallbackScriptEvent(lua_State* in_pLuaState) {}

        //	変数
        bool m_bDie;
        bool m_bStop;
        int m_Handle;
    };

    /*
            オブジェクト衝突専用
    */
    class C_ColisionActor : public C_PaketSendActor
    {
    public:
        //	ここでコリジョン判定クラスを追加するか？
        C_ColisionActor(const Vec3& in_pos, const float in_size)
            : C_PaketSendActor(), m_pos(in_pos), m_size(in_size)
        {
        }

        C_ColisionActor(const float in_size)
            : C_PaketSendActor(), m_pos(0.f, 0.f, 0.f), m_size(in_size)
        {
        }
        virtual ~C_ColisionActor(void) {}

        enum
        {
            RTTI_ID = 1,
        };

        //	クラス型判定用( これ以上仮想化しない )
        const unsigned VGetRttiId() const { return RTTI_ID; }

        //	データ取得
        const Vec3& GetPos() const { return m_pos; }
        const float GetSize() const { return m_size; }

        void SetPos(const Vec3& in_Pos)
        {
            m_pos.x = in_Pos.x;
            m_pos.y = in_Pos.y;
        }
        void SetSize(const float in_Size) { m_size = in_Size; }

    protected:
        Vec3 m_pos;
        //	矩形での判定にする
        float m_size;
    };

}  // namespace InGame

#endif

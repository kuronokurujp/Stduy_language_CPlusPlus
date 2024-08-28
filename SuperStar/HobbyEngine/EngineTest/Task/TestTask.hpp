#pragma once

// タスクシステムをテストするテストコード
#include <chrono>
#include <ctime>

#include "Engine/Task/Task.h"
#include "Engine/Task/TaskManager.h"

namespace Core
{
    TEST_CASE("Task Init and End")
    {
        Core::Memory::Manager memoryManager;

        CHECK(memoryManager.Start(0x1000000));
        // ページ確保テスト
        {
            // メモリサイズのイニシャライズ
            Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
                // 複数ページのサイズ
                {0, 3 * 1024 * 1024}};

            CHECK(memoryManager.SetupMemoryPage(memoryPageSetupInfoArray,
                                                HE_ARRAY_NUM(memoryPageSetupInfoArray)));
            CHECK(memoryManager.CheckAllMemoryBlock());
        }

        static TaskManager manager;
        static const Uint32 taskNum = 32;

        // タスクシステムの初期化
        CHECK(manager.Init(taskNum, 3));

        // タスクの確保数が意図通りか
        CHECK(manager.Max() == taskNum);

        // タスクの利用数が意図通りか
        CHECK(manager.UseCount() == 0);

        // タスクシステムの終了
        manager.End();

        CHECK(memoryManager.VRelease());
        memoryManager.Reset();
    }

    TEST_CASE("Task Add and Remove")
    {
        Core::Memory::Manager memoryManager;
        CHECK(memoryManager.Start(0x1000000));
        // ページ確保テスト
        {
            // メモリサイズのイニシャライズ
            Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
                // 複数ページのサイズ
                {0, 3 * 1024 * 1024}};

            CHECK(memoryManager.SetupMemoryPage(memoryPageSetupInfoArray,
                                                HE_ARRAY_NUM(memoryPageSetupInfoArray)));
            CHECK(memoryManager.CheckAllMemoryBlock());
        }

        /// <summary>
        /// タスクグループ一覧
        /// </summary>
        enum
        {
            GROUP_SYSTEM,
            GROUP_PLAYER,
            GROUP_ENEMY,
            GROUP_EFFECT,
            GROUP_POST,

            GROUP_NUM
        };

        // テスト用のタスクたち
        class EffectTask : public Task
        {
        public:
            EffectTask() : Task() {}

            const Bool VBegin() override final { return TRUE; }
            const Bool VEnd() override final { return TRUE; }

            void VUpdate(const Float32 in_dt, const TaskData&) override final
            {
                this->_time += in_dt;
                HE_LOG(HE_STR_TEXT("F"));

                // 3秒経過したら自動的に死ぬ
                if (this->_time >= 3000.0f) this->Kill();
            }

        private:
            Float32 _time = 0.0f;
        };

        class ObjectTask : public Task
        {
        public:
            ObjectTask() : Task() {}

            const Bool VBegin() override final { return TRUE; }
            const Bool VEnd() override final { return TRUE; }

            void VUpdate(const Float32 in_dt, const TaskData&) override
            {
                this->_time += in_dt;
                HE_LOG(HE_STR_TEXT("P"));

                // 1/30の確率でエフェクトを発生させる
                if (rand() % 30 == 0)
                    this->_pTaskManager->CreateAndAdd<EffectTask>(GROUP_SYSTEM, TRUE);
            }

        private:
            Float32 _time = 0.0f;
        };

        class EnemyTask : public ObjectTask
        {
        public:
            EnemyTask() : ObjectTask() {}

            void VUpdate(const Float32 in_dt, const TaskData&) override final
            {
                this->_time += in_dt;
                HE_LOG(HE_STR_TEXT("E"));
            }

        private:
            Float32 _time = 0.0f;
        };

        class DummySystemTask : public Task
        {
        public:
            DummySystemTask() {}

            const Bool VBegin() override final { return TRUE; }
            const Bool VEnd() override final { return TRUE; }

            void VUpdate(const Float32 in_dt, const TaskData&) override final
            {
                this->_time += in_dt;
                HE_LOG(HE_STR_TEXT("S"));
            }

        private:
            Float32 _time = 0.0f;
        };

        static TaskManager manager;
        static const Uint32 taskNum = 32;

        // タスクシステムの初期化
        CHECK(manager.Init(taskNum, GROUP_NUM));
        // タスクの確保数が意図通りか
        CHECK(manager.Max() == taskNum);
        // タスクの利用数が意図通りか
        CHECK(manager.UseCount() == 0);

        // システムタスクを起動する
        {
            auto h01 = manager.CreateAndAdd<DummySystemTask>(GROUP_SYSTEM, TRUE);
            auto h02 = manager.CreateAndAdd<DummySystemTask>(GROUP_SYSTEM, TRUE);
            CHECK(manager.GetTask(h01) != manager.GetTask(h02));
        }

        // プレイヤータスクを起動する
        Common::Handle&& hPlayer = manager.CreateAndAdd<ObjectTask>(GROUP_PLAYER, TRUE);

        manager.CreateAndAdd<ObjectTask>(GROUP_PLAYER, TRUE);

        // エネミータスクを起動する
        manager.CreateAndAdd<EnemyTask>(GROUP_ENEMY, TRUE);
        manager.CreateAndAdd<EnemyTask>(GROUP_ENEMY, TRUE);
        manager.CreateAndAdd<EnemyTask>(GROUP_ENEMY, TRUE);

        // エフェクトタスクを起動
        manager.CreateAndAdd<EffectTask>(GROUP_EFFECT, TRUE);

        // タスクの利用数が意図通りか
        CHECK(manager.UseCount() == 8);
        {
            // 擬似ゲームループ
            const Sint32 FPS       = 60;
            Float32 msec_one_frame = 1000.0f / FPS;
            Float32 end_time       = 10 * 1000.0f;  // 10秒
            Float32 delta_time = 0.0f, boot_time = 0.0f;

            while (boot_time < end_time)
            {
                Sint64 beginTimeMSec = std::chrono::duration_cast<std::chrono::milliseconds>(
                                           std::chrono::system_clock::now().time_since_epoch())
                                           .count();

                boot_time += delta_time;

                // タスクの更新
                manager.UpdateAll(delta_time, DEFAULT_TASK_DATA);

                HE_LOG(HE_STR_TEXT(" (%d / %d)\n"), manager.UseCount(), manager.Max());

                // フレーム更新間隔まで待つ
                do
                {
                    Sint64 nowTimeMSec = std::chrono::duration_cast<std::chrono::milliseconds>(
                                             std::chrono::system_clock::now().time_since_epoch())
                                             .count();

                    delta_time = static_cast<Float32>(nowTimeMSec - beginTimeMSec);
                } while (delta_time < msec_one_frame);

                // 設定したタスクが取得できるか
                {
                    ObjectTask* pPlayer = dynamic_cast<ObjectTask*>(manager.GetTask(hPlayer));
                    CHECK(pPlayer != NULL);
                }
            }
        }

        // タスクシステムの終了
        manager.End();

        // タスクシステムが終了しているので解放されているか
        CHECK(manager.UseCount() == 0);

        // タスクシステムが終了しているので取得が失敗しているか
        {
            ObjectTask* pPlayer = dynamic_cast<ObjectTask*>(manager.GetTask(hPlayer));
            CHECK(pPlayer == NULL);
        }

        CHECK(memoryManager.VRelease());
        memoryManager.Reset();
    }

    TEST_CASE("Task Move Task")
    {
        Core::Memory::Manager memoryManager;
        CHECK(memoryManager.Start(0x1000000));
        // ページ確保テスト
        {
            // メモリサイズのイニシャライズ
            Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
                // 複数ページのサイズ
                {0, 3 * 1024 * 1024}};

            CHECK(memoryManager.SetupMemoryPage(memoryPageSetupInfoArray,
                                                HE_ARRAY_NUM(memoryPageSetupInfoArray)));
            CHECK(memoryManager.CheckAllMemoryBlock());
        }
        /// <summary>
        /// タスクグループ一覧
        /// </summary>
        enum
        {
            GROUP_SYSTEM,
            GROUP_PLAYER,
            GROUP_ENEMY,
            GROUP_NUM
        };

        // テスト用のタスクたち
        class EffectTask : public Task
        {
        public:
            EffectTask() : Task() {}

            const Bool VBegin() override final { return TRUE; }
            const Bool VEnd() override final { return TRUE; }

            void VUpdate(const Float32 in_fDt, const TaskData&) override
            {
                this->_time += in_fDt;
                HE_LOG(HE_STR_TEXT("F"));

                // 3秒経過したら自動的に死ぬ
                if (this->_time >= 3000.0f) this->Kill();
            }

        private:
            Float32 _time = 0.0f;
        };

        class ObjectTask : public Task
        {
        public:
            ObjectTask() : Task() {}

            const Bool VBegin() override final { return TRUE; }
            const Bool VEnd() override final { return TRUE; }

            void VUpdate(const Float32 in_fDt, const TaskData&) override
            {
                this->_time += in_fDt;
                HE_LOG(HE_STR_TEXT("P"));

                // 1/30の確率でエフェクトを発生させる
                if (rand() % 30 == 0) this->_pTaskManager->CreateAndAdd<EffectTask>(0, FALSE);
            }

        private:
            Float32 _time = 0.0f;
        };

        static TaskManager manager;
        static const Uint32 taskNum = 32;

        // タスクシステムの初期化
        CHECK(manager.Init(taskNum, GROUP_NUM));
        // タスクの確保数が意図通りか
        CHECK(manager.Max() == taskNum);
        // タスクの利用数が意図通りか
        CHECK(manager.UseCount() == 0);

        // タスクを起動する
        auto h1 = manager.CreateAndAdd<ObjectTask>(GROUP_PLAYER, TRUE);
        auto h2 = manager.CreateAndAdd<ObjectTask>(GROUP_PLAYER, TRUE);
        auto h3 = manager.CreateAndAdd<ObjectTask>(GROUP_PLAYER, TRUE);

        // タスクの利用数が意図通りか
        CHECK(manager.UseCount() == 3);

        // グループ設定したタスクの総数が意図通りか
        CHECK(manager.CountWithGroup(GROUP_PLAYER) == 3);
        CHECK(manager.CountWithGroup(GROUP_SYSTEM) == 0);
        CHECK(manager.CountWithGroup(GROUP_ENEMY) == 0);

        // グループ移動
        manager.MoveGroupAll(GROUP_PLAYER, GROUP_SYSTEM);

        // タスクの利用数が意図通りか
        CHECK(manager.UseCount() == 3);

        // グループ設定したタスクの総数が意図通りか
        CHECK(manager.CountWithGroup(GROUP_PLAYER) == 0);
        CHECK(manager.CountWithGroup(GROUP_SYSTEM) == 3);
        CHECK(manager.CountWithGroup(GROUP_ENEMY) == 0);

        manager.MoveGropuTask(h1, GROUP_PLAYER);
        CHECK(manager.CountWithGroup(GROUP_PLAYER) == 1);
        CHECK(manager.CountWithGroup(GROUP_SYSTEM) == 2);
        CHECK(manager.CountWithGroup(GROUP_ENEMY) == 0);

        manager.MoveGropuTask(h2, GROUP_PLAYER);
        CHECK(manager.CountWithGroup(GROUP_PLAYER) == 2);
        CHECK(manager.CountWithGroup(GROUP_SYSTEM) == 1);
        CHECK(manager.CountWithGroup(GROUP_ENEMY) == 0);

        manager.MoveGropuTask(h1, GROUP_ENEMY);
        manager.MoveGropuTask(h2, GROUP_PLAYER);
        manager.MoveGropuTask(h3, GROUP_PLAYER);
        CHECK(manager.CountWithGroup(GROUP_PLAYER) == 2);
        CHECK(manager.CountWithGroup(GROUP_SYSTEM) == 0);
        CHECK(manager.CountWithGroup(GROUP_ENEMY) == 1);

        // タスクシステムの終了
        manager.End();

        // タスクシステムが終了しているので解放されているか
        CHECK(manager.UseCount() == 0);

        CHECK(memoryManager.VRelease());
        memoryManager.Reset();
    }

    TEST_CASE("Task Create or Delete")
    {
        Core::Memory::Manager memoryManager;
        CHECK(memoryManager.Start(0x1000000));
        // ページ確保テスト
        {
            // メモリサイズのイニシャライズ
            Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
                // 複数ページのサイズ
                {0, 3 * 1024 * 1024}};

            CHECK(memoryManager.SetupMemoryPage(memoryPageSetupInfoArray,
                                                HE_ARRAY_NUM(memoryPageSetupInfoArray)));
            CHECK(memoryManager.CheckAllMemoryBlock());
        }
        /// <summary>
        /// タスクグループ一覧
        /// </summary>
        enum
        {
            GROUP_SYSTEM,
            GROUP_PLAYER,
            GROUP_NUM
        };

        // テスト用のタスクたち
        class EffectTask : public Task
        {
        public:
            EffectTask() : Task() {}

            const Bool VBegin() override final { return TRUE; }
            const Bool VEnd() override final { return TRUE; }

            void VUpdate(const Float32 in_fDt, const TaskData&) override
            {
                this->_time += in_fDt;
                HE_LOG(HE_STR_TEXT("F"));

                // 3秒経過したら自動的に死ぬ
                if (this->_time >= 3000.0f) this->Kill();
            }

        private:
            Float32 _time = 0.0f;
        };

        class ObjectTask : public Task
        {
        public:
            ObjectTask() : Task() {}

            const Bool VBegin() override final { return TRUE; }
            const Bool VEnd() override final { return TRUE; }

            void VUpdate(const Float32 in_fDt, const TaskData&) override
            {
                this->_time += in_fDt;
                HE_LOG(HE_STR_TEXT("P"));

                // 1/30の確率でエフェクトを発生させる
                if (rand() % 30 == 0) this->_pTaskManager->CreateAndAdd<EffectTask>(0, TRUE);
            }

        private:
            Float32 _time = 0.0f;
        };

        static TaskManager manager;
        static const Uint32 taskNum = 32;

        // タスクシステムの初期化
        CHECK(manager.Init(taskNum, GROUP_NUM));
        // タスクの確保数が意図通りか
        CHECK(manager.Max() == taskNum);
        // タスクの利用数が意図通りか
        CHECK(manager.UseCount() == 0);

        // タスクを起動する
        // 削除した時にメモリから解放
        auto h1 = manager.CreateAndAdd<EffectTask>(GROUP_PLAYER, TRUE);
        CHECK(!h1.Null());
        auto h2 = manager.CreateAndAdd<EffectTask>(GROUP_PLAYER, TRUE);
        CHECK(!h2.Null());
        auto h3 = manager.CreateAndAdd<EffectTask>(GROUP_PLAYER, FALSE);
        CHECK(!h3.Null());

        // タスクの利用数が意図通りか
        CHECK(manager.UseCount() == 3);

        manager.RemoveTask(&h1);
        CHECK(manager.UseCount() == 2);
        CHECK(manager.GetTask(h1) == NULL);
        CHECK(manager.CacheCount() == 0);

        manager.RemoveTask(&h3);
        CHECK(manager.CacheCount() == 1);
        CHECK(manager.UseCount() == 1);

        auto h4 = manager.CreateAndAdd<EffectTask>(GROUP_PLAYER, TRUE);
        CHECK(!h4.Null());
        CHECK(manager.CacheCount() == 0);
        // タスクの利用数が意図通りか
        CHECK(manager.UseCount() == 2);

        // タスクシステムの終了
        manager.End();

        // タスクシステムが終了しているので解放されているか
        CHECK(manager.UseCount() == 0);

        CHECK(memoryManager.VRelease());
        memoryManager.Reset();
    }

    TEST_CASE("Testg Child Task")
    {
        Core::Memory::Manager memoryManager;
        CHECK(memoryManager.Start(0x1000000));
        // ページ確保テスト
        {
            // メモリサイズのイニシャライズ
            Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
                // 複数ページのサイズ
                {0, 3 * 1024 * 1024}};

            CHECK(memoryManager.SetupMemoryPage(memoryPageSetupInfoArray,
                                                HE_ARRAY_NUM(memoryPageSetupInfoArray)));
            CHECK(memoryManager.CheckAllMemoryBlock());
        }
        /// <summary>
        /// タスクグループ一覧
        /// </summary>
        enum
        {
            GROUP_SYSTEM,
            GROUP_PLAYER,
            GROUP_NUM
        };

        // テスト用のタスクたち
        class EffectTask : public Task
        {
        public:
            EffectTask() : Task() {}

            const Bool VBegin() override final { return TRUE; }
            const Bool VEnd() override final { return TRUE; }

            void VUpdate(const Float32 in_fDt, const TaskData&) override
            {
                this->_time += in_fDt;
                HE_LOG(HE_STR_TEXT("F"));

                // 3秒経過したら自動的に死ぬ
                if (this->_time >= 3000.0f) this->Kill();
            }

        private:
            Float32 _time = 0.0f;
        };

        class ObjectTask : public Task
        {
        public:
            ObjectTask() : Task() {}

            const Bool VBegin() override final { return TRUE; }
            const Bool VEnd() override final { return TRUE; }

            void VUpdate(const Float32 in_fDt, const TaskData&) override
            {
                this->_time += in_fDt;
                HE_LOG(HE_STR_TEXT("P"));
            }

        private:
            Float32 _time = 0.0f;
        };

        static TaskManager manager;
        static const Uint32 taskNum = 32;

        // タスクシステムの初期化
        CHECK(manager.Init(taskNum, GROUP_NUM));
        // タスクの確保数が意図通りか
        CHECK(manager.Max() == taskNum);
        // タスクの利用数が意図通りか
        CHECK(manager.UseCount() == 0);

        // タスクを起動する
        // 削除した時にメモリから解放
        auto h1 = manager.CreateAndAdd<EffectTask>(GROUP_PLAYER, TRUE);
        CHECK_FALSE(h1.Null());
        auto h2 = manager.CreateAndAdd<ObjectTask>(GROUP_PLAYER, TRUE);
        CHECK_FALSE(h2.Null());
        auto h3 = manager.CreateAndAdd<EffectTask>(GROUP_PLAYER, FALSE);
        CHECK_FALSE(h3.Null());

        // タスクの利用数が意図通りか
        CHECK(manager.UseCount() == 3);
        auto pTask = manager.GetTask(h1);
        CHECK_FALSE(pTask->AddChildTask(h1));
        CHECK(pTask->AddChildTask(h2));
        CHECK(manager.UseCount() == 3);
        manager.UpdateAll(0, DEFAULT_TASK_DATA);
        HE_LOG(HE_STR_TEXT(" (%d / %d)\n"), manager.UseCount(), manager.Max());

        // 親タスクが外したのだから子タスクも同時に外れているはず
        manager.RemoveTask(&h1);
        CHECK(manager.UseCount() == 1);
        CHECK(manager.GetTask(h1) == NULL);
        CHECK(manager.GetTask(h2) == NULL);
        CHECK(manager.GetTask(h3));
        CHECK(manager.CacheCount() == 0);

        auto h4 = manager.CreateAndAdd<EffectTask>(GROUP_PLAYER, FALSE);
        CHECK_FALSE(h4.Null());

        auto h5 = manager.CreateAndAdd<EffectTask>(GROUP_PLAYER, FALSE);
        CHECK_FALSE(h5.Null());

        pTask = manager.GetTask(h3);
        pTask->AddChildTask(h4);
        pTask = manager.GetTask(h4);
        pTask->AddChildTask(h5);

        manager.UpdateAll(0, DEFAULT_TASK_DATA);
        HE_LOG(HE_STR_TEXT(" (%d / %d)\n"), manager.UseCount(), manager.Max());

        // タスクシステムの終了
        manager.End();

        // タスクシステムが終了したのだから今まで確保したタスクがないとおかしい
        CHECK(manager.GetTask(h1) == NULL);
        CHECK(manager.GetTask(h2) == NULL);
        CHECK(manager.GetTask(h3) == NULL);

        // タスクシステムが終了しているので解放されているか
        CHECK(manager.UseCount() == 0);

        CHECK(memoryManager.VRelease());
        memoryManager.Reset();

        HE_LOG_LINE(HE_STR_TEXT(""));
    }

}  // namespace Core

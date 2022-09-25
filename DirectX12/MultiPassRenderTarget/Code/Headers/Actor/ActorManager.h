#pragma once

#include <vector>

/*
    TODO: アクター管理を生ポインターにしているが、ポインター先が消滅したらまずいので共有ポインターに変更する
*/
namespace Actor
{
    // 前方宣言
    class Actor;

    class ActorManager
    {
    public:
        ActorManager() { this->_Clear(); }

        /// <summary>
        /// アクターを登録.
        /// </summary>
        void AddActor(Actor* in_pActor);

        /// <summary>
        /// 登録したアクターを解放
        /// </summary>
        /// <param name="in_pActor"></param>
        void RemoveActor(Actor* in_pActor);

        const bool IsActor(Actor* in_pActor);

        const std::vector<Actor*> GetActors();
        // テンプレートで指定したアクターを取得.
        // ポインター型限定.
        template <class T> const std::vector<T> GetActors()
        {
            std::vector<T> find_list;
            for (unsigned int i = 0; i < this->_actors.size(); ++i)
            {
                auto actor = this->_actors[i];
                T t = dynamic_cast<T>(actor);
                if (t != NULL)
                {
                    find_list.emplace_back(t);
                }
            }

            return find_list;
        }

        /// <summary>
        /// 全てのアクターを破棄.
        /// アクターのメモリも同時に開放
        /// </summary>
        void DeleteAllActorsAndMemFree();

        void Tick(float in_deltaTimeSecond);
        void Render();

    protected:
        // Actor登録リスト
        std::vector<Actor*> _actors;

    private:
        void _Clear();
    };
}

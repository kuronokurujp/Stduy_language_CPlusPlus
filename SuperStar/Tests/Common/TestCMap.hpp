#pragma once

#include <time.h>

#include "Core/Common/FixMap.h"
#include "Core/Common/FixString.h"
#include "Core/Memory/Memory.h"

// 標準のメモリ確保を使用する<文字列, 整数>のマップ
typedef Core::Common::FixMap<Core::Common::FixString16, Sint32, 2048> BASICMAP;
std::ostream& operator<<(std::ostream& out, const BASICMAP::Iterator& r)
{
    (void)r;
    return out;
}

TEST_CASE("FixMap Test")
{
    clock_t ctime;
    Core::Common::FixString16 strKey;

    // データ個数
    const Sint32 ARRAY_NUM = 1024;

    // 乱数を時間で初期化
    srand((unsigned int)time(NULL));

    // データを用意しておく
    Sint32* pDataArray = new Sint32[ARRAY_NUM];
    for (Sint32 ii = 0; ii < ARRAY_NUM; ii++)
    {
        pDataArray[ii] = (Sint32)ii;
    }

    for (Sint32 shuffle = 0; shuffle < 100000; ++shuffle)
    {
        Sint32 a = rand() % ARRAY_NUM;
        Sint32 b = rand() % ARRAY_NUM;

        // 適当に混ぜっ返す
        Sint32 tmp    = pDataArray[a];
        pDataArray[a] = pDataArray[b];
        pDataArray[b] = tmp;
    }

    // マップの生成と空かどうかのテスト
    BASICMAP testmap;
    CHECK(testmap.Empty());

    // データの追加
    {
        ctime = clock();
        for (Sint32 ii = 0; ii < ARRAY_NUM; ii++)
        {
            // データの中身を16進数にしてキーにする
            strKey.Format("0x%04x", pDataArray[ii]);
            // キーとデータを追加する
            testmap.Add(strKey.Str(), pDataArray[ii]);
        }
        // 時間の報告
        E_LOG_LINE("add ctime=%f sec", (double)(clock() - ctime) / CLOCKS_PER_SEC);
    }

    // 空じゃなくなったかチェック
    CHECK(!testmap.Empty());

    // ノードの正当性チェック
    {
        ctime        = clock();
        Bool bResult = testmap.CheckValidByDebug(ARRAY_NUM);
        CHECK(bResult);
        E_LOG_LINE("check ctime=%f sec", (double)(clock() - ctime) / CLOCKS_PER_SEC);
    }

    // 検索
    {
        ctime = clock();
        for (Sint32 ii = 0; ii < ARRAY_NUM; ii++)
        {
            // キーを作る
            strKey.Format("0x%04x", ii);

            // 検索
            BASICMAP::Iterator it = testmap.Find(strKey.Str());
            CHECK(it != testmap.End());
            CHECK(it->_key == strKey);
            CHECK(it->_data == ii);
        }
        E_LOG_LINE("find ctime=%f sec", (double)(clock() - ctime) / CLOCKS_PER_SEC);
    }

    // イテレータテスト
    {
        // イテレータ用のチェックリスト配列を作る
        Bool* pCheckArray    = new Bool[ARRAY_NUM];
        Uint32 checked_count = 0;
        for (Sint32 ii = 0; ii < ARRAY_NUM; ii++)
        {
            pCheckArray[ii] = FALSE;
        }

        ctime = clock();
        for (BASICMAP::Iterator it = testmap.Begin(); it != testmap.End(); ++it)
        {
            CHECK((0 <= it->_data && it->_data < ARRAY_NUM));
            CHECK((pCheckArray[it->_data] == FALSE));

            // チェック済み
            pCheckArray[it->_data] = TRUE;
            // チェック済みカウント
            checked_count++;
        }

        // イテレータの数はノードの数と一致しているか？
        CHECK(checked_count == ARRAY_NUM);
        E_LOG_LINE("iterator ctime=%f sec", (double)(clock() - ctime) / CLOCKS_PER_SEC);

        E_SAFE_DELETE_ARRAY(pCheckArray);
    }

    // 削除
    {
        ctime = clock();
        for (Sint32 ii = 0; ii < ARRAY_NUM; ii++)
        {
            // キーを作って
            strKey.Format("0x%04x", ii);
            // 探す
            BASICMAP::Iterator it = testmap.Find(strKey.Str());
            Bool bResult;
            // 削除する
            bResult = testmap.Erase(it);
            CHECK(bResult);
        }

        E_LOG_LINE("iterator ctime=%f sec", (double)(clock() - ctime) / CLOCKS_PER_SEC);
    }

    // 全て削除が終わったので、空かどうかチェック
    CHECK(testmap.Empty());

    // 添え字でアクセスする
    testmap["one"]   = 1;
    testmap["two"]   = 2;
    testmap["three"] = 3;
    CHECK(testmap["one"] == 1);
    CHECK(testmap["two"] == 2);
    CHECK(testmap["three"] == 3);
    CHECK(testmap.Size() == 3);

    // 添え字で書き換え
    testmap["one"]   = 10;
    testmap["two"]   = 20;
    testmap["three"] = 30;
    CHECK(testmap["one"] == 10);
    CHECK(testmap["two"] == 20);
    CHECK(testmap["three"] == 30);
    // 数は変わっていないはず
    CHECK(testmap.Size() == 3);

    // クリア関数のテスト
    testmap.Clear();
    CHECK(testmap.Empty());

    // データの削除
    E_SAFE_DELETE_ARRAY(pDataArray);
}

TEST_CASE("FixMap Copy")
{
    BASICMAP srcmap;

    struct TEST_DATA
    {
        const Char* pKey = 0;
        Sint32 num       = 0;
    };
    static const TEST_DATA s_aArray[] = {
        {E_STR_TEXT("test1"), 10}, {E_STR_TEXT("test2"), 20}, {E_STR_TEXT("test3"), 30},
        {E_STR_TEXT("test4"), 40}, {E_STR_TEXT("test5"), 50}, {E_STR_TEXT("test6"), 60},
    };

    for (Uint32 i = 0; i < E_ARRAY_NUM(s_aArray); ++i)
    {
        srcmap.Add(s_aArray[i].pKey, s_aArray[i].num);
    }

    BASICMAP dstmap = srcmap;
    CHECK(dstmap.Size() == E_ARRAY_NUM(s_aArray));

    for (Uint32 i = 0; i < E_ARRAY_NUM(s_aArray); ++i)
    {
        auto iter = dstmap.Find(s_aArray[i].pKey);
        CHECK(iter.IsValid());
        CHECK(iter->_data == s_aArray[i].num);

        E_LOG_LINE(E_STR_TEXT("key(%s) / data(%d)"), iter->_key.Str(), iter->_data);
    }
}

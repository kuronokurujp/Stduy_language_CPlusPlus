#include "Engine/Common/CustomList.h"

TEST_CASE("List Push Pop ")
{
    // テスト用、文字クラス
    class CCharacter : public Core::Common::LinkedListNode<CCharacter>
    {
    public:
        CCharacter(Char* letter) { _pLetter = letter; }
        virtual ~CCharacter() {}

        void Print() { HE_LOG(_pLetter); }

    private:
        Char* _pLetter = NULL;
    };

    Core::Common::CustomList<CCharacter> linearlist;
    CHECK(linearlist.Empty());

    CCharacter a(HE_STR_TEXT("a"));

    CHECK(linearlist.PushBack(a));
    CHECK_FALSE(linearlist.Empty());
    CHECK(linearlist.PopBack());
    CHECK(linearlist.Empty());
    CHECK(linearlist.PushFront(a));
    CHECK_FALSE(linearlist.Empty());
    CHECK(linearlist.PopFront());
    CHECK(linearlist.Empty());
}

TEST_CASE("List Loop ")
{
    // テスト用、文字クラス
    class CCharacter2 : public Core::Common::LinkedListNode<CCharacter2>
    {
    public:
        CCharacter2(Char letter) { _pLetter = letter; }
        virtual ~CCharacter2() {}

        const Char GetChar() const { return this->_pLetter; }

    private:
        Char _pLetter = 0;
    };

    Core::Common::CustomList<CCharacter2> linearlist;
    CHECK(linearlist.Empty());

    CCharacter2 a(HE_STR_TEXT('a'));
    CCharacter2 b(HE_STR_TEXT('b'));
    CCharacter2 c(HE_STR_TEXT('c'));
    CCharacter2 d(HE_STR_TEXT('d'));

    CHECK(linearlist.PushBack(a));
    CHECK(linearlist.PushBack(b));
    CHECK(linearlist.PushBack(c));
    CHECK(linearlist.PushBack(d));
    CHECK_FALSE(linearlist.Empty());

    const Char* pTextCode = HE_STR_TEXT("abcd");
    Uint32 i              = 0;
    for (auto itr = linearlist.BeginItr(); itr != linearlist.EndItr(); ++itr)
    {
        CHECK((itr->GetChar() == pTextCode[i]));
        ++i;
    }

    i = 3;
    for (auto itr = linearlist.BeginRevItr(); itr != linearlist.EndRevItr(); ++itr)
    {
        CHECK((itr->GetChar() == pTextCode[i]));
        --i;
    }

    while (linearlist.Empty() == FALSE)
    {
        CHECK(linearlist.PopBack());
    }

    CHECK(linearlist.Empty());
}

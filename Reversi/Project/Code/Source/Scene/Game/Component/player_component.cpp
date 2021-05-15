#include "Scene/Game/Component/player_component.h"

#include "Actor/actor.h"

#include "Component/text_animation_component.h"
#include "Component/input_component.h"

#include "System/Interface/keyboard_interface.h"

#include "Scene/Game/Component/stonehand_component.h"

// TODO: プレイヤー用の入力コンポーネント追加
#ifdef __CUI_GAME__
class PlayerInputComponent final : public Component
{
public:
	PlayerInputComponent(Actor* in_pOwner, KeyboardInterface* in_pKeyboard)
		: Component(in_pOwner)
	{
		assert(in_pKeyboard != NULL);
		this->_p_keyboard = in_pKeyboard;
	}

	void Update(const float in_deltaTimeSecond) override final {}

	// アンドゥ入力しているか
	const bool IsUndo()
	{
		// 入力したテキストから石を置く
		const char* inputText = this->_p_keyboard->GetConfirmEnterText();
		return ((strlen(inputText) == 1) && (inputText[0] == 'u'));
	}

	// 石を置く
	const StoneHandComponent::eResultPlacementStone ExecutePlacementStone()
	{
		StoneHandComponent* pStoneHandComponent = this->_pOwner->GetComponent<StoneHandComponent*>();

		const char* inputText = this->_p_keyboard->GetConfirmEnterText();
		return pStoneHandComponent->SetPlacementStone(inputText);
	}

private:
	KeyboardInterface* _p_keyboard;
};
#else
class PlayerInputComponent final : public InputComponent
{
public:
	PlayerInputComponent(Actor* in_pOwner, KeyboardInterface* in_pKeyboard)
		: InputComponent(in_pOwner)
	{
		assert(in_pKeyboard != NULL);

		this->_Clear();

		this->_p_keyboard = in_pKeyboard;
	}

	// タッチイベント
	void OnTouchEvent(
		const eTouchEvent in_type, std::vector<_touch_event_data_>& in_datas) override final
	{
		if (!this->_enable)
			return;

		switch (in_type)
		{
		case eTouchEvent::TOUCH_EVENT_MODEL:
		{
#if _DEBUG
			for (auto it = in_datas.begin(); it != in_datas.end(); ++it)
				printf("input player: model handle = %ul\n", it->_touch_model.handle);
#endif

			break;
		}
		default:
			break;
		}
	}

	// アンドゥ入力しているか
	const bool IsUndo()
	{
		if (!this->_enable)
			return false;

		// 入力したテキストから石を置く
		const char* inputText = this->_p_keyboard->GetConfirmEnterText();
		return ((strlen(inputText) == 1) && (inputText[0] == 'u'));
	}

	// 石を置く
	const StoneHandComponent::eResultPlacementStone ExecutePlacementStone()
	{
		if (!this->_enable)
			return StoneHandComponent::eResultPlacementStone_Idle;

		return StoneHandComponent::eResultPlacementStone_Idle;
	}

	void Enable(const bool in_flag) { this->_enable = in_flag; }

private:
	void _Clear()
	{
		this->_p_keyboard = NULL;
		this->_enable = false;
	}

	KeyboardInterface* _p_keyboard;
	bool _enable;
};
#endif

PlayerComponent::PlayerComponent(Actor* in_pOwner, KeyboardInterface* in_pKeyboard)
	: Component(in_pOwner)
{
	this->_Clear();

	this->_p_keyboard = in_pKeyboard;

	this->_p_textAnimationComponent = new TextAnimationComponent(in_pOwner);

	// TODO: モデルをクリック検知するコンポーネント追加
	this->_p_inputComponent = new PlayerInputComponent(in_pOwner, in_pKeyboard);
	this->_p_inputComponent->Enable(false);
}

PlayerComponent::~PlayerComponent()
{
	this->_pOwner->RemoveComponentAndMemFree(this->_p_textAnimationComponent);
}

void PlayerComponent::Update(const float in_deltaTimeSecond)
{
	StoneHandComponent* pStoneHandComponent = this->_pOwner->GetComponent<StoneHandComponent*>();

	switch (pStoneHandComponent->GetState())
	{
	case StoneHandComponent::eState_Start:
	{
		pStoneHandComponent->Start();

		this->_p_inputComponent->Enable(true);

		this->_state = eState_InputText;
		break;
	}
	case StoneHandComponent::eState_Update:
	{
		switch (this->_state)
		{
		case eState_InputText:
		{
#ifdef __CUI_GAME__
			// キーボードからテキスト入力が確定したか
			if (this->_p_keyboard->IsConfirmText() == false)
			{
				// 表示テキスト生成
				const char* pStoneName = pStoneHandComponent->GetUseStone() == BoardData::eStone_ColorWhite ? "White" : "Black";
				// テキスト入力コンポーネントでもっと分かりやすくする
				// TODO: 全文が表示されない
				pStoneHandComponent->SetObjectText(
					"Player Turn %sStone\n"
					"- InputText description\n"
					"-- [1-8][a-h] => [BoardLineNumber][BoardAlphabet]\n"
					"-- u => Undo\n"
					"- TextInput: %s\n",
					pStoneName,
					this->_p_keyboard->GetConfirmInputText());
			}
			else
			{
				this->_state = eState_TurnBegin;
			}
#else
			this->_state = eState_TurnBegin;
#endif

			break;
		}
		case eState_TurnBegin:
		{
#ifdef __CUI_GAME__
#else
			// 表示テキスト生成
			const char* pStoneName = pStoneHandComponent->GetUseStone() == BoardData::eStone_ColorWhite ? "White" : "Black";
			// テキスト入力コンポーネントでもっと分かりやすくする
			pStoneHandComponent->SetObjectText(
				"Player Turn %sStone\n", pStoneName);
#endif

			// 入力したテキストから石を置く
			/*
			const char* inputText = this->_p_keyboard->GetConfirmEnterText();
			if ((strlen(inputText) == 1) && (inputText[0] == 'u'))
			*/
			if (this->_p_inputComponent->IsUndo())
			{
				// 一つ前の手に戻す
				BoardData::sPoint undoPlacementStonePoint;
				if (pStoneHandComponent->UndoPlacement(undoPlacementStonePoint) == true)
				{
					this->_state = eState_InputText;
				}
				else
				{
					this->_p_textAnimationComponent->StartAnimation(
						"Can't undo / input enter", false);
					this->_state = eState_ErrorMessage;
				}
			}
			else
			{
				// TODO: 石をクリックしているかどうかチェック
				//auto result = pStoneHandComponent->SetPlacementStone(inputText);
				auto result = this->_p_inputComponent->ExecutePlacementStone();
				if (result != StoneHandComponent::eResultPlacementStone_Sucess)
				{
					switch (result)
					{
					case StoneHandComponent::eResultPlacementStone_Idle:
						break;
					case StoneHandComponent::eResultPlacementStone_InputCountMiss:
						this->_p_textAnimationComponent->StartAnimation(
							"There is a mistake in the number of input characters / input enter", false);
						break;

					case StoneHandComponent::eResultPlacementStone_InputTextMiss:
						this->_p_textAnimationComponent->StartAnimation(
							"There is a mistake in the entered text content / input enter", false);
						break;

					case StoneHandComponent::eResultPlacementStone_FlipMiss:
						this->_p_textAnimationComponent->StartAnimation(
							"Can't turn stones over / input enter", false);
						break;

					case StoneHandComponent::eResultPlacementStone_Miss:
						this->_p_textAnimationComponent->StartAnimation(
							"Stones cannot be placed / input enter", false);
						break;
					default:
						assert(0);
						break;
					}

					this->_state = eState_ErrorMessage;
				}
				else
				{
					this->_state = eState_TurnEnd;
				}
			}

			break;
		}
		case eState_TurnEnd:
		{
			// ターン終了
			this->_p_inputComponent->Enable(false);

			pStoneHandComponent->End();
			break;
		}
		case eState_ErrorMessage:
		{
			if (this->_p_textAnimationComponent->IsAnimation() == true)
			{
				pStoneHandComponent->SetObjectText(this->_p_textAnimationComponent->GetText());
				break;
			}

			// 確定入力したらステート終了
			if (this->_p_keyboard->IsConfirmText() == true)
			{
				this->_state = eState_InputText;
			}

			break;
		}
		}

		break;
	}
	}
}

void PlayerComponent::_Clear()
{
	this->_state = eState_InputText;
	this->_p_keyboard = NULL;
	this->_p_textAnimationComponent = NULL;
	this->_p_inputComponent = NULL;
}
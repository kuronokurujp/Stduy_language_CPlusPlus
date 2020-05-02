#include "Component/playerComponent.h"

#include "Actor/actor.h"

#include "Component/stoneHandComponent.h"
#include "Component/textAnimationComponent.h"

#include "Interface/keyboardInterface.h"

PlayerComponent::PlayerComponent(Actor* in_pOwner, KeyboardInterface* in_pKeyboard)
	: Component(in_pOwner)
{
	this->_Clear();

	this->_pKeyboard = in_pKeyboard;

	this->_pTextAnimationComponent = new TextAnimationComponent(in_pOwner);
}

PlayerComponent::~PlayerComponent()
{
	this->_pOwner->RemoveComponentAndMemFree(this->_pTextAnimationComponent);
}

void PlayerComponent::Update(const float in_deltaTimeSecond)
{
	StoneHandComponent* pStoneHandComponent = this->_pOwner->GetComponent<StoneHandComponent*>();

	switch (pStoneHandComponent->GetState())
	{
	case StoneHandComponent::eState_Start:
	{
		pStoneHandComponent->Start();
		this->_state = eState_InputText;
		break;
	}
	case StoneHandComponent::eState_Update:
	{
		switch (this->_state)
		{
		case eState_InputText:
		{
			// キーボードからテキスト入力が確定したか
			if (this->_pKeyboard->IsConfirmText() == false)
			{
				// 表示テキスト生成

				const char* pStoneName = pStoneHandComponent->GetUseStone() == BoardData::eStone_ColorWhite ? "White" : "Black";
				// テキスト入力コンポーネントでもっと分かりやすくする
				pStoneHandComponent->SetObjectText(
					"Player Turn %sStone\n"
					"- InputText description\n"
					"-- [1-8][a-h] => [BoardLineNumber][BoardAlphabet]\n"
					"-- u => Undo\n"
					"- TextInput: %s\n",
					pStoneName,
					this->_pKeyboard->GetConfirmInputText());
			}
			else
			{
				this->_state = eState_TurnBegin;
			}

			break;
		}
		case eState_TurnBegin:
		{
			// 入力したテキストから石を置く
			const char* inputText = this->_pKeyboard->GetConfirmEnterText();
			if ((strlen(inputText) == 1) && (inputText[0] == 'u'))
			{
				// 一つ前の手に戻す
				BoardData::sPoint undoPlacementStonePoint;
				if (pStoneHandComponent->UndoPlacement(undoPlacementStonePoint) == true)
				{
					this->_state = eState_InputText;
				}
				else
				{
					this->_pTextAnimationComponent->StartAnimation(
						"Can't undo / input enter", false);
					this->_state = eState_ErrorMessage;
				}
			}
			else
			{
				auto result = pStoneHandComponent->SetPlacementStone(inputText);
				if (result != StoneHandComponent::eResultPlacementStone_Sucess)
				{
					switch (result)
					{
					case StoneHandComponent::eResultPlacementStone_InputCountMiss:
						this->_pTextAnimationComponent->StartAnimation(
							"There is a mistake in the number of input characters / input enter", false);
						break;

					case StoneHandComponent::eResultPlacementStone_InputTextMiss:
						this->_pTextAnimationComponent->StartAnimation(
							"There is a mistake in the entered text content / input enter", false);
						break;

					case StoneHandComponent::eResultPlacementStone_FlipMiss:
						this->_pTextAnimationComponent->StartAnimation(
							"Can't turn stones over / input enter", false);
						break;

					case StoneHandComponent::eResultPlacementStone_Miss:
						this->_pTextAnimationComponent->StartAnimation(
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
			pStoneHandComponent->End();
			break;
		}
		case eState_ErrorMessage:
		{
			if (this->_pTextAnimationComponent->IsAnimation() == true)
			{
				pStoneHandComponent->SetObjectText(this->_pTextAnimationComponent->GetText());
				break;
			}

			// 確定入力したらステート終了
			if (this->_pKeyboard->IsConfirmText() == true)
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
	this->_pKeyboard = NULL;
	this->_pTextAnimationComponent = NULL;
}
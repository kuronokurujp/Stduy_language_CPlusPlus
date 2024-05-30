#ifndef __DIRECTIONAL_LIGHT_H__
#define __DIRECTIONAL_LIGHT_H__

#include "Math/vector3.h"

namespace Light
{
	class Directional
	{
	public:
		// データは構造体にまとめる
		// この構造体をシェーダーにアドレスで渡す必要があるため
		struct Data
		{
			Math::Vector3 _direction;
			Math::Vector3 _diffuseColor;
			Math::Vector3 _specColor;
		};

		Directional() { this->_Clear(); }

		inline Data* GetDataPtr() { return &this->data; }

	private:
		void _Clear() {}

		Data data;
	};
};

#endif // __DIRECTIONAL_LIGHT_H__

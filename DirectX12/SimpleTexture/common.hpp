#pragma once

namespace Common
{
	/// <summary>
	/// 設定した値をアライメント値に合わせて返す
	/// </summary>
	/// <param name="in_size"></param>
	/// <param name="in_alignment"></param>
	/// <returns></returns>
	static size_t AlignmentedSize(size_t in_size, size_t in_alignment)
	{
		// 元の値にアライメントの値を足すだけだと
		// アライメント値の倍数にピッタリとならないので
		// 元の値がアライメント値からあふれた値を引いている
		return in_size + in_alignment - (in_size % in_alignment);
	}
}

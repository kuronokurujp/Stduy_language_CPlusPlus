#pragma once

namespace Common
{
	/// <summary>
	/// 設定した値をアライメント幅に合わせて返している
	/// </summary>
	/// <param name="in_size"></param>
	/// <param name="in_alignment"></param>
	/// <returns></returns>
	static size_t AlignmentedSize(size_t in_size, size_t in_alignment)
	{
		// in_alignment倍数として返す
		return in_size + in_alignment - (in_size % in_alignment);
	}
}

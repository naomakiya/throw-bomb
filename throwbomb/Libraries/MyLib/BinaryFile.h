/// <summary>
/// バイナリファイルを扱うクラス
/// </summary>
#pragma once

#include <memory>

class BinaryFile
{
public:
	// アクセサ
	const char* GetData() const { return m_data.get(); }
	unsigned int GetSize() const { return m_size; }

	// ファイル名を指定してロード
	static BinaryFile LoadFile(const wchar_t* fileName);

private:
	// データ
	std::unique_ptr<char[]> m_data;

	// サイズ
	unsigned int m_size;
};
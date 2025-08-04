/// <summary>
/// 	BinaryFile.h
/// </summary>
#pragma once

#include <memory>

class BinaryFile
{
protected:

	// データ
	std::unique_ptr<char[]> m_data;

	// サイズ
	unsigned int m_size;

public:

	// ファイルの読み込み
	static std::unique_ptr<BinaryFile> LoadFile(const wchar_t* fileName);

	BinaryFile();

	// ムーブコンストラクタ
	BinaryFile(BinaryFile* in);

	// 取得
	char* GetData() { return m_data.get(); }
	unsigned int GetSize() { return m_size; }
};
#include "pch.h"
#include "BinaryFile.h"
#include <fstream>
#include <assert.h>


std::unique_ptr<BinaryFile> BinaryFile::LoadFile(const wchar_t * fileName)
{
	std::ifstream ifs;

	// ファイルを開く
	ifs.open(fileName, std::ios::in | std::ios::binary);

	// 読み込み失敗時は強制終了
	assert(ifs);

	// ファイル内容の読み込み
	BinaryFile* bin = new BinaryFile();

	ifs.seekg(0, std::fstream::end);
	std::streamoff eofPos = ifs.tellg();
	ifs.clear();
	ifs.seekg(0, std::fstream::beg);
	std::streamoff begPos = ifs.tellg();
	bin->m_size = (unsigned int)(eofPos - begPos);

	bin->m_data.reset(new char[bin->m_size]);

	ifs.read(bin->m_data.get(), bin->m_size);

	// ファイルを閉じる
	ifs.close();

	return std::make_unique<BinaryFile>(bin);
}

BinaryFile::BinaryFile()
{
	m_size = 0;
}

BinaryFile::BinaryFile(BinaryFile* in)
{
	m_data = std::move(in->m_data);
	m_size = in->m_size;
}

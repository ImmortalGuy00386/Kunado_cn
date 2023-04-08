#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <codecvt>
#include <Windows.h>

BOOL PutConsoleW(LPCWSTR lpFormat, ...)
{
	static WCHAR buffer[MAX_PATH] = { 0 };

	va_list args = NULL;
	va_start(args, lpFormat);
	vswprintf_s(buffer, lpFormat, args);
	va_end(args);

	return WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), buffer, lstrlenW(buffer), NULL, NULL);
}

std::locale& GetCVT_UTF8()
{
	static std::locale cvtUTF8 = std::locale
	(
		std::locale::empty(),
		new std::codecvt_utf8
		<
		wchar_t,
		0x10ffff,
		std::codecvt_mode(std::consume_header | std::generate_header | std::little_endian)
		>
	);

	return cvtUTF8;
}

bool GetAllFileNameW(std::wstring wsFolder, std::vector<std::wstring>& vecList)
{
	WIN32_FIND_DATAW findData = { 0 };

	HANDLE hFind = FindFirstFileW((wsFolder + L"*").c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE) { return false; }

	do
	{
		if (!wcscmp(findData.cFileName, L".") || !wcscmp(findData.cFileName, L"..")) { continue; }
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;

		vecList.emplace_back(findData.cFileName);

	} while (FindNextFileW(hFind, &findData));

	FindClose(hFind);
	return true;
}

class TextStruct
{
public:
	std::wstring m_wsRaw;
	std::wstring m_wsTra;
};

std::wstring GetName(std::wstring& wsLine)
{
	if (wsLine[0] != L';')
	{
		std::cout << "Not Line!\n";
		return L"";
	}

	size_t pos = wsLine.find(L"\">>\"");
	if (pos != std::wstring::npos)
	{
		return wsLine.substr(3, pos - 3);
	}

	std::cout << "Get GetName Error!\n";
	return L"";
}

std::wstring GetMsg(std::wstring& wsLine)
{
	if (wsLine[0] != L';')
	{
		std::cout << "Not Line!\n";
		return L"";
	}

	size_t pos = wsLine.find(L"\">>\"");
	if (pos != std::wstring::npos)
	{
		pos += std::size(L"\">>\"") - 1;
		return wsLine.substr(pos, wsLine.size() - pos - 1);
	}

	std::cout << "Get GetLineName Error!\n";
	return L"";
}

std::wstring GetParameterValue(std::wstring& wsCode,  std::wstring wsPara)
{
	if (wsCode[0] != L'@')
	{
		std::cout << "Not Code!\n";
		return L"";
	}

	size_t endPos = 0;
	size_t begPos = wsCode.find(wsPara);
	if (begPos != std::wstring::npos)
	{
		begPos += wsPara.size() + 1;
		endPos = wsCode.find(L"\"", begPos);
		if (endPos != std::wstring::npos)
		{
			return wsCode.substr(begPos, endPos - begPos);
		}
	}

	std::cout << "Get Parameter Value Error!\n";
	return L"";
}

bool ReadReplaceText(std::wstring wsFileName, std::unordered_map<std::wstring, std::wstring>& mapLine)
{
	std::wifstream wifText(wsFileName);
	wifText.imbue(GetCVT_UTF8());
	if (!wifText.is_open())
	{
		std::cout << "Open File Error!\n";
		return false;
	}
	
	std::wstring text;
	std::wstring mes_no;
	for (std::wstring line; getline(wifText, line);)
	{
		size_t pos = line.find(L"\" => \"");
		if (pos != std::wstring::npos)
		{
			line.erase(line.size() - 2, 2);
			mes_no = line.substr(2, pos - 2);
			text = line.substr(pos + 6);
			mapLine.insert(std::pair<std::wstring, std::wstring>(mes_no, text));
		}
	}

	return true;
}

bool ReplaceText(
	std::wstring wsFileName,
	std::vector<TextStruct>& vecText,
	std::unordered_map<std::wstring, std::wstring>& mapTraText,
	std::unordered_map<std::wstring, std::wstring>& mapTraName,
	std::unordered_map<std::wstring, std::wstring>& mapTraSelect,
	std::unordered_map<std::wstring, std::wstring>& mapTraTitle)
{
	std::wifstream wifText(wsFileName);
	wifText.imbue(GetCVT_UTF8());
	if (!wifText.is_open())
	{
		std::cout << "Open File Error!\n";
		return false;
	}

	TextStruct MsgStruct;
	TextStruct NameStruct;
	TextStruct SelectStruct;
	TextStruct TitleStruct;
	for (std::wstring line; getline(wifText, line);)
	{
		if (line.find(L"; \"") == 0)
		{
			NameStruct.m_wsRaw = GetName(line);
			MsgStruct.m_wsRaw = GetMsg(line);
			getline(wifText, line);
			MsgStruct.m_wsTra = mapTraText.find(GetParameterValue(line, L"mes_no="))->second;
			NameStruct.m_wsTra = mapTraName.find(GetParameterValue(line, L"name_no="))->second;

			vecText.emplace_back(NameStruct);
			vecText.emplace_back(MsgStruct);
			continue;
		}

		//if (line.find(L"@api_set_message_select") == 0)
		//{
		//	SelectStruct.m_wsRaw = mapTraSelect.find(GetParameterValue(line, L"s0="))->second;
		//	SelectStruct.m_wsTra = SelectStruct.m_wsRaw;

		//	vecText.emplace_back(SelectStruct);
		//	continue;
		//}

		//if (line.find(L"@api_set_window_title_create") == 0)
		//{
		//	TitleStruct.m_wsRaw = mapTraTitle.find(GetParameterValue(line, L"title="))->second;
		//	TitleStruct.m_wsTra = TitleStruct.m_wsRaw;

		//	vecText.emplace_back(TitleStruct);
		//	continue;
		//}
	}

	return true;
}

bool WriteText(std::wstring wsFileName, std::vector<TextStruct>& vecText)
{
	std::wofstream wofText(wsFileName);
	wofText.imbue(GetCVT_UTF8());
	if (!wofText.is_open())
	{
		std::cout << "Open File Error!\n";
		return false;
	}

	for (auto& ite : vecText)
	{
		if (!ite.m_wsRaw.empty())
		{
			wofText
				<< L"Raw:" << ite.m_wsRaw << L'\n'
				<< L"Tra:" << ite.m_wsTra << L"\n\n";
		}
	}

	wofText.flush();

	return true;
}

void Extract()
{
	std::unordered_map<std::wstring, std::wstring> mapTraTitle;
	ReadReplaceText(L"lang_zhcn/_titleLang.ks", mapTraTitle);

	std::unordered_map<std::wstring, std::wstring> mapTraSelect;
	ReadReplaceText(L"lang_zhcn/_selectLang.ks", mapTraSelect);

	std::unordered_map<std::wstring, std::wstring> mapTraName;
	ReadReplaceText(L"lang_zhcn/_nameLang.ks", mapTraName);

	std::vector<std::wstring> vecRawFiles;
	GetAllFileNameW(L"main\\", vecRawFiles);

	for (auto& fileName : vecRawFiles)
	{
		std::unordered_map<std::wstring, std::wstring> mapTraText;
		ReadReplaceText(L"lang_zhcn/" + fileName.substr(0, fileName.size() - 3) + L"_lang.tjs", mapTraText);

		std::vector<TextStruct> vecText;
		ReplaceText(L"main/" + fileName, vecText, mapTraText, mapTraName, mapTraSelect, mapTraTitle);

		WriteText(L"out/" + fileName + L".txt", vecText);
	}
}

bool ReadPS3Text(
	std::wstring wsFileName, 
	std::vector<std::wstring>& vecInfo, 
	std::vector<TextStruct>& vecPS3Text,
	std::unordered_map<std::wstring, std::wstring>& mapKSText)
{
	std::wifstream wifText(wsFileName);
	wifText.imbue(GetCVT_UTF8());
	if (!wifText.is_open())
	{
		std::cout << "Open File Error!\n";
		return false;
	}
	
	TextStruct text;
	for (std::wstring line;getline(wifText, line);)
	{
		if (line.find(L"[Text:") != 0) continue;

		vecInfo.emplace_back(line);

		getline(wifText, text.m_wsRaw);
		if (text.m_wsRaw.find(L"Raw:") != 0)
		{
			std::cout << "Read Raw: ERROR!\n";
			return false;
		}

		getline(wifText, text.m_wsTra);
		if (text.m_wsTra.find(L"Tra:") != 0)
		{
			std::cout << "Read Tra: ERROR!\n";
			return false;
		}

		auto find = mapKSText.find(text.m_wsRaw);
		if (find == mapKSText.end())
		{
			PutConsoleW(L"File:%s , %s\n", wsFileName.c_str(), text.m_wsRaw.c_str());
			text.m_wsTra = L"Tra:";
		}
		else
		{
			text.m_wsTra = find->second;
		}

		vecPS3Text.emplace_back(text);
	}

	return true;
}

bool ReadKSText(std::wstring wsFileName, std::unordered_map<std::wstring, std::wstring>& mapText)
{
	std::wifstream wifText(wsFileName);
	wifText.imbue(GetCVT_UTF8());
	if (!wifText.is_open())
	{
		std::cout << "Open File Error!\n";
		return false;
	}

	std::wstring line1;
	for (std::wstring line; getline(wifText, line);)
	{
		if (line.find(L"Raw:") != 0) continue;

		getline(wifText, line1);
		if (line1.find(L"Tra:") != 0)
		{
			std::cout << "Read Raw: ERROR!\n";
			return false;
		}

		mapText.insert(std::pair<std::wstring, std::wstring>(line, line1));
	}

	return true;
}

bool WritePS3Text(std::wstring wsFileName, std::vector<std::wstring>& vecInfo, std::vector<TextStruct>& vecPS3Text)
{
	std::wofstream wofText(wsFileName);
	wofText.imbue(GetCVT_UTF8());
	if (!wofText.is_open())
	{
		std::cout << "Open File Error!\n";
		return false;
	}

	size_t count = 0;
	for (auto& text : vecPS3Text)
	{
		wofText
			<< vecInfo[count++] << L'\n'
			<< text.m_wsRaw << L'\n'
			<< text.m_wsTra << L"\n\n";
	}

	wofText.flush();

	return true;
}

void Match()
{
	std::vector<std::wstring> vecOutFiles;
	GetAllFileNameW(L"out\\", vecOutFiles);

	for (auto& fileName : vecOutFiles)
	{
		std::unordered_map<std::wstring, std::wstring> mapKSText;
		if (ReadKSText(L"out/" + fileName, mapKSText))
		{
			std::wstring ps3Name = fileName.substr(0, fileName.size() - 7) + L".ps3.txt";
			std::vector<std::wstring> vecInfo;
			std::vector<TextStruct> vecPS3Text;
			if (ReadPS3Text(L"ps3/" + ps3Name, vecInfo, vecPS3Text, mapKSText))
			{
				WritePS3Text(L"meg/" + ps3Name, vecInfo, vecPS3Text);
			}
		}
	}

	int a = 0;
}

void ReplaceName()
{

}

int main()
{
	Match();
	//Extract();
}
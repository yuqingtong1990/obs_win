#include "stdafx.h"
#include "CqtUtil.h"
#include <algorithm>
#include <Shlwapi.h>
#include <Shlobj.h>
#include <strsafe.h>
#include <time.h>
#include <winhttp.h>
#include <process.h>

#pragma comment(lib,"winhttp.lib")
#pragma comment(lib, "shlwapi.lib")
using namespace std;
#include <winternl.h>

inline void IncPtr(void **p, int i){ *p = (void*)((int)*p + i); }

std::wstring str2wstr( const std::string& str, unsigned int codepage /*=GBK*/)
{
	if (str.empty())
		return L"";
	int iULen = MultiByteToWideChar(codepage, 0, str.c_str(), -1, NULL, 0);
	wchar_t *ustr = (wchar_t*)malloc((iULen)*sizeof(wchar_t));
	if (!ustr)
		return L"";
	memset(ustr, 0, (iULen)*sizeof(wchar_t));
	MultiByteToWideChar(codepage, 0, str.c_str(), -1, ustr, iULen);
	wstring ret;
	ret.assign(ustr);
	free(ustr);
	return ret;
}

std::string wstr2str( const std::wstring& wstr, unsigned int codepage /*=GBK*/)
{
	if (wstr.empty())
		return "";
	int iALen = WideCharToMultiByte(codepage, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	char *astr = (char*)malloc(iALen);
	if (!astr)
		return "";
	memset(astr, 0, iALen);
	WideCharToMultiByte(codepage, 0, wstr.c_str(), -1, astr, iALen, NULL, NULL);
	string ret;
	ret.assign(astr);
	free(astr);
	return ret;
}

std::string gbktoUtf8( const std::string& stransi )
{
	int iULen = MultiByteToWideChar(CP_ACP, 0, stransi.c_str(), -1, NULL, 0);
	wchar_t *ustr = new (std::nothrow) wchar_t[iULen+1];
	if (NULL == ustr)
		return "";

	memset(ustr, 0, (iULen+1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, stransi.c_str(), -1, ustr, iULen);

	iULen = WideCharToMultiByte(CP_UTF8, 0, ustr, -1, NULL, 0, NULL, NULL);

	char *str = new (std::nothrow) char[iULen+1];
	memset(str, 0, (iULen+1)*sizeof(char));

	if (NULL == str)
		return "";
	WideCharToMultiByte(CP_UTF8, 0, ustr, -1, str, iULen, NULL, NULL);

	string strRet(str);
	delete []ustr;
	delete []str;
	return strRet;
}
std::string utf8toansi(const string &utf8)
{
	int iULen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
	wchar_t *ustr = (wchar_t*)malloc((iULen)*sizeof(wchar_t));
	if (!ustr)
		return "";
	memset(ustr, 0, (iULen)*sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, ustr, iULen);
	wstring ret;
	ret.assign(ustr);
	free(ustr);
	return wstr2str(ret);
}

std::string utf8togbk( const std::string& strutf8 )
{
	int iULen = MultiByteToWideChar(CP_UTF8, 0, strutf8.c_str(), -1, NULL, 0);
	wchar_t *ustr = new (std::nothrow) wchar_t[iULen+1];
	if (NULL == ustr)
		return "";

	memset(ustr, 0, (iULen+1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, strutf8.c_str(), -1, ustr, iULen);

	iULen = WideCharToMultiByte(CP_ACP, 0, ustr, -1, NULL, 0, NULL, NULL);
	char *str = new (std::nothrow) char[iULen+1];
	if (NULL == str)
		return "";

	memset(str, 0, (iULen+1)*sizeof(char));

	WideCharToMultiByte(CP_ACP, 0, ustr, -1, str, iULen, NULL, NULL);

	string strRet(str);
	delete []ustr;
	delete []str;
	return strRet;
}

std::string inttostr( int n, int radix /*= 10*/)
{
	char buf[16] = {0};
	_itoa_s(n,buf,radix);
	return string(buf);
}

std::wstring inttowstr( int n, int radix/* = 10 */)
{
	wchar_t buf[16] = { 0 };
	_itow_s(n,buf,radix);
	return wstring(buf);
}

std::string int64tostr(long long n, int radix /*= 10*/)
{
	char buf[64] = { 0 };
	_i64toa_s(n,buf,64,radix);
	return string(buf);
}

std::wstring int64towstr(long long n, int radix /*= 10*/)
{
	wchar_t buf[64] = { 0 };
	_i64tow_s(n,buf,64,radix);
	return wstring(buf);
}

std::string& ReplaceString( std::string &s, const std::string &OldPattern, const std::string &NewPattern )
{
	if (OldPattern == NewPattern)
		return s;
	string::size_type i;
	while (true)
	{
		i = s.find(OldPattern);
		if (i == string::npos)
			break;
		else
			s = s.replace(i, OldPattern.length(), NewPattern);
	}
	return s;
}

std::wstring& ReplaceStringW( std::wstring &s, const std::wstring &OldPattern, const std::wstring &NewPattern )
{
	if (OldPattern == NewPattern)
		return s;
	wstring::size_type i;
	while (true)
	{
		i = s.find(OldPattern);
		if (i == wstring::npos)
			break;
		else
			s = s.replace(i, OldPattern.length(), NewPattern);
	}
	return s;
}

struct CharCmpI
{
	bool operator()(char a, char b){ return (ChrCmpIA(a, b) == 0); }
};

struct CharCmpIW
{
	bool operator()(wchar_t a, wchar_t b){ return (ChrCmpIW(a, b) == 0); }
};

string& ReplaceStringI( string &s, const string &OldPattern, const string &NewPattern )
{
	if (!lstrcmpiA(OldPattern.c_str(), NewPattern.c_str()))
		return s;
	string::iterator it, itend;
	do{
		it = search(s.begin(), s.end(), OldPattern.begin(), OldPattern.end(), CharCmpI());
		if (it != s.end())
		{
			itend = it + OldPattern.length();
			s = s.replace(it, itend, NewPattern);
		}
		else
			break;
	} while (true);
	return s;
}

wstring& ReplaceStringIW( wstring &s, const wstring &OldPattern, const wstring &NewPattern )
{
	if (!lstrcmpiW(OldPattern.c_str(), NewPattern.c_str()))
		return s;
	wstring::iterator it, itend;
	do{
		it = search(s.begin(), s.end(), OldPattern.begin(), OldPattern.end(), CharCmpIW());
		if (it != s.end())
		{
			itend = it + OldPattern.length();
			s = s.replace(it, itend, NewPattern);
		}
		else
			break;
	} while (true);
	return s;
}

void SplitString( const string &AString, const string &ASplitStr, vector<string> &AStrings )
{
	const char *sdata = AString.c_str();
	const char *ssplit = ASplitStr.c_str();
	DWORD i = 0, j = 0, iSizeA = AString.length(), iSizeB = ASplitStr.length();
	AStrings.clear();
	while (i < iSizeA)
	{
		if (i + iSizeB > iSizeA)
		{
			AStrings.push_back(AString.substr(j));
			j = i;
			break;
		}
		if (memcmp(&(sdata[i]), ssplit, iSizeB) == 0)
		{
			//if (j < i)
			AStrings.push_back(AString.substr(j, i - j));
			j = i + iSizeB;
			i += iSizeB;
		}
		else
			++i;
	}
	if (j < i)
		AStrings.push_back(AString.substr(j));
	return;
}

void SplitString( const string &AString, const string &ASplitStr, string &ALeft, string &ARight )
{
	vector<string> vs;
	SplitString(AString, ASplitStr, vs);
	ALeft = vs[0];
	ARight = "";
	if (vs.size() > 1)
	{
		for (size_t i = 1; i != vs.size(); ++i)
		{
			if (ARight.empty())
				ARight = vs[i];
			else
				ARight = ARight + ASplitStr + vs[i];
		}
	}
	else
		ARight = "";
	return;
}

void SplitStringW( const wstring &AString, const wstring &ASplitStr, vector<wstring> &dest )
{
	wstring::size_type pos = wstring::npos;
	wstring::size_type offset = 0;
	dest.clear();
	while (1)
	{
		pos = AString.find(ASplitStr.c_str(), offset);
		if (wstring::npos == pos)
			break;
		dest.push_back(AString.substr(offset, pos - offset));
		offset = ASplitStr.length() + pos;

	}
	if (offset != AString.length())
		dest.push_back(AString.substr(offset));
}

void SplitStringW( const wstring &AString, const wstring &ASplitStr, wstring &ALeft, wstring &ARight )
{
	vector<wstring> vs;
	SplitStringW(AString, ASplitStr, vs);
	ALeft = vs[0];
	if (vs.size() > 1)
	{
		for (size_t i = 1; i != vs.size(); ++i)
		{
			if (ARight.empty())
				ARight = vs[i];
			else
				ARight = ARight + ASplitStr + vs[i];
		}
	}
	else
		ARight = L"";
	return;
}



std::string ConcatString(std::vector<std::string> vecStr)
{
	if (0 == vecStr.size())
		return "";
	string strRet;
	for (unsigned int i = 0; i<vecStr.size(); i++)
	{
		strRet += vecStr[i];
	}
	return strRet;
}

std::wstring ConcatStringW(std::vector<std::wstring> vecStr)
{
	if (0 == vecStr.size())
		return L"";
	wstring strRet;
	for (unsigned int i = 0; i<vecStr.size(); i++)
	{
		strRet += vecStr[i];
	}
	return strRet;
}

std::string IntList2String(const std::vector<int> &AStringList, const std::string &AConnectStr)
{
	std::string ret;
	for (vector<int>::const_iterator it = AStringList.begin(); it != AStringList.end(); ++it)
	{
		std::string str = _I2A_(*it);
		if (ret.empty())
			ret = str;
		else
			ret = ret + AConnectStr + str;
	}
	return ret;
}

std::wstring IntList2StringW(const std::vector<int> &AStringList, const std::wstring &AConnectStr)
{
	wstring ret;
	for (vector<int>::const_iterator it = AStringList.begin(); it != AStringList.end(); ++it)
	{
		std::wstring wstr = _I2W_(*it);
		if (ret.empty())
			ret = wstr;
		else
			ret = ret + AConnectStr + wstr;
	}
	return ret;
}

std::string StringList2String( const vector<string> &AStringList, const string &AConnectStr )
{
	string ret;
	for (vector<string>::const_iterator it = AStringList.begin(); it != AStringList.end(); ++it)
	{
		if (ret.empty())
			ret = *it;
		else
			ret = ret + AConnectStr + *it;
	}
	return ret;
}

std::string StringList2String( const list<string> &AStringList, const string &AConnectStr )
{
	string ret;
	for (list<string>::const_iterator it = AStringList.begin(); it != AStringList.end(); ++it)
	{
		if (ret.empty())
			ret = *it;
		else
			ret = ret + AConnectStr + *it;
	}
	return ret;
}

wstring StringList2StringW( const vector<wstring> &AStringList, const wstring &AConnectStr )
{
	wstring ret;
	for (vector<wstring>::const_iterator it = AStringList.begin(); it != AStringList.end(); ++it)
	{
		if (ret.empty())
			ret = *it;
		else
			ret = ret + AConnectStr + *it;
	}
	return ret;
}

wstring StringList2StringW( const list<wstring> &AStringList, const wstring &AConnectStr )
{
	wstring ret;
	for (list<wstring>::const_iterator it = AStringList.begin(); it != AStringList.end(); ++it)
	{
		if (ret.empty())
			ret = *it;
		else
			ret = ret + AConnectStr + *it;
	}
	return ret;
}

std::string StrFormat( const char *fmt,... )
{
	std::string strResult="";
	if (NULL != fmt)
	{
		va_list marker = NULL;            
		va_start(marker, fmt);                            //初始化变量参数 
		size_t nLength = _vscprintf(fmt, marker) + 1;    //获取格式化字符串长度
		std::vector<char> vBuffer(nLength, '\0');        //创建用于存储格式化字符串的字符数组
		int nWritten = _vsnprintf_s(&vBuffer[0], vBuffer.size(), nLength, fmt, marker);
		if (nWritten>0)
		{
			strResult = &vBuffer[0];
		}            
		va_end(marker);                                    //重置变量参数
	}
	return strResult; 
}

wstring StrFormatW( const wchar_t *fmt,... )
{
	std::wstring strResult=L"";
	if (NULL != fmt)
	{
		va_list marker = NULL;            
		va_start(marker, fmt);                            //初始化变量参数
		size_t nLength = _vscwprintf(fmt, marker) + 1;    //获取格式化字符串长度
		std::vector<wchar_t> vBuffer(nLength, L'\0');    //创建用于存储格式化字符串的字符数组
		int nWritten = _vsnwprintf_s(&vBuffer[0], vBuffer.size(), nLength, fmt, marker); 
		if (nWritten > 0)
		{
			strResult = &vBuffer[0];
		}
		va_end(marker);                                    //重置变量参数
	}
	return strResult; 
}

std::string upperStr( const std::string &sIn )
{
	std::string sOut(sIn);
	transform(sIn.begin(), sIn.end(), sOut.begin(), toupper);
	return sOut;
}

std::wstring upperStr( const std::wstring &wsIn )
{
	std::wstring wsOut(wsIn);
	transform(wsIn.begin(), wsIn.end(), wsOut.begin(), toupper);
	return wsOut;
}

std::string lowerStr( const std::string &sIn )
{
	std::string sOut(sIn);
	transform(sIn.begin(), sIn.end(), sOut.begin(), tolower);
	return sOut;
}

std::wstring lowerStr( const std::wstring &wsIn )
{
	std::wstring wsOut(wsIn);
	transform(wsIn.begin(), wsIn.end(), wsOut.begin(), tolower);
	return wsOut;
}


#define int2hex(a) ((a)<10?(a)+'0':(a)+'a'-10)
#define hex2int(a) ((a)>='a'?(a)-'a'+10:(a)-'0')

static char char2print(char a)
{
	if ((a >= 32) && (a <= 126))return a;
	return '.';
}

static std::string bin2print(const std::string &in, int size)
{
	const char *buf = in.c_str();
	std::string out("");
	std::string inp("");
	for (unsigned int i = 0; i < (unsigned)size; i++)
	{
		unsigned char tmp = i < in.length() ? buf[i] : ' ';
		inp += char2print(tmp);
		out += i < in.length() ? int2hex(tmp >> 4) : ' ';
		out += i < in.length() ? int2hex(tmp & 0x0f) : ' ';
		out += ' ';
		if ((i % 4) == 3) out += ' ';
	}
	out += inp + "\r\n";
	return out;
}

std::string bin2hex( const std::string &in, int size /*= 0*/ )
{
	const char *buf = in.c_str();
	std::string out("");
	int i = 0, j = 0;
	for (i; (unsigned)i < in.length(); i++)
	{
		char tmp = buf[i];
		if (!size)
		{
			out += int2hex((tmp >> 4) & 0x0f);
			out += int2hex(tmp & 0x0f);
		}
		else
		{
			out += bin2print(in.substr(i, size), size);
			i += size - 1;
		}
	}

	return out;
}

std::string hex2bin( const std::string &in )
{
	const char *buf = in.c_str();
	std::string out("");
	int i = 0;
	for (i; (unsigned)i < in.length(); i += 2)
	{
		if (((buf[i] >= '0' && buf[i] <= '9') || (buf[i] >= 'a' && buf[i] <= 'f'))
			&& ((buf[i + 1] >= '0' && buf[i + 1] <= '9') || (buf[i + 1] >= 'a' && buf[i + 1] <= 'f'))){
				char tmp = hex2int(buf[i]);
				char tmp1 = hex2int(buf[i + 1]);
				tmp = (tmp << 4) + tmp1;
				out += tmp;
		}
	}
	return out;
}

void GetCommandLineList(std::vector<string> &lst)
{
	LPCSTR cstr = GetCommandLineA();    
	std::string s(cstr);  
	bool bSub = false;
	std::string sSub = "";
	lst.clear();
	for(int i=0; i!=s.length(); ++i)
	{
		if ('\"' == s[i])
		{
			if (!bSub)
			{
				bSub = true;
				if (sSub!="")
				{
					lst.push_back(sSub);
					sSub = "";
				}
				continue;
			}
			else
			{
				bSub = false;
				lst.push_back(sSub);
				sSub = "";
			}
		}
		else if (' ' == s[i])
		{
			if (bSub)
			{
				sSub += s[i];
				continue;
			}
			else //not bsub
			{
				if (sSub!="")        
				{
					lst.push_back(sSub);                 
					sSub = "";
				}
				else
					continue;        
			}
		}
		else
			sSub += s[i];
	}
	if (sSub != "")
		lst.push_back(sSub);
}

int ParamCount()
{
	std::vector<string> lstCmdline;
	GetCommandLineList(lstCmdline);
	return lstCmdline.size();
}
string ParamStr(size_t i)
{
	std::vector<string> lstCmdline;
	GetCommandLineList(lstCmdline);
	if (i >= lstCmdline.size() || i<0)
		return "";
	else
		return lstCmdline[i];

	return "";  
}

wstring ParamStrW(size_t i)
{
	return _A2W_(ParamStr(i));
}

std::string delutf8Bom( const std::string& strutf8 )
{
	//-17 -69 -65
	if(strutf8[0] == -17 && strutf8[1] == -69 && strutf8[2] == -65) //处理UTF8文件头
	{ 
		return strutf8.substr(3,strutf8.size());
	}
	return strutf8;

}

std::wstring GetAppDataPath()
{
	wchar_t wcPath[MAX_PATH] = {0};   
	if ( SHGetSpecialFolderPathW( NULL, wcPath, CSIDL_APPDATA, FALSE ) == TRUE){
		return wstring(wcPath);
	}
	return L"";
}

std::wstring GetDesktopPath()
{
	wchar_t sDesktopPath[MAX_PATH] = {0};
	if( SHGetSpecialFolderPath( NULL, sDesktopPath, CSIDL_DESKTOPDIRECTORY, FALSE ) == TRUE ) {
		return wstring(sDesktopPath);
	}
	return L"";
}

std::wstring GetAllUserDesktopPath()
{
	wchar_t sDesktopPath[MAX_PATH] = {0};
	if( SHGetSpecialFolderPath( NULL, sDesktopPath, CSIDL_COMMON_DESKTOPDIRECTORY, FALSE ) == TRUE ) {
		return wstring(sDesktopPath);
	}
	return L"";
}

std::wstring GetQuickLaunckPath()
{
	std::wstring csPath = GetAppDataPath();
	csPath += (L"\\Microsoft\\Internet Explorer\\Quick Launch");
	return csPath;
}

std::wstring GeTaskbarPath()
{
	std::wstring csPath = GetQuickLaunckPath();
	csPath += (L"\\User Pinned\\TaskBar");
	return csPath;
}

std::wstring GetExeRunPath()
{
	wchar_t strCurPath[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, strCurPath, MAX_PATH);
	wstring wsCurPath = ExtractFilePathW(strCurPath, true);
	return wsCurPath;
}

std::wstring trimW( const std::wstring &str )
{
	if (str.empty())
		return str;  
	string::size_type i=-1,j=-1;
	UINT k;
	for(k=0;k<str.length();++k)
	{
		if (unsigned(str[k])>unsigned(L' '))
		{
			i = k;
			break;
		}    
	}
	for(k=str.length()-1;k>=0;--k)
	{
		if (unsigned(str[k])>unsigned(L' '))
		{
			j = k;
			break;
		}    
	}
	i = (i==-1?0:i);
	j = (j==-1?str.length()-1:j);
	return str.substr(i,j-i+1); 
}

std::wstring ExcludeTrailingPathDelimiterW( const std::wstring &path )
{
	wstring s = trimW(path);
	if (s.empty())
		return s;
	if (s[s.length()-1] == L'\\')
		return s.substr(0,s.length()-1);
	else
		return s;
}

std::wstring IncludeTrailingPathDelimiterW( const std::wstring &path )
{
	wstring s = trimW(path);
	if (s.empty())
		return s;
	if (s[s.length()-1] != L'\\')
		return s+L"\\";
	else
		return s;  
}

std::wstring ExtractFilePathW( const std::wstring &filestr, bool IncludeBackslash )
{
	if (filestr.empty())
		return L"";
	for(int i = filestr.length()-1; i>=0; --i)
	{
		if (filestr[i] == L'\\')
		{
			if (IncludeBackslash) 
				return filestr.substr(0,i+1);
			else
				return filestr.substr(0,i);
		}
	}
	return L"";
}

std::wstring ExtractFileNameW( const std::wstring &filestr )
{
	if (filestr.empty())
		return L"";
	for(int i = filestr.length()-1; i>=0; --i)
	{
		if (filestr[i] == L'\\')
		{
			return filestr.substr(i+1);
		}
	}
	return L"";
}

std::wstring ExtractUrlNameW(const std::wstring &urlstr)
{
	if (urlstr.empty())
		return L"";
	for(int i = urlstr.length()-1; i>=0; --i)
	{
		if (urlstr[i] == L'/')
		{
			return urlstr.substr(i+1);
		}
	}
	return L"";
}

 std::wstring ExtractExtensionsW(const std::wstring &filestr, bool IncludePoint /*= true*/)
{
	if (filestr.empty())
		return L"";
	for(int i = filestr.length()-1; i>=0; --i)
	{
		if (filestr[i] == L'.')
		{
			if (IncludePoint)
			{
				return filestr.substr(i);
			}
			else
			{
				return filestr.substr(i+1);
			}	
		}
	}
	return L"";
}

bool FileExistsW( const std::wstring &fn )
{
	DWORD Code = GetFileAttributesW(fn.c_str());
	if (Code == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}
	else
	{
		if ((Code & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
		
	}
}

bool DirectoryExistsW( const std::wstring &fn )
{
	DWORD Code = GetFileAttributesW(fn.c_str());
	return (Code != INVALID_FILE_ATTRIBUTES) && ((FILE_ATTRIBUTE_DIRECTORY & Code) != 0);
}

bool ForceDirectoryW( const std::wstring &fn )
{
	bool ret = true;
	if (fn.empty())
		return false;
	wstring sdir = ExcludeTrailingPathDelimiterW(fn);
	if (sdir.length() < 3 || DirectoryExistsW(sdir) || (ExtractFilePathW(sdir,true) == sdir))
		return ret;
	return ForceDirectoryW(ExtractFilePathW(sdir,true)) && CreateDirectoryW(sdir.c_str(), NULL);
}

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383


bool RegEnumKeys(HKEY key, wstring subkey,/*out*/vector<wstring> &keys,/*out*/vector<wstring> &values)
{
	HKEY hKey;
	if( RegOpenKeyExW( key,
		subkey.c_str(),
		0,
		KEY_READ,
		&hKey) == ERROR_SUCCESS
		)
	{
		keys.clear();
		values.clear();
		TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
		DWORD    cbName;                   // size of name string 
		TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
		DWORD    cchClassName = MAX_PATH;  // size of class string 
		DWORD    cSubKeys=0;               // number of subkeys 
		DWORD    cbMaxSubKey;              // longest subkey size 
		DWORD    cchMaxClass;              // longest class string 
		DWORD    cValues;              // number of values for key 
		DWORD    cchMaxValue;          // longest value name 
		DWORD    cbMaxValueData;       // longest value data 
		DWORD    cbSecurityDescriptor; // size of security descriptor 
		FILETIME ftLastWriteTime;      // last write time 

		DWORD i, retCode; 

		TCHAR  achValue[MAX_VALUE_NAME]; 
		DWORD cchValue = MAX_VALUE_NAME; 

		// Get the class name and the value count. 
		retCode = RegQueryInfoKey(
			hKey,                    // key handle 
			achClass,                // buffer for class name 
			&cchClassName,           // size of class string 
			NULL,                    // reserved 
			&cSubKeys,               // number of subkeys 
			&cbMaxSubKey,            // longest subkey size 
			&cchMaxClass,            // longest class string 
			&cValues,                // number of values for this key 
			&cchMaxValue,            // longest value name 
			&cbMaxValueData,         // longest value data 
			&cbSecurityDescriptor,   // security descriptor 
			&ftLastWriteTime);       // last write time 

		// Enumerate the subkeys, until RegEnumKeyEx fails.

		if (cSubKeys)
		{
			//printf( "\nNumber of subkeys: %d\n", cSubKeys);

			for (i=0; i<cSubKeys; i++) 
			{ 
				cbName = MAX_KEY_LENGTH;
				retCode = RegEnumKeyEx(hKey, i,
					achKey, 
					&cbName, 
					NULL, 
					NULL, 
					NULL, 
					&ftLastWriteTime); 
				if (retCode == ERROR_SUCCESS) 
				{
					//_tprintf(TEXT("(%d) %s\n"), i+1, achKey);
					keys.push_back(achKey);
				}
			}
		} 

		// Enumerate the key values. 

		if (cValues) 
		{
			// printf( "\nNumber of values: %d\n", cValues);

			for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
			{ 
				cchValue = MAX_VALUE_NAME; 
				achValue[0] = '\0'; 
				retCode = RegEnumValue(hKey, i, 
					achValue, 
					&cchValue, 
					NULL, 
					NULL,
					NULL,
					NULL);

				if (retCode == ERROR_SUCCESS ) 
				{ 
					values.push_back(achValue);
				} 
			}
		}

		RegCloseKey(hKey);
	}  
	else
		return false;

	return true;
}

bool RegKeyExists(HKEY key, wstring subkey)
{
	HKEY hKey;
	bool ret = (RegOpenKeyExW(key,
		subkey.c_str(),
		0,
		KEY_READ,
		&hKey) == ERROR_SUCCESS
		);
	if (ret)
		RegCloseKey(hKey);
	return ret;
}

bool RegValueExists(HKEY key, wstring subkey, wstring valuname)
{
	HKEY hKey;
	bool bIsExist = false;
	do 
	{
		if (ERROR_SUCCESS != RegOpenKeyExW(key, subkey.c_str(), 0, KEY_READ, &hKey))
			break;
		DWORD rtype, rsize;
		if (ERROR_SUCCESS != RegQueryValueExW(hKey,valuname.c_str(), 0, &rtype, NULL, &rsize))
			break;
		bIsExist = true;
	} while (0);
	RegCloseKey(hKey);
	return bIsExist;
}

DWORD RegGetValueSize(HKEY key, wstring subkey, wstring valuename)
{
	HKEY hKey;
	DWORD dwReturn = 0;
	do
	{
		if (RegOpenKeyExW(key, subkey.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
			break;
		DWORD rtype, rsize;
		if (ERROR_SUCCESS == RegQueryValueExW(hKey, valuename.c_str(), 0, &rtype, NULL, &rsize))
			dwReturn = rsize;
		else
			break;
	} while (0);
	RegCloseKey(hKey);
	return dwReturn;
}

std::wstring RegReadStringValue(HKEY key, wstring subkey, wstring valuename, wstring defaultret)
{
	HKEY hKey;
	wchar_t *ret = NULL;
	do
	{
		if (RegOpenKeyExW(key, subkey.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
			break;
		DWORD rtype, rsize;
		if (ERROR_SUCCESS == RegQueryValueExW(hKey, valuename.c_str(), 0, &rtype, NULL, &rsize))
		{
			if (REG_SZ != rtype)
				break;
			ret = (wchar_t*)malloc(rsize + 2);
			memset(ret, 0, rsize + 2);
			if (ERROR_SUCCESS == RegQueryValueExW(hKey, valuename.c_str(), 0, &rtype, (LPBYTE)ret, &rsize))
				defaultret.assign(ret);
		}
	} while (0);
	free(ret);
	return defaultret;
}

bool RegReadBinaryValue(HKEY key, wstring subkey, wstring valuename,/*in/out*/unsigned char *ret, DWORD &retsize)
{
	HKEY hKey;
	bool bSuccess = false;
	do
	{
		if (RegOpenKeyExW(key, subkey.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
			break;
		DWORD rsize = retsize;
		if (ERROR_SUCCESS == RegQueryValueExW(hKey, valuename.c_str(), 0, NULL, (LPBYTE)ret, &rsize))
			bSuccess = true;
	} while (0);
	RegCloseKey(hKey);
	return bSuccess;
}

DWORD RegReadDWORDValue(HKEY key, wstring subkey, wstring valuename, DWORD defaultret)
{
	HKEY hKey;
	do
	{
		if (RegOpenKeyExW(key, subkey.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
			break;
		DWORD rtype, rsize, ret;
		if (ERROR_SUCCESS == RegQueryValueExW(hKey, valuename.c_str(), 0, &rtype, NULL, &rsize))
		{
			if (REG_DWORD != rtype)
				break;
			if (ERROR_SUCCESS == RegQueryValueExW(hKey, valuename.c_str(), 0, &rtype, (LPBYTE)(&ret), &rsize))
				defaultret = ret;
		}
	} while (0);
	::RegCloseKey(hKey);
	return defaultret;
}

bool RegWriteStringValue(HKEY key, wstring subkey, wstring valuename, wstring valuedata)
{
	HKEY hKey;
	bool bSuccess = false;
	do
	{
		if (RegOpenKeyExW(key, subkey.c_str(), 0, KEY_READ | KEY_WRITE, &hKey) != ERROR_SUCCESS)
			break;
		if (ERROR_SUCCESS == RegSetValueExW(hKey, valuename.c_str(), 0, REG_SZ, (const BYTE*)valuedata.c_str(), valuedata.length() * 2 + 4))
			bSuccess = true;

	} while (0);
	::RegCloseKey(hKey);
	return bSuccess;
}

bool RegWriteBinaryValue(HKEY key, wstring subkey, wstring valuename, unsigned char *valuedata, DWORD datasize)
{
	HKEY hKey;
	bool bSuccess = false;
	do
	{
		if (RegOpenKeyExW(key, subkey.c_str(), 0, KEY_READ | KEY_WRITE, &hKey) != ERROR_SUCCESS)
			break;
		if (ERROR_SUCCESS == RegSetValueExW(hKey, valuename.c_str(), 0, REG_SZ, valuedata, datasize))
			bSuccess = true;
	} while (0);
	::RegCloseKey(hKey);
	return bSuccess;
}

bool RegWriteDWORDValue(HKEY key, wstring subkey, wstring valuename, DWORD valuedata)
{
	HKEY hKey;
	bool bSuccess = false;
	do
	{
		if (RegOpenKeyExW(key, subkey.c_str(), 0, KEY_READ | KEY_WRITE, &hKey) != ERROR_SUCCESS)
			break;
		if (ERROR_SUCCESS == RegSetValueExW(hKey, valuename.c_str(), 0, REG_SZ, (const BYTE*)(&valuedata), sizeof(DWORD)))
			bSuccess = true;
	} while (0);
	::RegCloseKey(hKey);
	return bSuccess;
}

bool RegRemoveValue(HKEY key, wstring subkey, wstring valuename)
{
	HKEY hKey;
	bool bSuccess = false;
	do
	{
		if (RegOpenKeyExW(key, subkey.c_str(), 0, KEY_READ | KEY_WRITE, &hKey) != ERROR_SUCCESS)
			break;
		if (ERROR_SUCCESS == RegDeleteValueW(hKey, valuename.c_str()))
			bSuccess = true;
	} while (0);
	::RegCloseKey(hKey);
	return bSuccess;
}

BOOL RegDelnodeRecurse(HKEY hKeyRoot, LPTSTR lpSubKey)
{
	LPTSTR lpEnd;
	LONG lResult;
	DWORD dwSize;
	TCHAR szName[MAX_PATH];
	HKEY hKey;
	FILETIME ftWrite;

	// First, see if we can delete the key without having
	// to recurse.

	lResult = RegDeleteKey(hKeyRoot, lpSubKey);

	if (lResult == ERROR_SUCCESS)
		return TRUE;

	lResult = RegOpenKeyEx(hKeyRoot, lpSubKey, 0, KEY_READ, &hKey);

	if (lResult != ERROR_SUCCESS)
	{
		if (lResult == ERROR_FILE_NOT_FOUND) {
			printf("Key not found.\n");
			return TRUE;
		}
		else {
			printf("Error opening key.\n");
			return FALSE;
		}
	}

	// Check for an ending slash and add one if it is missing.

	lpEnd = lpSubKey + lstrlen(lpSubKey);

	if (*(lpEnd - 1) != TEXT('\\'))
	{
		*lpEnd = TEXT('\\');
		lpEnd++;
		*lpEnd = TEXT('\0');
	}

	// Enumerate the keys

	dwSize = MAX_PATH;
	lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
		NULL, NULL, &ftWrite);

	if (lResult == ERROR_SUCCESS)
	{
		do {

			StringCchCopy(lpEnd, MAX_PATH * 2, szName);

			if (!RegDelnodeRecurse(hKeyRoot, lpSubKey)) {
				break;
			}

			dwSize = MAX_PATH;

			lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
				NULL, NULL, &ftWrite);

		} while (lResult == ERROR_SUCCESS);
	}

	lpEnd--;
	*lpEnd = TEXT('\0');

	RegCloseKey(hKey);

	// Try again to delete the key.

	lResult = RegDeleteKey(hKeyRoot, lpSubKey);

	if (lResult == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

BOOL RegDelnode(HKEY hKeyRoot, LPTSTR lpSubKey)
{
	TCHAR szDelKey[2 * MAX_PATH];

	StringCchCopy(szDelKey, MAX_PATH * 2, lpSubKey);
	return RegDelnodeRecurse(hKeyRoot, szDelKey);

}

bool RegRemoveKey(HKEY key, wstring subkey)
{
	return (TRUE == RegDelnode(key, (LPTSTR)(subkey.c_str())));
}

bool RegNewKey(HKEY key, wstring subkey)
{
	HKEY ret;
	bool bret = (ERROR_SUCCESS == RegCreateKeyExW(key, subkey.c_str(), 0, 0, 0, 0, 0, &ret, 0));
	if (bret)
		RegCloseKey(ret);
	return bret;
}

bool Str2Datetime(const string &str, SYSTEMTIME &ret)
{
	memset(&ret, 0, sizeof(SYSTEMTIME));
	//YYYY-MM-DD HH:NN:SS:ZZZ
	if (str.length() < strlen("YYYY-MM-DD"))
		return false;
	string sY, sM, sD, sH, sN, sS, sZ;
	int iY = 0, iM = 0, iD = 0, iH = 0, iN = 0, iS = 0, iZ = 0;
	sY = str.substr(0, 4);
	iY = atoi(sY.c_str());
	if (0 == iY)
		return false;
	sM = str.substr(5, 2);
	iM = atoi(sM.c_str());
	if (0 == iM || 12 < iM)
		return false;
	sD = str.substr(8, 2);
	iD = atoi(sD.c_str());
	if (0 == iD || 31 < iD)
		return false;
	if (str.length() >= 13)
		sH = str.substr(11, 2);
	if (str.length() >= 16)
		sN = str.substr(14, 2);
	if (str.length() >= 19)
		sS = str.substr(17, 2);
	if (str.length() >= 23)
		sZ = str.substr(20, 3);
	iH = atoi(sH.c_str());
	if (iH > 23)
		iH = 0;
	iN = atoi(sN.c_str());
	if (iN > 59)
		iN = 0;
	iS = atoi(sS.c_str());
	if (iS > 59)
		iS = 0;
	iZ = atoi(sZ.c_str());
	if (iZ > 999)
		iZ = 0;

	ret.wYear = (WORD)iY;
	ret.wMonth = (WORD)iM;
	ret.wDay = (WORD)iD;
	ret.wHour = (WORD)iH;
	ret.wMinute = (WORD)iN;
	ret.wSecond = (WORD)iS;
	ret.wMilliseconds = (WORD)iZ;

	return true;
}

bool IsLeapYear(WORD p_year)
{
	return ((p_year % 400 == 0) || (p_year % 4 == 0)) && (p_year % 100 != 0);
}

bool IsValidateDate(SYSTEMTIME &dt)
{
	if (dt.wMonth > 12 || dt.wMonth == 0
		|| dt.wDay > 31 || dt.wDay == 0
		|| (dt.wMonth == 2 && IsLeapYear(dt.wYear) && dt.wDay > 29)
		|| (dt.wMonth == 2 && (!IsLeapYear(dt.wYear)) && dt.wDay > 28)
		|| ((dt.wMonth == 4 || dt.wMonth == 6 || dt.wMonth == 9 || dt.wMonth == 11) && dt.wDay > 30)
		)
		return false;
	return true;
}

bool IsValidateDateTime(SYSTEMTIME &dt)
{
	if (!IsValidateDate(dt))
		return false;
	return (dt.wHour >= 0 && dt.wHour < 24 && dt.wMinute >= 0 && dt.wMinute < 60 && dt.wSecond >= 0 && dt.wSecond < 60 && dt.wMilliseconds >= 0 && dt.wMilliseconds < 1000);
}

int YearsBetween(SYSTEMTIME &p_now, SYSTEMTIME &p_then)
{
	return p_then.wYear - p_now.wYear;
}

int MonthsBetween(SYSTEMTIME &p_now, SYSTEMTIME &p_then)
{
	if (!IsValidateDate(p_now) || !IsValidateDate(p_then))
		return 0;
	return YearsBetween(p_now, p_then) * 12 + (p_then.wMonth - p_now.wMonth);
}

int DaysBetween(SYSTEMTIME &p_now, SYSTEMTIME &p_then)
{
	if (!IsValidateDate(p_now) || !IsValidateDate(p_then))
		return 0;
	SYSTEMTIME dtNow, dtThen;
	int x;
	if (p_now.wYear < p_then.wYear
		|| (p_now.wYear == p_then.wYear && p_now.wMonth < p_then.wMonth)
		|| (p_now.wYear == p_then.wYear && p_now.wMonth == p_then.wMonth && p_now.wDay < p_then.wDay)
		)
	{
		dtNow = p_now;
		dtThen = p_then;
		x = 1;
	}
	else
	{
		dtNow = p_then;
		dtThen = p_now;
		x = -1;
	}
	int ret = 0;
	while (true)
	{
		if (dtNow.wYear == dtThen.wYear && dtNow.wMonth == dtThen.wMonth && dtNow.wDay == dtThen.wDay)
			break;
		++ret;
		++(dtNow.wDay);

		if (dtNow.wMonth == 2)
		{
			if (dtNow.wDay > 28 && !IsLeapYear(dtNow.wYear))
			{
				dtNow.wMonth = 3;
				dtNow.wDay = 1;
			}
			else if (dtNow.wDay > 29)
			{
				dtNow.wMonth = 3;
				dtNow.wDay = 1;
			}
		}
		else if (dtNow.wDay > 30 && (dtNow.wMonth == 4 || dtNow.wMonth == 6 || dtNow.wMonth == 9 || dtNow.wMonth == 11))
		{
			++(dtNow.wMonth);
			dtNow.wDay = 1;
		}
		else if (dtNow.wDay > 31 && (dtNow.wMonth == 1 || dtNow.wMonth == 3 || dtNow.wMonth == 5 || dtNow.wMonth == 7 || dtNow.wMonth == 8 || dtNow.wMonth == 10 || dtNow.wMonth == 12))
		{
			++(dtNow.wMonth);
			dtNow.wDay = 1;
			if (dtNow.wMonth > 12)
			{
				++(dtNow.wYear);
				dtNow.wMonth = 1;
			}
		}
	}

	return ret*x;
}

int HoursBetween(SYSTEMTIME &p_now, SYSTEMTIME &p_then)
{
	if (!IsValidateDateTime(p_now) || !IsValidateDateTime(p_then))
		return 0;
	return DaysBetween(p_now, p_then) * 24 + (p_then.wHour - p_now.wHour);
}

int MinutesBetween(SYSTEMTIME &p_now, SYSTEMTIME &p_then)
{
	if (!IsValidateDateTime(p_now) || !IsValidateDateTime(p_then))
		return 0;
	return HoursBetween(p_now, p_then) * 60 + (p_then.wMinute - p_now.wMinute);
}

long long SecondsBetween(SYSTEMTIME &p_now, SYSTEMTIME &p_then)
{
	return MinutesBetween(p_now, p_then) * 60 + (p_then.wMinute - p_now.wMinute);
}

void IncDay_(SYSTEMTIME &dt, int days)
{
	int d = days;
	while (d > 0)
	{
		--d;
		++(dt.wDay);
		if (dt.wMonth == 2)
		{
			if (dt.wDay > 29 && IsLeapYear(dt.wYear))
			{
				dt.wDay = 1;
				dt.wMonth = 3;
			}
			if (dt.wDay > 28 && !IsLeapYear(dt.wYear))
			{
				dt.wDay = 1;
				dt.wMonth = 3;
			}
		}
		else if (dt.wMonth == 4 || dt.wMonth == 6 || dt.wMonth == 9 || dt.wMonth == 11)
		{
			if (dt.wDay > 30)
			{
				dt.wDay = 1;
				++(dt.wMonth);
			}
		}
		else
		{
			if (dt.wDay > 31)
			{
				dt.wDay = 1;
				++(dt.wMonth);
			}
		}
		if (dt.wMonth > 12)
		{
			dt.wMonth = 1;
			++(dt.wYear);
		}
	}
}

void IncYear(SYSTEMTIME &dt, int years)
{
	dt.wYear += years;
}

void IncMonth(SYSTEMTIME &dt, int months)
{
	int y = months / 12;
	int m = months % 12;
	IncYear(dt, y);
	while (m > 0)
	{
		--m;
		++(dt.wMonth);
		if (dt.wMonth > 12)
		{
			++(dt.wYear);
			dt.wMonth = 1;
		}
	}
	if (dt.wMonth == 2)
	{
		if (dt.wDay > 29 && IsLeapYear(dt.wYear))
			dt.wDay = 29;
		if (dt.wDay > 28 && !IsLeapYear(dt.wYear))
			dt.wDay = 28;
	}
	else if (dt.wMonth == 4 || dt.wMonth == 6 || dt.wMonth == 9 || dt.wMonth == 11)
	{
		if (dt.wDay > 30)
			dt.wDay = 30;
	}

}

void DecDay(SYSTEMTIME &dt, int days)
{
	int d = days;
	while (d < 0)
	{
		++d;
		--(dt.wDay);
		if (dt.wDay < 1)
		{
			--(dt.wMonth);
			if (dt.wMonth < 1)
			{
				dt.wMonth = 12;
				--(dt.wYear);
			}
			if (dt.wMonth == 2)
			{
				if (IsLeapYear(dt.wYear))
					dt.wDay = 29;
				else
					dt.wDay = 28;
			}
			else if (dt.wMonth == 4 || dt.wMonth == 6 || dt.wMonth == 9 || dt.wMonth == 11)
			{
				dt.wDay = 30;
			}
			else
			{
				dt.wDay = 31;
			}
		}
	}
}

void IncDay(SYSTEMTIME &dt, int days)
{
	if (!IsValidateDate(dt))
		return;
	if (days >= 0)
		IncDay_(dt, days);
	else
		DecDay(dt, days);
	dt.wDayOfWeek = DayOfWeek(dt);
}

void IncHour(SYSTEMTIME &dt, int hours)
{
	if (!IsValidateDateTime(dt))
		return;
	int d = hours / 24;
	int h = hours % 24;
	IncDay(dt, d);
	if (hours >= 0)
	{
		if (dt.wHour + h > 23)
		{
			IncDay(dt, 1);
			dt.wHour = (dt.wHour + h - 24);
		}
		else
			dt.wHour += h;
	}
	else
	{
		int i = dt.wHour;
		i += h;
		if (i < 0)
		{
			IncDay(dt, -1);
			dt.wHour = 24 + h;
		}
		else
			dt.wHour = i;
	}
}

void IncMinute(SYSTEMTIME &dt, int minutes)
{
	if (!IsValidateDateTime(dt))
		return;
	int h = minutes / 60;
	int m = minutes % 60;
	IncHour(dt, h);
	if (minutes >= 0)
	{
		if (dt.wMinute + m > 59)
		{
			IncHour(dt, 1);
			dt.wMinute = (dt.wMinute + m - 60);
		}
		else
			dt.wMinute = dt.wMinute + m;
	}
	else
	{
		int i = dt.wMinute;
		i += m;
		if (i < 0)
		{
			IncHour(dt, -1);
			dt.wMinute = 60 + m;
		}
		else
			dt.wMinute = i;
	}
}

void IncSecond(SYSTEMTIME &dt, int seconds)
{
	if (!IsValidateDateTime(dt))
		return;
	int m = seconds / 60;
	int s = seconds % 60;
	IncMinute(dt, m);
	if (seconds >= 0)
	{
		if (dt.wSecond + s > 59)
		{
			IncMinute(dt, 1);
			dt.wSecond = (dt.wSecond + s - 60);
		}
		else
			dt.wSecond = dt.wSecond + s;
	}
	else
	{
		int i = dt.wSecond;
		i += s;
		if (i < 0)
		{
			IncHour(dt, -1);
			dt.wSecond = 60 + s;
		}
		else
			dt.wSecond = i;
	}
}

int DayOfWeek(SYSTEMTIME &dt)
{
	if (!IsValidateDate(dt))
		return -1;
	SYSTEMTIME st;
	GetLocalTime(&st);
	int ispan = DaysBetween(st, dt);
	int ispan1 = ispan % 7;
	if (ispan > 0)
	{
		dt.wDayOfWeek = st.wDayOfWeek + ispan1;
		if (dt.wDayOfWeek > 6)
			dt.wDayOfWeek -= 7;
	}
	else
	{
		int wd = st.wDayOfWeek;
		wd += ispan1;
		if (wd < 0)
			dt.wDayOfWeek = 7 + wd;
		else
			dt.wDayOfWeek = wd;
	}
	return dt.wDayOfWeek;
}

void IncWeek(SYSTEMTIME &dt, int weeks)
{
	if (!IsValidateDate(dt))
		return;
	IncDay(dt, weeks * 7);
}

int WeeksBetween(SYSTEMTIME &p_now, SYSTEMTIME &p_then)
{
	return DaysBetween(p_now, p_then) / 7;
}

string FormatDatetime(const SYSTEMTIME &st)
{
	char cst[30] = { 0 };
	sprintf_s(cst, "%4d-%02d-%02d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return string(cst);
}

string FormatDate(const SYSTEMTIME &st)
{
	char cst[30] = { 0 };
	sprintf_s(cst, "%4d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
	return string(cst);
}

string FormatTime(const SYSTEMTIME &st)
{
	char cst[30] = { 0 };
	sprintf_s(cst, "%02d:%02d:%02d:%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return string(cst);
}

std::wstring FormatDatetimeW(const SYSTEMTIME &st)
{
	wchar_t cst[30] = { 0 };
	swprintf_s(cst, L"%4d-%02d-%02d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return wstring(cst);
}

std::wstring FormatDateW(const SYSTEMTIME &st)
{
	wchar_t cst[30] = { 0 };
	swprintf_s(cst, L"%4d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
	return wstring(cst);
}

std::wstring FormatTimeW(const SYSTEMTIME &st)
{
	wchar_t cst[30] = { 0 };
	swprintf_s(cst, L"%02d:%02d:%02d:%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return wstring(cst);
}

bool ParseDataTime(const wstring ws, SYSTEMTIME& st)
{
	wstring wsDate, wsTime;
	SplitStringW(ws, L" ", wsDate, wsTime);
	int begin = 0;
	int end = 0;
	std::vector<wstring> vecData;
	do
	{
		end = wsDate.find(L"-", begin);
		vecData.push_back(wsDate.substr(begin, end - begin));
		begin = end + 1;
	} while (end != -1);
	if (vecData.size() != 3)
		return false;
	int year = _wtoi(vecData[0].c_str());
	int month = _wtoi(vecData[1].c_str());
	int day = _wtoi(vecData[2].c_str());
	begin = 0; end = 0;
	std::vector<wstring> vecTime;
	do
	{
		end = wsTime.find(L":", begin);
		vecTime.push_back(wsTime.substr(begin, end - begin));
		begin = end + 1;
	} while (end != -1);
	if (vecTime.size() != 3)
		return false;
	int hour = _wtoi(vecTime[0].c_str());
	int minute = _wtoi(vecTime[1].c_str());
	int second = _wtoi(vecTime[2].c_str());
	st.wYear = year;
	st.wMonth = month;
	st.wDay = day;
	st.wHour = hour;
	st.wMinute = minute;
	st.wSecond = second;
	st.wMilliseconds = 0;
	return IsValidateDate(st) ? true : false;
}

void GetUTCTime(wstring& wsTime )
{
	SYSTEMTIME st;
	FILETIME ft;
	GetSystemTime ( &st );
	SystemTimeToFileTime( &st, &ft );
	ULARGE_INTEGER ui;
	ui.LowPart = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;
	INT64 Pt;
	Pt = (INT64)((ui.QuadPart - 116444736000000000) / 10000);
	wchar_t temp[64];
	_i64tow_s(Pt,temp,64,10);
	wsTime.assign(temp);
}

void t2tm( long long time, SYSTEMTIME& st )
{
	tm temptm; 
	localtime_s(&temptm, &time);
	st.wYear = 1900 + temptm.tm_year;
	st.wMonth = temptm.tm_mon+1;
	st.wDayOfWeek = temptm.tm_wday;
	st.wDay = temptm.tm_mday;
	st.wHour = temptm.tm_hour;
	st.wMinute = temptm.tm_min;
	st.wSecond = temptm.tm_sec;
	st.wMilliseconds = 0;
}

void tm2t(long long& time, const SYSTEMTIME& st)
{
	tm temptm = {st.wSecond,  st.wMinute, st.wHour, st.wDay, st.wMonth - 1,  st.wYear - 1900,  st.wDayOfWeek,  0, 0};
	time = mktime(&temptm);
}

bool ParseURL(const wchar_t *url,wstring &host,wstring &path,wstring &obj,INTERNET_PORT &port)
{
	wchar_t szHostName[2048] = L"";
	wchar_t szURLPath[2048] = L"";
	wchar_t szURLExtra[2048] = L"";
	URL_COMPONENTS urlComp;
	memset(&urlComp, 0, sizeof(urlComp));
	urlComp.dwStructSize = sizeof(urlComp);
	urlComp.lpszHostName = szHostName;
	urlComp.dwHostNameLength = 2048;
	urlComp.lpszUrlPath = szURLPath;
	urlComp.dwUrlPathLength = 2048;
	urlComp.lpszExtraInfo = szURLExtra;
	urlComp.dwExtraInfoLength = 2048;
	urlComp.dwSchemeLength = -1;

	if (::WinHttpCrackUrl(url, wcslen(url), 0, &urlComp))
	{
		host.assign(szHostName);
		path.assign(szURLPath);
		obj.assign(szURLExtra);
		port = urlComp.nPort;
		return true;
	}
	return false;
}


 int winhttp_get(const wchar_t *url, const wchar_t *headers_crlf, void **response, int &response_size, std::wstring *response_header, HTTPCALLCB cb ,void* customdata/*= NULL*/)
{
	wstring host, path, obj;
	WORD wdPort = 0;
	if (!ParseURL(url, host, path, obj, wdPort))
		return -1;
	HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
	BOOL bResults = FALSE;
	DWORD dwSize = 0, dwDownLoaded = 0;
	MemoryStream memResponse;
	void *buf = NULL;
	/*proxy*/
	WINHTTP_CURRENT_USER_IE_PROXY_CONFIG prxy;
	memset(&prxy, 0, sizeof(WINHTTP_CURRENT_USER_IE_PROXY_CONFIG));
	WinHttpGetIEProxyConfigForCurrentUser(&prxy);
	DWORD prxy_type = WINHTTP_ACCESS_TYPE_NO_PROXY;
	LPCTSTR prxy_name = WINHTTP_NO_PROXY_NAME;
	LPCTSTR prxy_bypass = WINHTTP_NO_PROXY_BYPASS;
	if (prxy.lpszProxy)
	{
		prxy_type = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
		prxy_name = prxy.lpszProxy;
		prxy_bypass = prxy.lpszProxyBypass;
	}
	hSession = WinHttpOpen(L"Mozilla/5.0 (compatible)",
		prxy_type,
		prxy_name,
		prxy_bypass,
		0);
	if (prxy.lpszAutoConfigUrl)
		GlobalFree(prxy.lpszAutoConfigUrl);
	if (prxy.lpszProxy)
		GlobalFree(prxy.lpszProxy);
	if (prxy.lpszProxyBypass)
		GlobalFree(prxy.lpszProxyBypass);

	if (hSession)
		hConnect = WinHttpConnect(hSession, host.c_str(), wdPort, 0);
	DWORD openflag = WINHTTP_FLAG_REFRESH;
	if (wdPort == 443)
		openflag |= WINHTTP_FLAG_SECURE;
	if (hConnect)
		hRequest = WinHttpOpenRequest(hConnect, L"GET", (path + obj).c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, openflag);
	if (hRequest && headers_crlf)
		bResults = WinHttpAddRequestHeaders(hRequest, headers_crlf, -1, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);
	if (hRequest)
		bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, NULL, 0, 0, NULL);
	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);
	if (bResults && response_header)
	{
		dwSize = 0;
		WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, NULL, &dwSize, WINHTTP_NO_HEADER_INDEX);
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			wchar_t *head = new wchar_t[dwSize / sizeof(wchar_t)];
			bResults = WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, head, &dwSize, WINHTTP_NO_HEADER_INDEX);
			if (bResults)
				response_header->assign(head);
			delete[] head;
		}
	}
	dwSize = 0;
	if (bResults)
	{
		do
		{
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
				break;
			if (dwSize == 0)
				break;
			buf = malloc(dwSize);
			if (NULL == buf)
				break;
			if (!WinHttpReadData(hRequest, buf, dwSize, &dwDownLoaded))
				break;
			memResponse.Write(buf, dwDownLoaded);
			if (cb)
			{
				cb(buf,dwDownLoaded,customdata);
			}
			SafeFree(buf);
		} while (dwSize > 0);
		SafeFree(buf);
		if (memResponse.GetSize() > 0)
		{
			*response = malloc(memResponse.GetSize());
			if (*response)
			{
				memResponse.Seek(MemoryStream::soBegin, 0);
				memResponse.Read(*response, memResponse.GetSize());
				response_size = memResponse.GetSize();
			}
		}
	}

	int reterr = GetLastError();
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	if (bResults)
		return 0;
	else
		return reterr;

}

int winhttp_post(const wchar_t *url, const wchar_t *headers_crlf, void **response, int &response_size, wstring *response_header, void *postdata, int postdata_size)
{
	wstring host, path, obj;
	WORD wdPort = 0;
	if (!ParseURL(url, host, path, obj, wdPort))
		return -1;
	HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
	BOOL bResults = FALSE;
	DWORD dwSize = 0, dwDownLoaded = 0, dwUpLoaded = 0, dwRemain = 0;
	MemoryStream memResponse;
	void *buf = NULL;
	void *buftmp = NULL;
	/*proxy*/
	WINHTTP_CURRENT_USER_IE_PROXY_CONFIG prxy;
	memset(&prxy, 0, sizeof(WINHTTP_CURRENT_USER_IE_PROXY_CONFIG));
	WinHttpGetIEProxyConfigForCurrentUser(&prxy);
	DWORD prxy_type = WINHTTP_ACCESS_TYPE_NO_PROXY;
	LPCTSTR prxy_name = WINHTTP_NO_PROXY_NAME;
	LPCTSTR prxy_bypass = WINHTTP_NO_PROXY_BYPASS;
	if (prxy.lpszProxy)
	{
		prxy_type = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
		prxy_name = prxy.lpszProxy;
		prxy_bypass = prxy.lpszProxyBypass;
	}
	hSession = WinHttpOpen(L"Mozilla/5.0 (compatible)",
		prxy_type,
		prxy_name,
		prxy_bypass,
		0);
	if (prxy.lpszAutoConfigUrl)
		GlobalFree(prxy.lpszAutoConfigUrl);
	if (prxy.lpszProxy)
		GlobalFree(prxy.lpszProxy);
	if (prxy.lpszProxyBypass)
		GlobalFree(prxy.lpszProxyBypass);

	if (hSession)
		hConnect = WinHttpConnect(hSession, host.c_str(), wdPort, 0);
	DWORD openflag = WINHTTP_FLAG_REFRESH;
	if (wdPort == 443)
		openflag |= WINHTTP_FLAG_SECURE;
	if (hConnect)
		hRequest = WinHttpOpenRequest(hConnect, L"POST", (path + obj).c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, openflag);
	if (hRequest && headers_crlf)
		bResults = WinHttpAddRequestHeaders(hRequest, headers_crlf, -1, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);
	if (hRequest)
		bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, NULL, 0, postdata_size, NULL);
	if (bResults && postdata && postdata_size > 0)
	{
		dwRemain = postdata_size;
		buftmp = postdata;
		do
		{
			bResults = WinHttpWriteData(hRequest, buftmp, dwRemain, &dwUpLoaded);
			if (bResults)
			{
				dwRemain -= dwUpLoaded;
				IncPtr(&buftmp, dwUpLoaded);
				dwUpLoaded = 0;
				if (0 == dwRemain)
					break;
			}
			else
			{
				DWORD i = GetLastError();
				break;
			}
		} while (1);
	}
	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);
	if (bResults && response_header)
	{
		dwSize = 0;
		WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, NULL, &dwSize, WINHTTP_NO_HEADER_INDEX);
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			wchar_t *head = new wchar_t[dwSize / sizeof(wchar_t)];
			bResults = WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, head, &dwSize, WINHTTP_NO_HEADER_INDEX);
			if (bResults)
				response_header->assign(head);
			delete[] head;
		}
	}
	dwSize = 0;
	if (bResults)
	{
		do
		{
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
				break;
			if (dwSize == 0)
				break;
			buf = malloc(dwSize);
			if (NULL == buf)
				break;
			if (!WinHttpReadData(hRequest, buf, dwSize, &dwDownLoaded))
				break;
			memResponse.Write(buf, dwDownLoaded);
			free(buf);
			buf = NULL;
		} while (dwSize > 0);
		if (buf)
			free(buf);
		if (memResponse.GetSize() > 0)
		{
			*response = malloc(memResponse.GetSize());
			if (*response)
			{
				memResponse.Seek(MemoryStream::soBegin, 0);
				memResponse.Read(*response, memResponse.GetSize());
				response_size = memResponse.GetSize();
			}
		}
	}

	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	if (bResults)
		return 0;
	else
		return GetLastError();
}

 int winhttp_ex(const wchar_t *url, void **response, int &response_size, const wchar_t *headers_crlf /*= NULL*/, const wchar_t* sMode /*= NULL*/, std::wstring* response_header /*= NULL*/, void *postdata /*= NULL*/, int postdata_size /*= NULL*/)
{
	wstring host, path, obj;
	WORD wdPort = 0;
	if (!ParseURL(url, host, path, obj, wdPort))
		return -1;
	HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
	BOOL bResults = FALSE;
	DWORD dwSize = 0, dwDownLoaded = 0, dwUpLoaded = 0, dwRemain = 0;
	MemoryStream memResponse;
	void *buf = NULL;
	void *buftmp = NULL;
	/*proxy*/
	WINHTTP_CURRENT_USER_IE_PROXY_CONFIG prxy;
	memset(&prxy, 0, sizeof(WINHTTP_CURRENT_USER_IE_PROXY_CONFIG));
	WinHttpGetIEProxyConfigForCurrentUser(&prxy);
	DWORD prxy_type = WINHTTP_ACCESS_TYPE_NO_PROXY;
	LPCTSTR prxy_name = WINHTTP_NO_PROXY_NAME;
	LPCTSTR prxy_bypass = WINHTTP_NO_PROXY_BYPASS;
	if (prxy.lpszProxy)
	{
		prxy_type = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
		prxy_name = prxy.lpszProxy;
		prxy_bypass = prxy.lpszProxyBypass;
	}
	hSession = WinHttpOpen(L"Mozilla/5.0 (compatible)",
		prxy_type,
		prxy_name,
		prxy_bypass,
		0);
	if (prxy.lpszAutoConfigUrl)
		GlobalFree(prxy.lpszAutoConfigUrl);
	if (prxy.lpszProxy)
		GlobalFree(prxy.lpszProxy);
	if (prxy.lpszProxyBypass)
		GlobalFree(prxy.lpszProxyBypass);

	if (hSession)
		hConnect = WinHttpConnect(hSession, host.c_str(), wdPort, 0);
	DWORD openflag = WINHTTP_FLAG_REFRESH;
	if (wdPort == 443)
		openflag |= WINHTTP_FLAG_SECURE;
	if (hConnect)
		if (sMode)
		{
			hRequest = WinHttpOpenRequest(hConnect,sMode, (path + obj).c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, openflag);
		}
		else
		{
			hRequest = WinHttpOpenRequest(hConnect, L"GET", (path + obj).c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, openflag);
		}
		
	if (hRequest && headers_crlf)
		bResults = WinHttpAddRequestHeaders(hRequest, headers_crlf, -1, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);
	if (hRequest)
		bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, NULL, 0, postdata_size, NULL);
	if (bResults && postdata && postdata_size > 0)
	{
		dwRemain = postdata_size;
		buftmp = postdata;
		do
		{
			bResults = WinHttpWriteData(hRequest, buftmp, dwRemain, &dwUpLoaded);
			if (bResults)
			{
				dwRemain -= dwUpLoaded;
				IncPtr(&buftmp, dwUpLoaded);
				dwUpLoaded = 0;
				if (0 == dwRemain)
					break;
			}
			else
			{
				DWORD i = GetLastError();
				break;
			}
		} while (1);
	}
	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);
	if (bResults && response_header)
	{
		dwSize = 0;
		WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, NULL, &dwSize, WINHTTP_NO_HEADER_INDEX);
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			wchar_t *head = new wchar_t[dwSize / sizeof(wchar_t)];
			bResults = WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, head, &dwSize, WINHTTP_NO_HEADER_INDEX);
			if (bResults)
				response_header->assign(head);
			delete[] head;
		}
	}
	dwSize = 0;
	if (bResults)
	{
		do
		{
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
				break;
			if (dwSize == 0)
				break;
			buf = malloc(dwSize);
			if (NULL == buf)
				break;
			if (!WinHttpReadData(hRequest, buf, dwSize, &dwDownLoaded))
				break;
			memResponse.Write(buf, dwDownLoaded);
			free(buf);
			buf = NULL;
		} while (dwSize > 0);
		if (buf)
			free(buf);
		if (memResponse.GetSize() > 0)
		{
			*response = malloc(memResponse.GetSize());
			if (*response)
			{
				memResponse.Seek(MemoryStream::soBegin, 0);
				memResponse.Read(*response, memResponse.GetSize());
				response_size = memResponse.GetSize();
			}
		}
	}

	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	if (bResults)
		return 0;
	else
		return GetLastError();
}

void FreeMemory(void* pFree)
{
	SafeFree(pFree);
}
typedef NTSTATUS (WINAPI* _NtQueryInformationProcess)
	(_In_ HANDLE ProcessHandle, _In_ PROCESSINFOCLASS ProcessInformationClass,
	_Out_ PVOID ProcessInformation, _In_ ULONG ProcessInformationLength, _Out_opt_ PULONG ReturnLength);
_NtQueryInformationProcess NtQueryInformationProcess_;

DWORD GetProcessPEBAddress(HANDLE hProc)
{
	PROCESS_BASIC_INFORMATION peb;
	DWORD tmp;
	NtQueryInformationProcess_ = (_NtQueryInformationProcess)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQueryInformationProcess");
	NtQueryInformationProcess_(hProc, ProcessBasicInformation, &peb, sizeof(PROCESS_BASIC_INFORMATION), &tmp);
	return (DWORD)peb.PebBaseAddress;
}

void OSVersion(DWORD& Major,DWORD& Minor,DWORD& Build)
{
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	DWORD pebAddress = GetProcessPEBAddress(handle);
	DWORD OSMajorVersionAddress = pebAddress + 0x0a4;
	DWORD OSMinorVersionAddress = pebAddress + 0x0a8;
	DWORD OSBuildNumberAddress = pebAddress + 0x0ac;
	ReadProcessMemory(handle, (void*)OSMajorVersionAddress, &Major, sizeof(Major), 0);
	ReadProcessMemory(handle, (void*)OSMinorVersionAddress, &Minor, sizeof(Minor), 0);
	ReadProcessMemory(handle, (void*)OSBuildNumberAddress, &Build, sizeof(Build), 0);
}
/*
std::string GetWinOSVersion()
{
	std::string strRet = "UnKown OS";
	do 
	{
		SYSTEM_INFO si;
		ZeroMemory(&si, sizeof(SYSTEM_INFO));
		GetSystemInfo(&si);
		OSVERSIONINFOEX osvi;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if(!GetVersionEx ((OSVERSIONINFO *) &osvi))
			break;

		switch(osvi.dwMajorVersion)
		{
		case 4:
			{
				if (osvi.dwMinorVersion == 0)
				{
					if (osvi.dwPlatformId==VER_PLATFORM_WIN32_NT)
						strRet = "Microsoft Windows NT 4.0";
					else
						strRet = "Microsoft Windows 95";
				}
				else if (osvi.dwMinorVersion == 10)
				{
					strRet = "Microsoft Windows 98";
				}
				else if (osvi.dwMinorVersion == 90)
				{
					strRet = "Microsoft Windows Me";
				}

			}
			break;
		case 5:{
					if (osvi.dwMinorVersion == 0)
					{
						strRet = "Microsoft Windows 2000";
					}
					else if (osvi.dwMinorVersion == 1)
					{
						strRet = "Microsoft Windows xp";
					}
					else if (osvi.dwMinorVersion == 2)
					{
						if(osvi.wProductType==VER_NT_WORKSTATION && si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
						{
							strRet = "Microsoft Windows XP Professional x64 Edition";
						}
						else if (GetSystemMetrics(SM_SERVERR2)==0)
						{
							strRet = "Microsoft Windows Server 2003";
						}
						else if (GetSystemMetrics(SM_SERVERR2)!=0)
						{
							strRet = "Microsoft Windows Server 2003 R2";
						}	
					}
			   }
			   break;
		case 6:{
					if (osvi.dwMinorVersion == 0)
					{
						if(osvi.wProductType==VER_NT_WORKSTATION)
							strRet = "Microsoft Windows Vista"; 
						else 
							strRet = "Microsoft Windows Server 2008";    
					}
					else if (osvi.dwMinorVersion == 1)
					{
						if(osvi.wProductType==VER_NT_WORKSTATION) 
							strRet = "Microsoft Windows 7"; 
						else 
							strRet = "Microsoft Windows Server 2008 R2"; 
						break;
					}
					else if (osvi.dwMinorVersion == 2)
					{
						DWORD Major = 0,Minor = 0,Build = 0;
						OSVersion(Major,Minor,Build);
						if (Major == 10)
						{
							strRet = "Microsoft Windows 10"; 
						}
						else if(Major == 10 && Minor == 3)
						{
							strRet = "Microsoft Windows 8.1"; 
						}
						else
						{
							strRet = "Microsoft Windows 8"; 
						}
					}
			   }
			   break;
		default:
			break;

		}
	} while (0);
	return strRet;
}
*/
bool LoadFileToBuffer(const wstring &file,/*outer*/void **buf,/*outer*/DWORD &bufsize)
{
	bool bSuccess = false;
	if (!FileExistsW(file))
		return false;
	HANDLE h = CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			DWORD iSize = GetFileSize(h, NULL);
			if (iSize == INVALID_FILE_SIZE)
				break;
			SetFilePointer(h, 0, NULL, FILE_BEGIN);
			*buf = malloc(iSize);
			if (NULL == *buf)
				break;
			DWORD iReaded = 0;
			if (ReadFile(h, *buf, iSize, &iReaded, NULL))
			{
				bufsize = iSize;
				bSuccess = true;
			}
		} while (0);
		CloseHandle(h);
	}
	return bSuccess;
}

bool SaveBufferToFile(const wstring &file, const void *buf, DWORD bufsize)
{
	if (NULL == buf || bufsize == 0)
		return false;
	bool bSuccess = false;
	HANDLE h = CreateFileW(file.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			SetFilePointer(h, 0, NULL, FILE_BEGIN);
			DWORD wted = 0;
			if (TRUE == WriteFile(h, buf, bufsize, &wted, NULL))
			{
				bSuccess = true;
			}
		} while (0);
		CloseHandle(h);
	}
	return bSuccess;
}

BOOL UrlEncode( const char* szSrc, char* pBuf, int cbBufLen, BOOL bUpperCase )
{
	if (szSrc == NULL || pBuf == NULL || cbBufLen <= 0)
		return FALSE;

	size_t len_ascii = strlen(szSrc);
	if (len_ascii == 0)
	{
		pBuf[0] = 0;
		return TRUE;
	}

	//先转换到UTF-8
	char baseChar = bUpperCase ? 'A' : 'a';
	int cchWideChar = MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, NULL, 0);
	LPWSTR pUnicode = (LPWSTR)malloc((cchWideChar + 1) * sizeof(WCHAR));
	if (pUnicode == NULL)
		return FALSE;
	MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, pUnicode, cchWideChar + 1);

	int cbUTF8 = WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, NULL, 0, NULL, NULL);
	LPSTR pUTF8 = (LPSTR)malloc((cbUTF8 + 1) * sizeof(CHAR));
	if (pUTF8 == NULL)
	{
		free(pUnicode);
		return FALSE;
	}
	WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, pUTF8, cbUTF8 + 1, NULL, NULL);
	pUTF8[cbUTF8] = '\0';

	unsigned char c;
	int cbDest = 0; //累加
	unsigned char *pSrc = (unsigned char*)pUTF8;
	unsigned char *pDest = (unsigned char*)pBuf;
	while (*pSrc && cbDest < cbBufLen - 1)
	{
		c = *pSrc;
		if (isalpha(c) || isdigit(c) || c == '-' || c == '.' || c == '~' || c == '_')
		{
			*pDest = c;
			++pDest;
			++cbDest;
		}
		else if (c == ' ')
		{
			*pDest = '+';
			++pDest;
			++cbDest;
		}
		else
		{
			//检查缓冲区大小是否够用？
			if (cbDest + 3 > cbBufLen - 1)
				break;
			pDest[0] = '%';
			pDest[1] = (c >= 0xA0) ? ((c >> 4) - 10 + baseChar) : ((c >> 4) + '0');
			pDest[2] = ((c & 0xF) >= 0xA) ? ((c & 0xF) - 10 + baseChar) : ((c & 0xF) + '0');
			pDest += 3;
			cbDest += 3;
		}
		++pSrc;
	}
	//null-terminator
	*pDest = '\0';
	free(pUnicode);
	free(pUTF8);
	return TRUE;
}

BOOL UrlDecode( const char* szSrc, char* pBuf, int cbBufLen )
{
	if (szSrc == NULL || pBuf == NULL || cbBufLen <= 0)
		return FALSE;

	size_t len_ascii = strlen(szSrc);
	if (len_ascii == 0)
	{
		pBuf[0] = 0;
		return TRUE;
	}

	char *pUTF8 = (char*)malloc(len_ascii + 1);
	if (pUTF8 == NULL)
		return FALSE;

	int cbDest = 0; //累加
	unsigned char *pSrc = (unsigned char*)szSrc;
	unsigned char *pDest = (unsigned char*)pUTF8;
	while (*pSrc)
	{
		if (*pSrc == '%')
		{
			*pDest = 0;
			//高位
			if (pSrc[1] >= 'A' && pSrc[1] <= 'F')
				*pDest += (pSrc[1] - 'A' + 10) * 0x10;
			else if (pSrc[1] >= 'a' && pSrc[1] <= 'f')
				*pDest += (pSrc[1] - 'a' + 10) * 0x10;
			else
				*pDest += (pSrc[1] - '0') * 0x10;

			//低位
			if (pSrc[2] >= 'A' && pSrc[2] <= 'F')
				*pDest += (pSrc[2] - 'A' + 10);
			else if (pSrc[2] >= 'a' && pSrc[2] <= 'f')
				*pDest += (pSrc[2] - 'a' + 10);
			else
				*pDest += (pSrc[2] - '0');

			pSrc += 3;
		}
		else if (*pSrc == '+')
		{
			*pDest = ' ';
			++pSrc;
		}
		else
		{
			*pDest = *pSrc;
			++pSrc;
		}
		++pDest;
		++cbDest;
	}
	//null-terminator
	*pDest = '\0';
	++cbDest;

	int cchWideChar = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pUTF8, cbDest, NULL, 0);
	LPWSTR pUnicode = (LPWSTR)malloc(cchWideChar * sizeof(WCHAR));
	if (pUnicode == NULL)
	{
		free(pUTF8);
		return FALSE;
	}
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pUTF8, cbDest, pUnicode, cchWideChar);
	WideCharToMultiByte(CP_ACP, 0, pUnicode, cchWideChar, pBuf, cbBufLen, NULL, NULL);
	free(pUTF8);
	free(pUnicode);
	return TRUE;
}


MemoryStream::MemoryStream()
	:m_buffer(NULL), m_size(0), m_pos(0), m_capacity(0), m_init_capacity(1024)
{

}

MemoryStream::~MemoryStream()
{
	Clear();
}

long MemoryStream::Read(MemoryStream &dest, long bytes)
{
	long new_bytes = min(m_size - m_pos, bytes);
	if (new_bytes <= 0)
		return 0;
	void *p = m_buffer;
	IncPtr(&p, m_pos);
	dest.Write(p, new_bytes);
	m_pos += new_bytes;
	return new_bytes;
}

long MemoryStream::Read(void *dest, long bytes)
{
	if (bytes <= 0)
		return 0;
	long ret = min(bytes, m_size - m_pos);
	if (ret <= 0)
		return 0;
	void *p = m_buffer;
	IncPtr(&p, m_pos);
	memcpy(dest, p, ret);
	m_pos += ret;
	return ret;
}

bool MemoryStream::Write(const MemoryStream &from, long bytes /*= -1*/)
{
	if (bytes < -1 || bytes == 0 || bytes > from.GetSize())
		return false;
	long lsize = 0;
	if (-1 == bytes)
		lsize = from.GetSize();
	else
		lsize = bytes;
	long add_size = lsize - (m_capacity - m_pos);
	while (add_size > 0)
	{
		if (!Expand())
			return false;
		add_size = lsize - (m_capacity - m_pos);
	}
	void *p = m_buffer;
	IncPtr(&p, m_pos);
	memcpy(p, from.GetBuffer(), lsize);
	m_pos += lsize;
	m_size = max(m_size, m_pos);
	return true;
}

bool MemoryStream::Write(const void *from, long bytes)
{
	if (bytes <= 0)
		return false;
	long add_size = bytes - (m_capacity - m_pos);
	while (add_size > 0)
	{
		if (!Expand())
			return false;
		add_size = bytes - (m_capacity - m_pos);
	}
	void *p = m_buffer;
	IncPtr(&p, m_pos);
	memcpy(p, from, bytes);
	m_pos += bytes;
	m_size = max(m_size, m_pos);
	return true;
}

void MemoryStream::Seek(SeekOrigin so, long offset)
{
	if (m_size <= 0)
		return;
	long os = 0;
	if (so == soBegin)
		os = offset;
	else if (so == soEnd)
		os = m_size - 1 + offset;
	else if (so == soCurrent)
		os = m_pos + offset;
	else
		return;
	if (os < 0)
		os = 0;
	else if (os >= m_size)
		os = m_size - 1;
	m_pos = os;
}

void MemoryStream::Clear()
{
	free(m_buffer);
	m_buffer = NULL;
	m_pos = 0;
	m_size = 0;
	m_capacity = 0;
}

bool MemoryStream::LoadFromFile(const wstring &file)
{
	void *buf = NULL;
	DWORD bufsize = 0;
	if (LoadFileToBuffer(file, &buf, bufsize))
	{
		Clear();
		Write(buf, bufsize);
		free(buf);
		return true;
	}
	return false;
}

bool MemoryStream::SaveToFile(const wstring &file)
{
	return SaveBufferToFile(file, m_buffer, m_size);
}

bool MemoryStream::Expand(long new_capacity /*= -1*/)
{
	long newp = m_capacity * 2;
	if (0 == newp)
		newp = m_init_capacity;
	if (new_capacity != -1)
		newp = new_capacity;
	if (newp <= m_capacity)
		return false;
	void *new_buf = NULL;
	if (m_buffer == NULL)
		new_buf = malloc(newp);
	else
		new_buf = realloc(m_buffer, newp);
	if (NULL == new_buf)
		return false;
	m_buffer = new_buf;
	m_capacity = newp;
	return true;
}


TThread::TThread() 
	: m_FreeOnTerminate(false)
	, m_Stopped(false)
	, m_ThreadHandle(0)
	, m_ThreadID(0)
{

}

void TThread::Start()
{
	m_ThreadHandle = (HANDLE)_beginthreadex(NULL, 0, Wrapper, (void *)this, 0, &m_ThreadID);
}

unsigned int __stdcall TThread::Wrapper(void *Param)
{
	TThread * thrd = (TThread*)(Param);
	thrd->Run();
	if (thrd->GetFreeOnTerminate())
		delete thrd;
	_endthreadex(NULL);
	return 0;
}


#define TIMETRAYICON 1990531
CTrayIconEx *CTrayIconEx::pCTrayIconEx = NULL;
void CALLBACK OnTrayTimer(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);

CTrayIconEx::CTrayIconEx(void)
{
	memset(&m_trayData, 0, sizeof(m_trayData));
	m_hIcon = NULL;
	m_bVisible = false;
	m_bTwinkling = false;
	pCTrayIconEx = this;
	m_pTrayMsglistLock = new RTL_CRITICAL_SECTION();
	InitializeCriticalSection(m_pTrayMsglistLock);
}

CTrayIconEx::~CTrayIconEx(void)
{
	vector<TrayMessageInfo *>::iterator iter = m_TrayMsglist.begin();
	for (; iter != m_TrayMsglist.end();){
		delete *iter;
		iter = m_TrayMsglist.erase(iter);
	}
	if (m_pTrayMsglistLock){
		delete m_pTrayMsglistLock;
	}
}

DWORD CTrayIconEx::GetTrayIconDataSize()
{
	HINSTANCE hinstDll;
	DWORD ret = sizeof(NOTIFYICONDATA);
	hinstDll = LoadLibraryA("Shell32.dll");

	if (hinstDll)
	{
		DLLGETVERSIONPROC pDllGetVersion;
		pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, "DllGetVersion");
		if (pDllGetVersion)
		{
			DLLVERSIONINFO dvi;
			HRESULT hr;
			ZeroMemory(&dvi, sizeof(dvi));
			dvi.cbSize = sizeof(dvi);
			hr = (*pDllGetVersion)(&dvi);
			if (SUCCEEDED(hr))
			{
				char ver[25] = { 0 };
				sprintf_s(ver, "%d.%d.%d", dvi.dwMajorVersion, dvi.dwMinorVersion, dvi.dwBuildNumber);
				string sver(ver);
				if (sver >= "6.0.6")
					ret = sizeof(NOTIFYICONDATA);
				else if (sver >= "6.0")
					ret = NOTIFYICONDATA_V3_SIZE;
				else if (sver >= "5.0")
					ret = NOTIFYICONDATA_V2_SIZE;
			}
		}
		FreeLibrary(hinstDll);
	}
	return ret;
}

bool CTrayIconEx::CreateTrayIcon(HWND _RecvHwnd, UINT _IconIDResource, LPCTSTR _ToolTipText /*= NULL*/, UINT _Message /*= NULL/*(UIEVENT_TRAYICON)*/)
{
	m_trayData.cbSize = GetTrayIconDataSize();
	m_trayData.hWnd = _RecvHwnd;
	m_trayData.uID = _IconIDResource;
	m_trayData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_trayData.uCallbackMessage = _Message;
	m_hIcon = m_trayData.hIcon = (HICON)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(_IconIDResource), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	if (wcslen(_ToolTipText) < 128)
	{
		wcsncpy_s(m_trayData.szTip,128,_ToolTipText,_TRUNCATE);
	}
	::SetTimer(_RecvHwnd, TIMETRAYICON, 500, (TIMERPROC)&OnTrayTimer);
	return Shell_NotifyIcon(NIM_ADD, &m_trayData) == TRUE;
}

bool CTrayIconEx::DeleteTrayIcon()
{
	if (m_trayData.hWnd)
		::KillTimer(m_trayData.hWnd, TIMETRAYICON);
	return Shell_NotifyIcon(NIM_DELETE, &m_trayData) == TRUE;
}

bool CTrayIconEx::SetTooltipText(LPCTSTR _ToolTipText)
{
	if (_ToolTipText)
		wcscpy_s(m_trayData.szTip, _ToolTipText);
	m_trayData.uFlags |= NIF_TIP;
	return Shell_NotifyIcon(NIM_MODIFY, &m_trayData) == TRUE;
}

LPCTSTR CTrayIconEx::GetTooltipText() const
{
	return m_trayData.szTip;
}

bool CTrayIconEx::SetIcon(HICON _Hicon)
{
	if (_Hicon != NULL)
	{
		m_hIcon = _Hicon;
	}
	m_trayData.hIcon = _Hicon;
	m_trayData.uFlags |= NIF_ICON;
	return Shell_NotifyIcon(NIM_MODIFY, &m_trayData) == TRUE;
}

bool CTrayIconEx::SetIcon(LPCTSTR _IconFile)
{
	HICON hIcon = LoadIcon(GetModuleHandle(NULL), _IconFile);
	return SetIcon(hIcon);
}

bool CTrayIconEx::SetIcon(UINT _IDResource)
{
	HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(_IDResource));
	return SetIcon(hIcon);
}

HICON CTrayIconEx::GetIcon() const
{
	return m_hIcon;
}

bool CTrayIconEx::IsVisible()
{
	return m_bVisible;
}

void CTrayIconEx::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;
}

bool CTrayIconEx::IsTwinkling()
{
	return m_bTwinkling;
}

void CTrayIconEx::SetTwinkling(bool bTwinkling)
{
	m_bTwinkling = bTwinkling;
}

void CTrayIconEx::TrayMessage(const wstring &msg, wstring title, TrayMessageType mt, DWORD timeout)
{
	if (msg.length() > 254)
		return;
	if (title.length() > 60)
		return;
	EnterCriticalSection(m_pTrayMsglistLock);
	TrayMessageInfo *info = new TrayMessageInfo();
	info->msg = msg;
	info->msg_type = mt;
	info->timeout = timeout;
	info->title = title;
	m_TrayMsglist.push_back(info);
	LeaveCriticalSection(m_pTrayMsglistLock);
}

void CTrayIconEx::OnTimerTray()
{
	NOTIFYICONDATA* TrayIcon = GetTrayIcon();
	if (TrayIcon == NULL)
		return;
	if (!Shell_NotifyIcon(NIM_MODIFY, GetTrayIcon()))
		Shell_NotifyIcon(NIM_ADD, GetTrayIcon());
	TrayIcon->uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	//闪烁时不显示气泡
	if (IsTwinkling())
	{
		SetVisible(!(IsVisible()));
		SetIcon(IsVisible() ? GetIcon() : NULL);
	}
	else
	{
		if (m_TrayMsglist.size() > 0)
		{
			EnterCriticalSection(m_pTrayMsglistLock);
			TrayIcon->uFlags = (TrayIcon->uFlags | NIF_INFO);
			memset(TrayIcon->szInfo, 0, sizeof(TrayIcon->szInfo));
			memcpy(TrayIcon->szInfo, m_TrayMsglist[0]->msg.c_str(), m_TrayMsglist[0]->msg.length() * 2);
			TrayIcon->uTimeout = m_TrayMsglist[0]->timeout;
			memset(TrayIcon->szInfoTitle, 0, sizeof(TrayIcon->szInfoTitle));
			memcpy(TrayIcon->szInfoTitle, m_TrayMsglist[0]->title.c_str(), m_TrayMsglist[0]->title.length() * 2);
			switch (m_TrayMsglist[0]->msg_type)
			{
			case mtError:
				TrayIcon->dwInfoFlags = NIIF_ERROR;
				break;
			case mtInfo:
				TrayIcon->dwInfoFlags = NIIF_INFO;
			case mtWarning:
				TrayIcon->dwInfoFlags = NIIF_WARNING;
			}
			delete *m_TrayMsglist.begin();
			m_TrayMsglist.erase(m_TrayMsglist.begin());
			LeaveCriticalSection(m_pTrayMsglistLock);
		}
	}
}

void CALLBACK OnTrayTimer(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	if (idEvent == TIMETRAYICON){
		if (NULL != CTrayIconEx::pCTrayIconEx){
			CTrayIconEx::pCTrayIconEx->OnTimerTray();
		}
	}
}

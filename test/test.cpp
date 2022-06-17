#include <iostream>  
#include <string>
#include <tchar.h>
#include <Wincrypt.h>
using namespace std;  
#pragma comment(lib, "ws2_32.lib") 
// 计算文件的MD5字符串，支持大文件(large file)
string ComputeFileMD5(const TCHAR* szFilename)
{
	HCRYPTPROV hCryptProv = 0;
	HCRYPTHASH hHash = 0;
	HANDLE hMapFile = 0;

	BYTE bHash[0x7f];
	DWORD dwHashLen= 16;
	BYTE* pBuf;
	string digest;
	DWORD dwFileSize = 0, dwHighFileSize = 0;
 
	HANDLE hFile = CreateFile(szFilename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if( hFile == INVALID_HANDLE_VALUE )
	{
		return digest;
	}
 
	dwFileSize = GetFileSize(hFile, &dwHighFileSize);
 
	hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
 
	if (hMapFile == NULL || hMapFile == INVALID_HANDLE_VALUE)
	{
		goto release;
	}
 
	if(CryptAcquireContext(&hCryptProv,NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET))
	{
		if(CryptCreateHash(hCryptProv,CALG_MD5,0, 0, &hHash))
		{
			UINT64 filesize = (UINT64)dwHighFileSize << 32 | dwFileSize;
			for(UINT64 i = 0; i <= filesize; i+=1048576)// 1024*1024
			{
				size_t bufsize = size_t(min(filesize-i, 1048576));
				pBuf = (BYTE*) MapViewOfFile(hMapFile, FILE_MAP_READ, (DWORD)(i >> 32), (DWORD)i, bufsize);
 
				if (pBuf == NULL)
				{
					goto release;
				}
 
				if(!CryptHashData(hHash, pBuf, bufsize, 0))
				{
					goto release;
				}
				UnmapViewOfFile(pBuf);
			}
			if(CryptGetHashParam(hHash, HP_HASHVAL, bHash, &dwHashLen, 0))
			{
				digest.resize(32);
				BYTE value;
				for (int i = 0; i<16; i++)
				{
					value = bHash[i] >> 4;
					digest[i*2] = value < 10 ? char(value + '0') : char(value - 10 + 'a');
					value = bHash[i] & 0x0F;
					digest[i*2+1] = value < 10 ? char(value + '0') : char(value - 10 + 'a');
				}
			}
		}
	}
 
release:
	CryptDestroyHash(hHash);
	CryptReleaseContext(hCryptProv, 0);
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	CloseHandle(hFile);
	return digest;
}


int main(int argc, char *argv[])  
{
    string str = "There was a grocery shop in a town. Plenty of mice lived in that grocery shop. Food was in plenty for them. They ate everything and spoiled all the bags. They also wasted the bread, biscuits and fruits of the shop. The grocer got really worried. So, he thought \"I should buy a cat and let it stay at the grocery. Only then I can save my things.\" He bought a nice, big fat cat and let him stay there. The cat had a nice time hunting the mice and killing them. The mice could not move freely now. They were afraid that anytime the cat would eat them up. The mice wanted to do something. They held a meeting and all of them tweeted \"We must get rid of the cat. Can someone give a suggestion\"?  All the mice sat and brooded. A smart looking mouse stood up and said, \"The cat moves softly. That is the problem. If we can tie a bell around her neck, then things will be fine. We can know the movements of the cat\". \"Yes, that is answer,\" stated all the mice. An old mouse slowly stood up and asked, \"Who would tie the bell?\" After some moments there was no one there to answer this question."; 
    cout << "md5 of this string: " << ComputeFileMD5("test.cpp")<<endl;  
    return 0;  
}

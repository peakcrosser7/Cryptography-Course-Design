/* 彩虹表 */
#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace std;

const int ChainLen = 100001;
struct RainbowChain {
	char head[9], tail[9];
};

RainbowChain g_rainbow[10000];

bool unitSHA1(const char* str, int length, unsigned sha1[5]);
unsigned SHA1_ROTL(unsigned a, unsigned b);
unsigned SHA1_F(unsigned B, unsigned C, unsigned D, unsigned t);
void R(const unsigned sha1[5], char str[8], int no);
bool findChainPwd(const char* pass, const unsigned* sha1, int m, char* res);
bool findSha1Pwd(const unsigned* sha1, int m, char* res);
bool travelChain(const unsigned* sha1, int m, char* res);

inline bool equalHash(const unsigned* a, const unsigned* b) {
	return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] 
		&& a[3] == b[3] && a[4] == b[4];
}


int main() {
	int m;
	scanf("%d", &m);
	for (int i = 0; i < m; ++i) {
		scanf("%s %s", g_rainbow[i].head, g_rainbow[i].tail);
	}
	unsigned sha1[5];
	char res[9];
	for (int i = 0; i < 5; ++i) {
		scanf("%8x", &sha1[i]);
	}
	if (findSha1Pwd(sha1, m, res)) {
		printf("%s", res);
	}
	else puts("None");
	return 0;
}

bool travelChain(const unsigned* sha1, int m, char* res) {
	for (int i = 0; i < m; ++i) {
		char tmpP[9];
		unsigned tmpH[5];
		strcpy(tmpP, g_rainbow[i].head);
		int j = 1;
		while (strcmp(tmpP, g_rainbow[i].tail)) {
			unitSHA1(tmpP, 8, tmpH);
			if (equalHash(tmpH, sha1)) {
				strcpy(res, tmpP);
				return true;
			}
			R(tmpH, tmpP, j);
			j = j < 100 ? j + 1 : 1;
		}
	}
	return false;
}

bool findChainPwd(const char* pass, const unsigned* sha1, int m, char* res) {
	char tmpP[9];
	for (int k = 0; k < m; ++k) {
		if (strcmp(pass, g_rainbow[k].tail) == 0) {
			strcpy(tmpP, g_rainbow[k].head);
			unsigned tmpH[5];
			for (int i = 1, no = 1; no < ChainLen; ++no) {
				unitSHA1(tmpP, 8, tmpH);
				if (equalHash(sha1, tmpH)) {
					strcpy(res, tmpP);
					return true;
				}
				R(tmpH, tmpP, i);
				i = i < 100 ? i + 1 : 1;
			}
			return false;
		}
	}
	return false;
}

bool findSha1Pwd(const unsigned* sha1, int m, char* res) {
	char tmpP[9]{ 0 };
	unsigned tmpH[5];
	for (int i = 0; i < 100; ++i) {
		R(sha1, tmpP, i + 1);
		if (i + 1 == 100 && findChainPwd(tmpP, sha1, m, res)) {
			return true;
		}
		for (int j = 1; j < 100000; ++j) {
			unitSHA1(tmpP, 8, tmpH);
			R(tmpH, tmpP, (i + j) % 100 + 1);
			if ((i + j) % 100 + 1 == 100 && findChainPwd(tmpP, sha1, m, res)) {
				return true;
			}
		}
	}
	return false;
}

bool unitSHA1(const char* str, int len, unsigned sha1[5]) {
	//计算字符串SHA-1
	//参数说明：
	//str         字符串指针
	//length      字符串长度
	//sha1        用于保存SHA-1的字符串指针
	//返回值是否成功

	unsigned char* pp, * ppend;
	unsigned int l, i, K[80], W[80], TEMP, A, B, C, D, E,
		H0, H1, H2, H3, H4;
	H0 = 0x67452301, H1 = 0xEFCDAB89, H2 = 0x98BADCFE,
		H3 = 0x10325476, H4 = 0xC3D2E1F0;

	for (i = 0; i < 20; K[i++] = 0x5A827999);
	for (i = 20; i < 40; K[i++] = 0x6ED9EBA1);
	for (i = 40; i < 60; K[i++] = 0x8F1BBCDC);
	for (i = 60; i < 80; K[i++] = 0xCA62C1D6);

	l = len + ((len % 64 > 56) ?
		(128 - len % 64) : (64 - len % 64));
	if (!(pp = (unsigned char*)malloc((unsigned int)l))) return false;
	for (i = 0; i < len; pp[i + 3 - 2 * (i % 4)] = str[i], i++);
	for (pp[i + 3 - 2 * (i % 4)] = 128, i++; i < l; i++) {
		pp[i + 3 - 2 * (i % 4)] = 0;
	}
	*((unsigned int*)(pp + l - 4)) = len << 3;
	*((unsigned int*)(pp + l - 8)) = len >> 29;
	for (ppend = pp + l; pp < ppend; pp += 64) {
		for (i = 0; i < 16; W[i] = ((unsigned int*)pp)[i], i++);
		for (i = 16; i < 80; i++) {
			W[i] = SHA1_ROTL((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]), 1);
		}
		A = H0, B = H1, C = H2, D = H3, E = H4;
		for (i = 0; i < 80; i++) {
			TEMP = SHA1_ROTL(A, 5) + SHA1_F(B, C, D, i) + E + W[i] + K[i];
			E = D, D = C, C = SHA1_ROTL(B, 30), B = A, A = TEMP;
		}
		H0 += A, H1 += B, H2 += C, H3 += D, H4 += E;
	}
	free(pp - l);
	sha1[0] = H0, sha1[1] = H1, sha1[2] = H2, sha1[3] = H3, sha1[4] = H4;
	return true;
}

inline unsigned SHA1_ROTL(unsigned a, unsigned b) {
	return ((a >> (32 - b)) & (0x7fffffff >> (31 - b))) | (a << b);
}

inline unsigned SHA1_F(unsigned B, unsigned C, unsigned D, unsigned t) {
	return ((t < 40) ? ((t < 20) ? ((B & C) | ((~B) & D)) : (B ^ C ^ D)) :
		((t < 60) ? ((B & C) | (B & D) | (C & D)) : (B ^ C ^ D)));
}

void R(const unsigned sha1[5], char str[8], int no) {
	unsigned n = (sha1[0] + sha1[1] * no) % 2176782336;
	str[0] = 'a', str[1] = '0', str[2] = '0', str[3] = '0';
	str[4] = '0', str[5] = '0', str[6] = '0', str[7] = '0';
	int i = 2;
	while (n) {
		unsigned tmp = n % 36;
		if (tmp < 10) str[i++] = tmp + '0';
		else str[i++] = tmp - 10 + 'a';
		n /= 36;
	}
}

/**/
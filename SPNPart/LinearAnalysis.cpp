/* 线性分析 */
#include <cstdio>
#include <cmath>
#include <algorithm>

using namespace std;

struct PCPair {		//明密文对
	unsigned short x, y;
};
struct KeyInfo {	//子秘钥信息
	unsigned short L2, L4;
	int maxCount;
};

const int Nr = 4;
const int l = 4, m = 4;		//m个l比特s盒
const unsigned short SubstiBox[16] = {
	0xe,0x4,0xd,0x1,
	0x2,0xf,0xb,0x8,
	0x3,0xa,0x6,0xc,
	0x5,0x9,0x0,0x7
};
const unsigned short InverSubstiBox[16] = {
	0xe,0x3,0x4,0x8,
	0x1,0xc,0xa,0xf,
	0x7,0xd,0x9,0x6,
	0xb,0x2,0x0,0x5
};
const int T = 8000;		//明密文对数
PCPair g_pc[T];

unsigned linearAnalysis(const PCPair* pc, int T, const unsigned short* inverSBox);
unsigned short SPN(unsigned short x, const unsigned short* k,
	const unsigned short* sBox);
unsigned short substitute(unsigned short u, const unsigned short* sBox);
unsigned short permutate(unsigned short v);
void arrangeEncKey(unsigned key, unsigned short* k);
bool maxkeyCmp (const KeyInfo& a, const KeyInfo& b);

inline void read(unsigned short& res) {
	char ch = getchar();
	res = 0;
	while ((ch < '0' || ch>'9') && (ch < 'a' || ch>'f'))
		ch = getchar();
	char num;
	while (ch >= '0' && ch <= '9' || ch >= 'a' && ch <= 'f') {
		num = (ch >= '0' && ch <= '9') ? ch - '0' : ch - 'a' + 10;
		res = (res << 4) + num, ch = getchar();
	}
}


int main() {
	int n;
	scanf("%d", &n);
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < T; ++j) {
			read(g_pc[j].x), read(g_pc[j].y);
		}
		printf("%08x\n", linearAnalysis(g_pc, T, InverSubstiBox));
	}
	return 0;
}

//线性分析
unsigned linearAnalysis(const PCPair* pc, int T,
		const unsigned short* inverSBox) {
	const unsigned short len = l * m;
	unsigned short v[5], u[5], z24, z13;
	//第一条线性逼近:24盒
	int Count24[16][16]{ 0 };
	for (int i = 0; i < T; ++i) {
		for (unsigned short L2 = 0; L2 < 16; ++L2)
			for (unsigned short L4 = 0; L4 < 16; ++L4) {
				v[2] = L2 ^ ((pc[i].y >> 2 * l) & 0xf);
				v[4] = L4 ^ (pc[i].y & 0xf);
				u[2] = inverSBox[v[2]];
				u[4] = inverSBox[v[4]];
				z24 = ((pc[i].x >> (len - 5)) & 0x1)
					^ ((pc[i].x >> (len - 7)) & 0x1)
					^ ((pc[i].x >> (len - 8)) & 0x1)
					^ ((u[2] >> 2) & 0x1) ^ (u[2] & 0x1)
					^ ((u[4] >> 2) & 0x1) ^ (u[4] & 0x1);
				if (z24 == 0) Count24[L2][L4]++;
			}
	}

	//记录所有L2L4的信息并按maxCount降序排序
	KeyInfo maxkey24[256];
	int no = 0;
	for (unsigned short L2 = 0; L2 < 16; ++L2)
		for (unsigned short L4 = 0; L4 < 16; ++L4) {
			maxkey24[no].maxCount = abs(Count24[L2][L4] - T / 2);
			maxkey24[no].L2 = L2, maxkey24[no].L4 = L4;
			no++;
		}
	sort(maxkey24, maxkey24 + 256, maxkeyCmp);

	//由maxCount最大开始尝试
	for (int to = 0; to<256; ++to) {
		//第二条线性逼近:123盒
		int Count13[16][16]{ 0 };
		unsigned short  maxkey2 = maxkey24[to].L2, maxkey4 = maxkey24[to].L4;
		for (int i = 0; i < T; ++i) {
			v[2] = maxkey2 ^ ((pc[i].y >> 2 * l) & 0xf);
			u[2] = inverSBox[v[2]];
			for (unsigned short L1 = 0; L1 < 16; ++L1)
				for (unsigned short L3 = 0; L3 < 16; ++L3) {
					v[1] = L1 ^ ((pc[i].y >> 3 * l) & 0xf);
					v[3] = L3 ^ ((pc[i].y >> l) & 0xf);
					u[1] = inverSBox[v[1]];
					u[3] = inverSBox[v[3]];	

					z13 = ((pc[i].x >> (len - 5)) & 0x1)
						^ ((pc[i].x >> (len - 6)) & 0x1)
						^ ((pc[i].x >> (len - 7)) & 0x1)
						^ ((pc[i].x >> (len - 8)) & 0x1)
						^ ((u[1] >> 2) & 0x1) ^ (u[1] & 0x1)
						^ ((u[2] >> 2) & 0x1) ^ (u[2] & 0x1)
						^ ((u[3] >> 2) & 0x1) ^ (u[3] & 0x1);

					if (z13 == 0) ++Count13[L1][L3];
				}
		}
		unsigned short maxkey1, maxkey3;
		int max13 = -1;
		for (unsigned short L1 = 0; L1 < 16; ++L1)
			for (unsigned short L3 = 0; L3 < 16; ++L3) {
				Count13[L1][L3] = abs(Count13[L1][L3] - T / 2);
				if (Count13[L1][L3] > max13) {
					max13 = Count13[L1][L3];
					maxkey1 = L1, maxkey3 = L3;
				}
			}

		unsigned short k16b = (maxkey1 << 12) | (maxkey2 << 8) | (maxkey3 << 4) | maxkey4;	//最后一轮子秘钥
		unsigned key;
		unsigned short k[Nr + 1];
		//暴力枚举前16b
		for (unsigned i = 0; i <= 0xffff; ++i) {
			key = (i << 16) | k16b;
			arrangeEncKey(key, k);
			int z;
			for (z = 0; z < 3; ++z) {
				if (SPN(pc[z].x, k, SubstiBox) != pc[z].y)
					break;
			}
			if (z == 3) return key;	
		}
	}
	return 0;
}

inline void arrangeEncKey(unsigned key, unsigned short* k) {
	k[0] = (key >> 16) & 0xffff, k[1] = (key >> 12) & 0xffff,
		k[2] = (key >> 8) & 0xffff, k[3] = (key >> 4) & 0xffff,
		k[4] = key & 0xffff;
}

unsigned short SPN(unsigned short x, const unsigned short* k,
	const unsigned short* sBox) {
	unsigned short w = x, u, v;
	for (int i = 0; i < Nr - 1; ++i) {
		u = k[i] ^ w;
		v = substitute(u, sBox);
		w = permutate(v);
	}
	u = k[Nr - 1] ^ w;
	v = substitute(u, sBox);
	return k[Nr] ^ v;
}

inline unsigned short substitute(unsigned short u, const unsigned short* sBox) {
	unsigned short res = sBox[(u & 0xf)] | sBox[(u >> 4) & 0xf] << 4
		| sBox[(u >> 8) & 0xf] << 8 | sBox[(u >> 12) & 0xf] << 12;
	return res;
}

inline unsigned short permutate(unsigned short v) {
	unsigned short res = ((v >> 15) & 1) << 15 | ((v >> 11) & 1) << 14
		| ((v >> 7) & 1) << 13 | ((v >> 3) & 1) << 12 | ((v >> 14) & 1) << 11
		| ((v >> 10) & 1) << 10 | ((v >> 6) & 1) << 9 | ((v >> 2) & 1) << 8
		| ((v >> 13) & 1) << 7 | ((v >> 9) & 1) << 6 | ((v >> 5) & 1) << 5
		| ((v >> 1) & 1) << 4 | ((v >> 12) & 1) << 3 | ((v >> 8) & 1) << 2
		| ((v >> 4) & 1) << 1 | (v & 1);
	return res;
}

bool maxkeyCmp (const KeyInfo& a, const KeyInfo& b)
{
	return a.maxCount > b.maxCount;
}


/**/
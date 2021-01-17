/* 差分分析 */
#include <cstdio>
#include <cstdlib>
#include <ctime>

using namespace std;

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
unsigned short g_cipherTxt[0x10000][5];		//密文

unsigned differentialAnalysis(const unsigned short* inverSBox);
void arrangeEncKey(unsigned key, unsigned short* k);
unsigned short SPN(unsigned short x, const unsigned short* k,
	const unsigned short* sBox);
unsigned short substitute(unsigned short u, const unsigned short* sBox);
unsigned short permutate(unsigned short v);


int main() {
	int n;
	scanf("%d", &n);
	getchar();
	char tmp;
	//srand((unsigned)time(nullptr));

	for (int i = 0; i < n; ++i) {
		//unsigned key = ((rand() % 0x10000) << 16) | (rand() % 0x10000);
		//printf("RightKey=%08x\n", key);
		//unsigned short k[Nr + 1];
		//keyEncArrange(key, k);
		for (int j = 0; j <= 0xffff; ++j) {		//按照每个16进制字符(4b)存储密文到二维数组
			g_cipherTxt[j][1] = ((tmp = getchar()) <= '9') ? (tmp - '0') : (tmp - 'a' + 10);
			g_cipherTxt[j][2] = ((tmp = getchar()) <= '9') ? (tmp - '0') : (tmp - 'a' + 10);
			g_cipherTxt[j][3] = ((tmp = getchar()) <= '9') ? (tmp - '0') : (tmp - 'a' + 10);
			g_cipherTxt[j][4] = ((tmp = getchar()) <= '9') ? (tmp - '0') : (tmp - 'a' + 10);
			getchar();
			//auto tmp = SPN(unsigned short(j), k, substiBox);
			//CipherTxt[j][1] = (tmp >> 12) & 0xf, CipherTxt[j][2] = (tmp >> 8) & 0xf,
			//	CipherTxt[j][3] = (tmp >> 4) & 0xf, CipherTxt[j][4] = tmp & 0xf;
		}
		unsigned res = differentialAnalysis(InverSubstiBox);
		printf("%08x\n", res);
		//if (res != key) printf("ERROR!!!!!!!!!!!!!!!!!!!!\n\n");
	}
}

//差分分析
unsigned differentialAnalysis(const unsigned short* inverSBox) {
	unsigned short v[5], u[5], v_[5], u_[5], u__[5];
	//第一条差分链:24盒
	int Count24[16][16]{ 0 };
	for (int i = 0; i < 1500; ++i) {
		unsigned short star = i ^ 0x0b00;
		//unsigned short y16b = CipherTxt[i], y_16b = CipherTxt[i ^ 0x0b00];
		if (g_cipherTxt[i][1] == g_cipherTxt[star][1]
			&& g_cipherTxt[i][3] == g_cipherTxt[star][3]) {
			for (unsigned short L2 = 0; L2 < 16; ++L2)
				for (unsigned short L4 = 0; L4 < 16; ++L4) {
					v[2] = L2 ^ g_cipherTxt[i][2];
					v[4] = L4 ^ g_cipherTxt[i][4];
					u[2] = inverSBox[v[2]];
					u[4] = inverSBox[v[4]];
					v_[2] = L2 ^ g_cipherTxt[star][2];
					v_[4] = L4 ^ g_cipherTxt[star][4];
					u_[2] = inverSBox[v_[2]];
					u_[4] = inverSBox[v_[4]];
					u__[2] = u[2] ^ u_[2];
					u__[4] = u[4] ^ u_[4];
					if (u__[2] == 0x6 && u__[4] == 0x6) ++Count24[L2][L4];
				}
		}
	}
	int max24 = -1;
	unsigned short maxkey2, maxkey4;
	for (unsigned short L2 = 0; L2 < 16; ++L2)
		for (unsigned short L4 = 0; L4 < 16; ++L4) {
			if (Count24[L2][L4] > max24) {
				max24 = Count24[L2][L4];
				maxkey2 = L2, maxkey4 = L4;
			}
		}

	//第二条差分链:134盒
	int Count13[16][16]{ 0 };
	for (int i = 0; i < 11500; ++i) {
		unsigned short star = i ^ 0x00a0;
		if (g_cipherTxt[i][2] == g_cipherTxt[star][2]) {
			v[4] = maxkey4 ^ g_cipherTxt[i][4];
			u[4] = inverSBox[v[4]];
			v_[4] = maxkey4 ^ g_cipherTxt[star][4];
			u_[4] = inverSBox[v_[4]];
			u__[4] = u[4] ^ u_[4];
			if (u__[4] == 0x5) {
				for (unsigned short L1 = 0; L1 < 16; ++L1) {
					for (unsigned short L3 = 0; L3 < 16; ++L3) {
						v[1] = L1 ^ g_cipherTxt[i][1];
						v[3] = L3 ^ g_cipherTxt[i][3];
						u[1] = inverSBox[v[1]];
						u[3] = inverSBox[v[3]];
						v_[1] = L1 ^ g_cipherTxt[star][1];
						v_[3] = L3 ^ g_cipherTxt[star][3];
						u_[1] = inverSBox[v_[1]];
						u_[3] = inverSBox[v_[3]];
						u__[1] = u[1] ^ u_[1];
						u__[3] = u[3] ^ u_[3];
						if (u__[1] == 0x5 && u__[3] == 0x5) ++Count13[L1][L3];
					}
				}
			}
		}
	}
	int max13 = -1;
	unsigned short maxkey1, maxkey3;
	for (unsigned short L1 = 0; L1 < 16; ++L1)
		for (unsigned short L3 = 0; L3 < 16; ++L3) {
			if (Count13[L1][L3] > max13) {
				max13 = Count13[L1][L3];
				maxkey1 = L1, maxkey3 = L3;
			}
		}
	unsigned short k16b = (maxkey1 << 12) | (maxkey2 << 8) | (maxkey3 << 4) | maxkey4;	//最后一轮子秘钥
	unsigned key;
	unsigned short k[5];
	//暴力枚举前16b
	for (unsigned i = 0; i <= 0xffff; ++i) {
		key = (i << 16) | (maxkey1 << 12) | (maxkey2 << 8) | (maxkey3 << 4) | maxkey4;
		arrangeEncKey(key, k);
		int z;
		for (z = 0; z < 20; ++z) {
			unsigned short c16b = (g_cipherTxt[z][1] << 12) | (g_cipherTxt[z][2] << 8) |
				(g_cipherTxt[z][3] << 4) | g_cipherTxt[z][4];
			if (SPN(z, k, SubstiBox) != c16b) {
				break;
			}
		}
		if (z == 20) return key;
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
	for (int i = 0; i < 3; ++i) {
		u = k[i] ^ w;
		v = substitute(u, sBox);
		w = permutate(v);
	}
	u = k[3] ^ w;
	v = substitute(u, sBox);
	return k[4] ^ v;
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

/**/
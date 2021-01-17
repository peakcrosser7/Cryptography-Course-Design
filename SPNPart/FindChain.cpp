/* 寻找差分链/线性逼近 */
#include <vector>
#include <algorithm>

using namespace std;

struct PCPair {
	unsigned short x, y;
};
struct SpnPair {	//SPN对
	unsigned short uv;	//S盒的输入u或输出v
	float chainVal;		//链值: 偏差或扩散率
	bool operator==(const SpnPair& b) const {
		if(uv==b.uv&&chainVal==b.chainVal) return true;
		return false;
	}
};
struct MapEle {		//隐射表元素
	unsigned short opt,		//对应输出的4进制
		val;	//对应的链值
};

const int l = 4, m = 4;
const int T = 8000;		//明密文对数
const unsigned short InverSubstiBox[16] = {
	0xe,0x3,0x4,0x8,
	0x1,0xc,0xa,0xf,
	0x7,0xd,0x9,0x6,
	0xb,0x2,0x0,0x5
};

const vector<vector<MapEle>> LinearMap{		//线性分析表
	{{0,16}}, {{7,14}},{{0xe,2}},{{9,2}},{{5,4},{0xd,4}},
	{{6,12},{0xa,4}},{{3,12},{0xb,12}}, {{5,4},{0xc,12}},
	{{0xf,12}},{{8,4},{0xd,12}},{{1,12},{4,4}},
	{{1,12},{3,4},{4,12},{6,12}},{{2,12},{0xb,12}},
	{{5,12},{8,4}},{{5,4},{0xc,4}},{{2,4},{0xa,12}}
};

const vector<vector<MapEle>> DiffMap{	//差分分析表
	{{0,16}}, {{0xa,4}, {0xc,4}}, {{5,6}}, {{9,4}, {0xf,4}},
	{{6,6}, {0xb,4}}, {{1,4}, {0xa,4}},	{{3,4}, {5,4}},
	{{0xf,4}}, {{0xb,4}, {0xd,4}}, {{7,4}},	{{8,6}, {0xe,4}},
	{{2,8}}, {{0xd,6}}, {{1,4}, {7,4}}, {{8,6}, {3,4}}, {{4,6}, {9,4}}
};

PCPair g_pc[T];

vector<SpnPair> findChain(unsigned short xBitPos,
 const bool* sBoxSel, const vector<vector<MapEle>>& chainMap);
void findRound(SpnPair up, int Nr,
	const vector<vector<MapEle>>& chainMap, vector<SpnPair>& res);
void findV(SpnPair up, SpnPair vp, int no,
	const vector<vector<MapEle>>& chainMap, vector<SpnPair>& vpArr);
float getChainVal(unsigned short val);

unsigned short permutate(unsigned short v);
unsigned short linearAnalysis(unsigned short xBitPos, unsigned short uRes,
	const PCPair*pc, int T, const unsigned short* inverSBox);
void makeZ(unsigned short xBitPos, unsigned short uRes);

inline bool cmpSpnPairByUv(const SpnPair& a, const SpnPair& b) {
	//if (a.chainVal != b.chainVal)
	//	return a.chainVal > b.chainVal;
	return a.uv < b.uv;
}

//inline bool cmpSpnPairByVal(const SpnPair& a, const SpnPair& b) {
//	//if (a.chainVal != b.chainVal)
//	//	return a.chainVal > b.chainVal;
//	return a.chainVal > b.chainVal;
//}

int main() {
	//int n;
	//unsigned short maxkey;
	//scanf("%d", &n);
	//for (int j = 0; j < T; ++j) {
	//	scanf("%hx %hx", &pc[j].x, &pc[j].y);
	//}
	////寻找满足给定maxkey的线性逼近(输出uRes只覆盖1、3s盒
	//bool sBoxSel[] = { 1,1,1,1 };
	//for (int no = 1; no <= 4; ++no) {
	//	for (unsigned short i = 0; i <= 0xf; ++i) {
	//		unsigned short xBitPos = i << (4 - no) * 4;
	//		auto upRes = findChain(xBitPos, sBoxSel, linearMap);
	//		for (int j = 0; j < upRes.size(); ++j) {
	//			maxkey = linearAnalysis(xBitPos, upRes[j].uv, pc, T, inverSubstiBox);
	//			if (maxkey == 0x30f0) {
	//				printf("xBitPos=%04hx, uRes=%04hx\n", xBitPos, upRes[j].uv);
	//				makeZ(xBitPos, upRes[j].uv);
	//			}
	//		}
	//	}
	//}

	////已知输入xBitPos寻找线性逼近/差分链输出uRes
	//bool sBox[] = { 0,1,0,1 };
	//auto vRes = findChain(0x0b00, sBox, linearMap);
	//if (vRes.size()) {
	//	for (int i = 0; i < vRes.size(); ++i)
	//		printf("%04hx\n", vRes[i].uv);
	//}

	////验证线性逼近
	//bool sBoxSel[] = { 1,1,1,0 };
	//unsigned short xBitPos = 0x0f00;
	//auto upRes = findChain(xBitPos, sBoxSel, linearMap);
	//for (int i = 0; i < upRes.size(); ++i) {
	//	maxkey = linearAnalysis(xBitPos, upRes[i].uv, pc, T, inverSubstiBox);
	//	printf("maxkey=%04hx, uRes=%04hx\n", maxkey, upRes[i].uv);
	//	if (maxkey == 0x30d0)
	//		printf("maxkey=%04hx, uRes=%04hx\n", maxkey, upRes[i].uv);
	//}

	//寻找给定覆盖盒的差分链
	bool sBoxSel[] = { 1,0,1,0 };
	//bool sBoxSel[] = { 0,1,0,1 };
	//vector<SpnPair> upRes;
	for (int no = 1; no <= 2; ++no) {
		for (unsigned short i = 1; i <= 0xff; ++i) {
			unsigned short xBitPos = i << (2 - no) * 8;
			printf("%x\n", xBitPos);
			auto upRes = findChain(xBitPos, sBoxSel, DiffMap);
			//upRes.insert(upRes.end(), tmp.begin(),tmp.end());
			for (int j = 0; j < upRes.size(); ++j) {
				printf("xBitPos=%04hx, uRes=%04hx, val=%f\n",
					xBitPos, upRes[j].uv, upRes[j].chainVal);
			}
		}
	}

	return 0;
}

//寻找链(线性逼近/差分链)
vector<SpnPair> findChain(unsigned short xBitPos, const bool *sBoxSel, const vector<vector<MapEle>>& chainMap) {
	SpnPair up = { xBitPos,1 };
	vector<SpnPair> res;
	findRound(up, 1, chainMap, res);
	//for (int i = 0; i < vRes.size(); ++i) printf("%04hx\n", vRes[i]);
	//去除限定S盒输出位不符合的链
	for (auto it = res.begin(); it != res.end();) {
		int no;
		for (no = 0; no < 4; ++no) {
			//if (sBoxSel[no] == false && ((it->uv >> (3 - no) * 4) & 0xf )!= 0) {
			if (sBoxSel[no] == true && ((it->uv >> (3 - no) * 4) & 0xf) == 0
				|| it->chainVal < 0.005) {
				it = res.erase(it);
				break;
			}
		}
		if (no == 4) ++it;
	}
	//排序去重
	sort(res.begin(), res.end(), cmpSpnPairByUv);
	res.erase(unique(res.begin(), res.end()), res.end());
	return res;
}

//寻找SPN经1轮P盒置换后所有可能的u值存入res
void findRound(SpnPair up, int Nr, 
		const vector<vector<MapEle>>& chainMap, vector<SpnPair>& res) {
	if (Nr == 4) {		//第四轮时记录u值退出
		//printf("vRes= %04hx\n", u);
		//getchar();
		res.push_back(up);
		return;
	}
	vector<SpnPair> vpArr;
	findV(up, { 0,up.chainVal }, 1, chainMap, vpArr);
	for (int i = 0; i < vpArr.size(); ++i) {
		up.uv = permutate(vpArr[i].uv);		//代换
		up.chainVal = vpArr[i].chainVal;	//记录链值
		findRound(up, Nr + 1, chainMap, res);
	}
}

//寻找一轮中经1个S盒代换后所有可能的v值
void findV(SpnPair up, SpnPair vp, int no, 
	const vector<vector<MapEle>>& chainMap,  vector<SpnPair>& vpArr) {
	if (no == 5) {	//尝试完4个S盒后记录v值退出
		vpArr.push_back(vp);
		//printf("v= %04hx\n", v);
		//getchar();
		return;
	}
	unsigned short u4b = (up.uv >> (4 - no) * 4) & 0xf;
	if (u4b == 0) {		//若当前S盒无输入直接进行下一个S盒
		findV(up, vp, no + 1, chainMap, vpArr);
	}
	else {		//当前S盒有输入
		auto tmpV = vp;
		auto vList = chainMap.at(u4b);
		//遍历该S盒输入对应的所有结果
		for (int vIdx = 0; vIdx < vList.size();++vIdx) {	
			auto v4b = vList.at(vIdx).opt;
			tmpV.uv |= v4b << (4 - no) * 4;
			tmpV.chainVal *= getChainVal(vList.at(vIdx).val);	//计算链值
			findV(up, tmpV, no + 1, chainMap, vpArr);	//进行下一个S盒
			tmpV = vp;
		}
	}
}

//计算链值(偏差/扩散率)
float getChainVal(unsigned short val) {
	return 1.f * val / 16;
}

unsigned short linearAnalysis(unsigned short xBitPos, unsigned short uRes, 
		const PCPair* pc, int T, const unsigned short* inverSBox) {
	const unsigned short k2Get = 0xa, k4Get = 0x1;
	const unsigned short len = l * m;
	unsigned short v[5], u[5], z13;
	//unsigned short z24;
	int Count24[16][16]{ 0 }, Count13[16][16]{ 0 };
	for (int i = 0; i < T; ++i) {
		if (((pc[i].y >> 2 * l) & 0xf) != 0xa)
			continue;
		for (unsigned short L1 = 0; L1 < 16; ++L1)
			for (unsigned short L2 = 0; L2 < 16; ++L2) {
				z13 = 0;
				v[2] = k2Get ^ ((pc[i].y >> (2 * l)) & 0xf);
				v[4] = k4Get ^ (pc[i].y & 0xf);
				v[1] = L1 ^ ((pc[i].y >> (3 * l)) & 0xf);
				v[3] = L2 ^ ((pc[i].y >> l) & 0xf);
				for (int j = 1; j <= 4; j++) u[j] = inverSBox[v[j]];

				unsigned short u16b = (u[1] << 12) | (u[2] << 8) | (u[3] << 4) | u[4];
				//z24 = ((x >> (len - 5)) & 0x1)
				//	^ ((x >> (len - 7)) & 0x1)
				//	^ ((x >> (len - 8)) & 0x1)
				//	^ ((u[2] >> 2) & 0x1) ^ (u[2] & 0x1)
				//	^ ((u[4] >> 2) & 0x1) ^ (u[4] & 0x1);
				//if (z24 == 0) Count24[L1][L2]++;
				for (int c = 1; c <= 16; ++c) {
					if ((xBitPos >> (16 - c)) & 0x1) 
						z13 ^= (pc[i].x >> (16 - c)) & 0x1;
				}
				for (int c = 1; c <= 16; ++c) {
					if ((uRes >> (16 - c)) & 0x1)
						z13 ^= (u16b >> (16 - c)) & 0x1;
				}
				if (z13 == 0) ++Count13[L1][L2];
			}
	}

	unsigned short maxkey1, maxkey3, maxkey2 = 0, maxkey4 = 0;
	int max24 = -1, max13 = -1;
	for (unsigned short L1 = 0; L1 < 16; ++L1)
		for (unsigned short L2 = 0; L2 < 16; ++L2) {
			//Count24[L1][L2] = abs(Count24[L1][L2] - T / 2);
			Count13[L1][L2] = abs(Count13[L1][L2] - T / 2);
			//if (Count24[L1][L2] > max24) {
			//	max24 = Count24[L1][L2];
			//	maxkey2 = L1, maxkey4 = L2;
			//}
			if (Count13[L1][L2] > max13) {
				max13 = Count13[L1][L2];
				maxkey1 = L1, maxkey3 = L2;
			}
		}
	return (maxkey1 << 12) | (maxkey2 << 8) | (maxkey3 << 4) | maxkey4;
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

void makeZ(unsigned short xBitPos, unsigned short uRes) {
	printf("z13 =");

	//z13 = ((pc[i].x >> (len - 5)) & 0x1)
	//	^ ((pc[i].x >> (len - 6)) & 0x1)
	//	^ ((pc[i].x >> (len - 7)) & 0x1)
	//	^ ((pc[i].x >> (len - 8)) & 0x1)
	//	^ ((u[1] >> 2) & 0x1) ^ (u[1] & 0x1)
	//	^ ((u[2] >> 2) & 0x1) ^ (u[2] & 0x1)
	//	^ ((u[3] >> 2) & 0x1) ^ (u[3] & 0x1);
	for (int c = 1; c <= 16; ++c) {
		if ((xBitPos >> (16 - c)) & 0x1)
			printf(" ((pc[i].x >> (len - %d)) & 0x1) ^\n", c);
	}
	bool flag = true;
	for (int c = 1; c <= 16; ++c) {
		if ((uRes >> (16 - c)) & 0x1) {
			if (flag) {
				printf(" ((u[%d] >> %d) & 0x1)^", (c - 1) / 4 + 1, c % 4 ? 4 - c % 4 : 0);
				flag = false;
			}
			else printf(" \n((u[%d] >> %d) & 0x1)^", (c - 1) / 4 + 1, c % 4 ? 4 - c % 4 : 0);
		}
	}
	printf("\b;\n\n");

}

/**/
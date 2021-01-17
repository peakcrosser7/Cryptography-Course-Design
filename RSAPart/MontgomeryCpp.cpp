/* 蒙哥马利算法(C++完成) */
/* 由于使用类相关语法,最后一个测试用例超时, 仅供参考*/
#include <cstdio>
#include <gmpxx.h>
#include <cstring>
#include <cmath>

using namespace std;

const unsigned LimbBytes = sizeof(mp_limb_t);
const unsigned HalfBits = GMP_LIMB_BITS / 2;

class Montgomery {
private:
	mpz_class a_, e_, N_;
	mp_limb_t mask_ = 1, IN_;
	unsigned n_;
	//蒙哥马利变换
	mpz_class Mont(const mpz_class& a) const;
	//蒙哥马利乘
	void MontMult(mpz_class& A, const mpz_class& B);
	//蒙哥马利逆变换
	void MontInv(mpz_class& a) { MontMult(a, 1); }

public:
	Montgomery(const mpz_class& a, const mpz_class& e,
		const mpz_class& N);
	mpz_class operator() ();
};

mpz_class modInv(mpz_class a, mpz_class N);
mpz_class CRT(const mpz_class& c1, const mpz_class& c2,
	const mpz_class& qqInv, const mpz_class& ppInv, const mpz_class& N);


int main() {
	int n;
	scanf("%d", &n);
	mpz_class p, q, e, d, c, c1, c2, res,
		ppInv, qqInv, N, dmodp_1, dmodq_1;
	gmp_scanf("%Zd %Zd %Zd", p.get_mpz_t(), q.get_mpz_t(), e.get_mpz_t());
	d = modInv(e, (p - 1) * (q - 1));
	//gmp_printf("d=%Zd\n", d.get_mpz_t());
	dmodp_1 = d % (p - 1), dmodq_1 = d % (q - 1);
	ppInv = modInv(p, q), qqInv = modInv(q, p);
	ppInv *= p, qqInv *= q, N = p * q;
	for (int i = 0; i < n; ++i) {
		gmp_scanf("%Zd", c.get_mpz_t());
		c1 = Montgomery(c % p, dmodp_1, p)();
		//gmp_printf("c1=%Zd\n", c1.get_mpz_t());
		c2 = Montgomery(c % q, dmodq_1, q)();
		//gmp_printf("c2=%Zd\n", c2.get_mpz_t());
		res = CRT(c1, c2, qqInv, ppInv, N);
		gmp_printf("%Zd\n", res.get_mpz_t());
	}
	return 0;
}

Montgomery::Montgomery(const mpz_class& a, const mpz_class& e, 
	const mpz_class& N) : a_(a), e_(e), N_(N) {
	mpz_class d;
	/// d <<= LIMBBIT;
	auto dP = d.get_mpz_t();
	mpz_realloc(dP, 2);
	dP->_mp_size = 2;
	dP->_mp_d[1] = 1, dP->_mp_d[0] = 0;
	/// mask_ = d - 1;
	mpn_sub_n(&mask_, dP->_mp_d, &mask_, 2);
	
	//gmp_printf("d=%Zx, mask=%Mx\n", d.get_mpz_t(), mask_);
	n_ = N.get_mpz_t()->_mp_size;
	//if (tmp < 0) IN_ = (d + tmp) & mask_;
	//else IN_ = tmp & mask_;
	//mpn_and_n(&IN_,tmp.get_mpz_t()->_mp_d,mask_)
	mpz_class tmp = d - modInv(N_, d);
	mpn_and_n(&IN_, tmp.get_mpz_t()->_mp_d, &mask_, 1);
	//gmp_printf("n=%u, IN=%Mx\n", n_, IN_);
}

mpz_class Montgomery::operator() () {
	mpz_class X = Mont(1), A = Mont(a_);
	//gmp_printf("X=%Zx, A=%Zx\n", X.get_mpz_t(), A.get_mpz_t());
	char* str = new char[10000];
	mpz_get_str(str, 2, e_.get_mpz_t());

	for (int i = 0; str[i] != '\0'; ++i) {
		MontMult(X, X);
		if (str[i] == '1') {
			MontMult(X, A);
		}
		//gmp_printf("X=%Zx\n", X.get_mpz_t());
	}
	delete[] str;
	MontInv(X);
	return X;
}

mpz_class Montgomery::Mont(const mpz_class& a) const {
	int aSize = a.get_mpz_t()->_mp_size;
	mpz_class res;
	auto resP = res.get_mpz_t();
	unsigned newSize = aSize + n_;
	mpz_realloc(resP, newSize);
	resP->_mp_size = newSize;
	memcpy(resP->_mp_d + n_, a.get_mpz_t()->_mp_d, aSize * LimbBytes);
	memset(resP->_mp_d, 0, n_ * LimbBytes);
	return  res % N_;
}

void Montgomery::MontMult(mpz_class& A, const mpz_class& B) {
	mp_limb_t Tq[64], t0IN[4];
	mp_limb_t* pLow = Tq;
	A *= B;
	mpz_ptr T = A.get_mpz_t(), Np = N_.get_mpz_t();
	int TSize = T->_mp_size;
	memcpy(Tq, T->_mp_d, TSize * LimbBytes);
	memset(Tq + TSize, 0, ((n_ << 2) - TSize) * LimbBytes);
	for (unsigned i = 0, highSize = n_ + 1; i < n_; ++i) {
		mpn_mul_1(t0IN, pLow, 1, IN_);
		mp_limb_t carry = mpn_addmul_1(pLow, Np->_mp_d, n_,
			t0IN[0]);
		mpn_add_1(pLow + n_, pLow + n_, highSize, carry);
		++pLow, --highSize;
	}
	if (mpn_cmp(pLow, Np->_mp_d, n_) >= 0) {
		mpn_sub(pLow, pLow, n_ + 1, Np->_mp_d, n_);
	}
	memcpy(T->_mp_d, pLow, n_ * LimbBytes);
	T->_mp_size = n_;
}

inline mpz_class CRT(const mpz_class& c1, const mpz_class& c2,
	const mpz_class& qqInv, const mpz_class& ppInv, const mpz_class& N) {
	mpz_class res = (c1 * qqInv + c2 * ppInv) % N;
	return res;
}

mpz_class modInv(mpz_class a, mpz_class N) {
	mpz_class x1(1), x2(0), x3(N), y1(0), y2(1), y3(a), q, t1, t2, t3;
	if (a > N) a %= N;
	while (y3 != 0) {
		q = x3 / y3;
		t1 = x1 - q * y1, t2 = x2 - q * y2, t3 = x3 - q * y3;
		x1 = y1, x2 = y2, x3 = y3;
		y1 = t1, y2 = t2, y3 = t3;
	}
	if (x3 == 1) return x2 > 0 ? x2 : x2 + N;
	return 0;
}

/**/
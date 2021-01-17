/* 蒙哥马利算法 */
#include <cstdio>
#include <cstring>
#include <gmp.h>

using namespace std;

const unsigned LimbBytes = sizeof(mp_limb_t);

mpz_t N_;
mpz_t tmp_;
mp_limb_t IN_;
unsigned n_;
//蒙哥马利变换
void Mont(mpz_t res, const mpz_t a);
//蒙哥马利乘
void MontMult(mpz_t res, const mpz_t A, const mpz_t B);
//蒙哥马利逆变换
void MontInv(const mpz_t a, mpz_t res);
void Montgomery (mpz_t res, const mpz_t a,
		const mpz_t e, const mpz_t N);
void modInv(mpz_t res, const mpz_t a, const mpz_t N);


int main() {
	int n;
	scanf("%d", &n);
	mpz_t p, q, e, d, c, c1, c2, res, ppInv, qqInv,
		N, dmodp_1, dmodq_1, tmp, p_1, q_1;
	mpz_init(p), mpz_init(q), mpz_init(e), mpz_init(d);
	mpz_init(c), mpz_init(c1), mpz_init(c2), mpz_init(res);
	mpz_init(ppInv), mpz_init(qqInv), mpz_init(N);
	mpz_init(dmodp_1), mpz_init(dmodq_1);
	mpz_init(p_1), mpz_init(q_1), mpz_init(tmp);
	mpz_init(N_),mpz_init(tmp_);

	gmp_scanf("%Zd %Zd %Zd", p, q, e);
	mpz_sub_ui(p_1, p, 1);
	mpz_sub_ui(q_1, q, 1);
	mpz_mul(tmp, p_1, q_1);
	modInv(d, e, tmp);
	mpz_mod(dmodp_1, d, p_1), mpz_mod(dmodq_1, d, q_1);
	modInv(ppInv, p, q), modInv(qqInv, q, p);
	mpz_mul(ppInv, ppInv, p), mpz_mul(qqInv, qqInv, q);
	mpz_mul(N, p, q);
	for (int i = 0; i < n; ++i) {
		gmp_scanf("%Zd", c);
		mpz_mod(tmp, c, p); 
		Montgomery(c1, tmp, dmodp_1, p);
		mpz_mod(tmp, c, q);
		Montgomery(c2, tmp, dmodq_1, q);

		//CRT
		mpz_mul(q_1, c1, qqInv), mpz_mul(p_1, c2, ppInv);
		mpz_add(res, q_1, p_1);
		mpz_mod(res, res, N);
		gmp_printf("%Zd\n", res);
	}

	mpz_clear(p), mpz_clear(q), mpz_clear(e), mpz_clear(d);
	mpz_clear(c), mpz_clear(c1), mpz_clear(c2), mpz_clear(res);
	mpz_clear(ppInv), mpz_clear(qqInv), mpz_clear(N);
	mpz_clear(dmodp_1), mpz_clear(dmodq_1);
	mpz_clear(p_1), mpz_clear(q_1), mpz_clear(tmp);
	mpz_clear(N_), mpz_clear(tmp_);
	return 0;
}

void modInv(mpz_t res, const mpz_t a, const mpz_t N) {
	mpz_t x1, x2, x3, y1, y2, y3, q, t1, t2, t3;
	mpz_init_set_ui(x1, 1), mpz_init_set_ui(x2, 0), mpz_init_set(x3, N);
	mpz_init_set_ui(y1, 0), mpz_init_set_ui(y2, 1), mpz_init_set(y3, a);
	mpz_init(q), mpz_init(t1), mpz_init(t2), mpz_init(t3);

	while (mpz_cmp_ui(y3, 0) != 0) {
		mpz_div(q, x3, y3);
		mpz_mul(t1, q, y1), mpz_sub(t1, x1, t1);
		mpz_mul(t2, q, y2), mpz_sub(t2, x2, t2);
		mpz_mul(t3, q, y3), mpz_sub(t3, x3, t3);
		mpz_set(x1, y1), mpz_set(x2, y2), mpz_set(x3, y3);
		mpz_set(y1, t1), mpz_set(y2, t2), mpz_set(y3, t3);
	}
	if (mpz_cmp_ui(x3, 1) == 0) {
		if (mpz_cmp_ui(x2, 0) > 0) {
			mpz_set(res, x2);
		}
		else {
			mpz_add(res, x2, N);
		}
	}
	else mpz_set_ui(res, 0);

	mpz_clear(x1), mpz_clear(x2), mpz_clear(x3);
	mpz_clear(y1), mpz_clear(y2), mpz_clear(y3);
	mpz_clear(t1), mpz_clear(t2), mpz_clear(t3);
	mpz_clear(q);
}

void Montgomery(mpz_t res, const mpz_t a, const mpz_t e,
	const mpz_t N) {
	///Constructor
	mp_limb_t mask_ = 1;
	mpz_set(N_, N);
	mpz_init(tmp_);
	mpz_t d;
	mpz_init_set_ui(d, 1);
	mpz_mul_2exp(d, d, GMP_LIMB_BITS);
	mpn_sub_n(&mask_, d->_mp_d, &mask_, 1);

	n_ = N_->_mp_size;
	modInv(tmp_, N_, d);
	mpz_sub(tmp_, d, tmp_);
	mpn_and_n(&IN_, tmp_->_mp_d, &mask_, 1);
	mpz_clear(d);

	///Run
	mpz_t X, A;
	mpz_init(X), mpz_init(A);
	mpz_set_ui(tmp_, 1);
	Mont(X, tmp_), Mont(A, a);
	char* str = new char[100000];
	mpz_get_str(str, 2, e);

	for (int i = 0; str[i] != '\0'; ++i) {
		MontMult(X, X, X);
		if (str[i] == '1') {
			MontMult(X, X, A);
		}
	}
	delete[] str;
	MontInv(X, res);
	mpz_clear(X), mpz_clear(A);
}

inline void Mont(mpz_t res, const mpz_t a) {
	mpz_mul_2exp(tmp_, a, GMP_LIMB_BITS * n_);
	mpz_mod(res, tmp_, N_);
}

void MontMult(mpz_t T, const mpz_t A, const mpz_t B) {
	mp_limb_t Tq[64], t0IN[4];
	mp_limb_t* pLow = Tq;
	mpz_mul(T, A, B);
	int TSize = T->_mp_size;
	memcpy(Tq, T->_mp_d, TSize * LimbBytes);
	memset(Tq + TSize, 0, ((n_ << 2) - TSize) * LimbBytes);
	for (unsigned i = 0, highSize = n_ + 1; i < n_; ++i) {
		mpn_mul_1(t0IN, pLow, 1, IN_);
		mp_limb_t carry = mpn_addmul_1(pLow, N_->_mp_d, n_,
			t0IN[0]);
		mpn_add_1(pLow + n_, pLow + n_, highSize, carry);
		++pLow, --highSize;
	}
	if (mpn_cmp(pLow, N_->_mp_d, n_) >= 0 ) {
		mpn_sub(pLow, pLow, n_ + 1, N_->_mp_d, n_);
	}
	memcpy(T->_mp_d, pLow, n_ * LimbBytes);
	T->_mp_size = n_;
}

inline void MontInv(const mpz_t a, mpz_t res) {
	mpz_set_ui(tmp_, 1);
	MontMult(res, a, tmp_);
}

/**/
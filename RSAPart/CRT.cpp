/* 中国剩余定理 */
#include <cstdio>
#include <gmpxx.h>

using namespace std;

mpz_class CRT(const mpz_class& c1, const mpz_class& c2,
	const mpz_class& qqInv, const mpz_class& ppInv, const mpz_class& N);
mpz_class modInv(mpz_class a, mpz_class N);
mpz_class squareMult(const mpz_class& a, const mpz_class& e,
	const mpz_class& N);


int main() {
	int n;
	scanf("%d", &n);
	mpz_class p, q, e, d, c, c1, c2, res, 
		ppInv, qqInv, N, dmodp_1, dmodq_1;
	gmp_scanf("%Zd %Zd %Zd", p.get_mpz_t(), q.get_mpz_t(), e.get_mpz_t());
	d = modInv(e, (p - 1) * (q - 1));
	dmodp_1 = d % (p - 1), dmodq_1 = d % (q - 1);
	ppInv = modInv(p, q), qqInv = modInv(q, p);
	//modInv(p, q, ppInv, qqInv);
	ppInv *= p, qqInv *= q, N = p * q;
	for (int i = 0; i < n; ++i) {
		gmp_scanf("%Zd", c.get_mpz_t());
		c1 = squareMult(c % p, dmodp_1, p);
		c2 = squareMult(c % q, dmodq_1, q);
		res = CRT(c1, c2, qqInv, ppInv, N);
		gmp_printf("%Zd\n", res.get_mpz_t());
	}
	return 0;
}

//中国剩余定理
mpz_class CRT(const mpz_class& c1, const mpz_class& c2,
	const mpz_class& qqInv, const mpz_class& ppInv, const mpz_class& N) {
	mpz_class res;
	res = (c1 * qqInv + c2 * ppInv) % N;
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

mpz_class squareMult(const mpz_class& a, const mpz_class& e,
	const mpz_class& N) {
	mpz_class z(1);
	char* str = new char[100000];
	mpz_get_str(str, 2, e.get_mpz_t());
	for (unsigned i = 0; str[i] != '\0'; ++i) {
		z = (z * z) % N;
		if (str[i] == '1') z = (z * a) % N;
	}
	delete[] str;
	return z;
}


/**/
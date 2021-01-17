/* 平方乘算法 */
#include <cstdio>
#include <cstring>
#include <gmpxx.h>

using namespace std;

mpz_class squareMult(const mpz_class& a, const mpz_class& e,
	const mpz_class& n);


int main() {
	int n;
	scanf("%d", &n);
	for (int i = 0; i < n; ++i) {
		mpz_class e, m, p, q, res;
		gmp_scanf("%Zd %Zd %Zd %Zd",
			e.get_mpz_t(), m.get_mpz_t(), p.get_mpz_t(), q.get_mpz_t());
		res = squareMult(m, e, p * q);
		gmp_printf("%Zd\n", res.get_mpz_t());
	}
	return 0;
}

//平方乘 计算a^e(mod N)
mpz_class squareMult(const mpz_class& a, const mpz_class& e, 
	const mpz_class& N) {
	mpz_class z(1);
	char* str = new char[100000];
	mpz_get_str(str, 2, e.get_mpz_t());
	unsigned l = strlen(str);
	for (unsigned i = 0; i < l; ++i) {
		z = (z * z) % N;
		if (str[i] == '1') z = (z * (a % N)) % N;
	}
	delete[] str;
	return z;
}

/**/



/* RSA参数生成 */
#include <cstdio>
#include <ctime>
#include <cstring>
#include <gmpxx.h>

using namespace std;

bool MillerRabin(const mpz_class& n);
mpz_class squareMult(const mpz_class& a, const mpz_class& e,
	const mpz_class& n);
mpz_class gcd(mpz_class a, mpz_class b);
mpz_class modInv(mpz_class a, mpz_class N);


int main() {
	int n;
	scanf("%d", &n);
	mpz_class e, p, q, d;
	for (int i = 0; i < n; ++i) {
		gmp_scanf("%Zd %Zd %Zd", e.get_mpz_t(), p.get_mpz_t(), q.get_mpz_t());
		if (MillerRabin(p) && MillerRabin(q) && abs(p - q) > 10000
			&& gcd(p - 1, q - 1) < 5000 && gcd((p - 1) * (q - 1), e) == 1) {
			d = modInv(e, (p - 1) * (q - 1));
			gmp_printf("%Zd\n", d.get_mpz_t());
		}
		else {
			printf("ERROR\n");
		}
	}
	return 0;
}

//素性检测
bool MillerRabin(const mpz_class& n) {
	int s;
	mpz_class t(n - 1);
	for (s = 0; ((t >> s) & 1) == 0; ++s);
	t >>= s;
	gmp_randclass rd(gmp_randinit_mt);
	rd.seed(time(nullptr));
	mpz_class a = rd.get_z_range(n - 1) + 1;
	mpz_class b = squareMult(a, t, n);
	if (b == 1) return true;
	for (int i = 0; i < s; ++i) {
		if (b == n - 1) return true;
		else b = (b * b) % n;
	}
	return false;
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

//最大公约数
mpz_class gcd(mpz_class a, mpz_class b) {
	mpz_class r(a);
	while (r!=0) {
		r = a % b, a = b, b = r;
	}
	return a;
}

//模逆 计算a^(-1)(mod N)
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
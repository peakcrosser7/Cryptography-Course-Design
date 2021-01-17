/* PKCS7 */
#include <cstring>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/pkcs7.h>
#include <openssl/x509.h>

using namespace std;

//ca证书
const char* cacertStr = "\
-----BEGIN CERTIFICATE----- \n\
MIIB/zCCAaagAwIBAgIJAKKa0PAt9M1FMAoGCCqBHM9VAYN1MFsxCzAJBgNVBAYT \n\
AkNOMQ4wDAYDVQQIDAVIdUJlaTEOMAwGA1UEBwwFV3VIYW4xDTALBgNVBAoMBEhV \n\
U1QxDDAKBgNVBAsMA0NTRTEPMA0GA1UEAwwGY2Fyb290MB4XDTIwMDkyMDIwNTkx \n\
OVoXDTMwMDkxODIwNTkxOVowWzELMAkGA1UEBhMCQ04xDjAMBgNVBAgMBUh1QmVp \n\
MQ4wDAYDVQQHDAVXdUhhbjENMAsGA1UECgwESFVTVDEMMAoGA1UECwwDQ1NFMQ8w \n\
DQYDVQQDDAZjYXJvb3QwWTATBgcqhkjOPQIBBggqgRzPVQGCLQNCAASJ8mm28JJR \n\
bZKLr6DCo1+KWimpKEsiTfZM19Zi5ao7Au6YLosyN71256MWmjwkwXxJeLa0lCfm \n\
kF/YWCX6qGQ0o1MwUTAdBgNVHQ4EFgQUAL5hW3RUzqvsiTzIc1gUHeK5uzQwHwYD \n\
VR0jBBgwFoAUAL5hW3RUzqvsiTzIc1gUHeK5uzQwDwYDVR0TAQH/BAUwAwEB/zAK \n\
BggqgRzPVQGDdQNHADBEAiAaZMmvE5zzXHx/TBgdUhjtpRH3Jpd6OZ+SOAfMtKxD \n\
LAIgdKq/v2Jkmn37Y9U8FHYDfFqk5I0qlQOAmuvbVUi3yvM= \n\
-----END CERTIFICATE----- \n\
";
//B私钥
const char pkeyBStr[] = "\
-----BEGIN EC PARAMETERS----- \n\
BggqgRzPVQGCLQ== \n\
-----END EC PARAMETERS----- \n\
-----BEGIN EC PRIVATE KEY----- \n\
MHcCAQEEINQhCKslrI3tKt6cK4Kxkor/LBvM8PSv699Xea7kTXTToAoGCCqBHM9V \n\
AYItoUQDQgAEH7rLLiFASe3SWSsGbxFUtfPY//pXqLvgM6ROyiYhLkPxEulwrTe8 \n\
kv5R8/NA7kSSvcsGIQ9EPWhr6HnCULpklw== \n\
-----END EC PRIVATE KEY----- \n\
";

char p7Str[4096] = "\
-----BEGIN PKCS7-----\n\
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\n\
HXftWv0=\n\
-----END PKCS7----- \n\
", p7Res[4096];

X509* getCert(const char* cert);
PKCS7* getPKCS7(const char* p7Str);
EVP_PKEY* getpkey(const char* private_key);
bool verifySign(PKCS7* p7, BIO* p7Bio, X509* cacert);
int printErr();

int main() {
	//FILE* fp = fopen("4.in", "r");
	//fread(p7Str, sizeof(p7Str) - 1, 1, fp);
	fread(p7Str, sizeof(p7Str) - 1, 1, stdin);
	OpenSSL_add_all_algorithms();
	PKCS7* p7 = getPKCS7(p7Str);
	if (!p7) return printErr();
	EVP_PKEY* pkeyB = getpkey(pkeyBStr);
	if (!pkeyB) return printErr();
	//解码PKCS7获取原始数据
	BIO* p7Bio = PKCS7_dataDecode(p7, pkeyB, nullptr, nullptr);
	//将明文从BIO的原始数据中读取, 并获取明文长度
	int resLen = BIO_read(p7Bio, p7Res, sizeof(p7Res));
	if (resLen <= 0) return printErr();
	X509* cacert = getCert(cacertStr);
	if (!cacert) return printErr();
	//验证签名
	if (!verifySign(p7, p7Bio, cacert)) return printErr();
	for (int i = 0; i < resLen; ++i) putchar(p7Res[i]);
	
	X509_free(cacert);
	BIO_free(p7Bio);
	EVP_PKEY_free(pkeyB);
	//fclose(fp);
	return 0;
}

//输出错误
inline int printErr() {
	printf("ERROR");
	return 0;
}

//签名验证
bool verifySign(PKCS7* p7, BIO* p7Bio, X509* cacert) {
	//获取签名者信息栈
	STACK_OF(PKCS7_SIGNER_INFO)* sk = PKCS7_get_signer_info(p7);
	if (!sk) return false;
	//证书存储区:用于验证证书的结构体
	X509_STORE* store = X509_STORE_new();
	//证书存储区上下文环境
	X509_STORE_CTX* ct = X509_STORE_CTX_new();
	//添加信任的根证书到证书存储区
	X509_STORE_add_cert(store, cacert);	
	//获取签名者个数
	int signNum = sk_PKCS7_SIGNER_INFO_num(sk);
	int flag = true;
	for (int i = 0; i < signNum; ++i) {
		//获取签名者信息
		PKCS7_SIGNER_INFO* signInfo = sk_PKCS7_SIGNER_INFO_value(sk, i);
		//证书验证
		auto res = PKCS7_dataVerify(store, ct, p7Bio, p7, signInfo);
		PKCS7_SIGNER_INFO_free(signInfo);
		if (res <= 0) {
			flag = false;
			break;
		}
	}	
	X509_STORE_free(store);
	X509_STORE_CTX_free(ct);
	sk_PKCS7_SIGNER_INFO_free(sk);
	return flag;
}

//获取PKCS7结构体
PKCS7* getPKCS7(const char* p7Str) {
	BIO* p7Bio = BIO_new_mem_buf((char*)p7Str, strlen(p7Str));
	if (!p7Bio) return nullptr;
	PKCS7* ret = PEM_read_bio_PKCS7(p7Bio, nullptr, nullptr, nullptr);
	BIO_free(p7Bio);
	return ret;
}

//获取私钥结构体
EVP_PKEY* getpkey(const char* private_key) {
	BIO* bio_pkey = BIO_new_mem_buf((char*)private_key, strlen(private_key));
	if (bio_pkey == NULL)
		return NULL;
	//对私钥进行PEM格式读取
	EVP_PKEY* ret = PEM_read_bio_PrivateKey(bio_pkey, NULL, NULL, NULL);
	BIO_free(bio_pkey);
	return ret;
}

//获取X509格式证书
X509* getCert(const char* cert) {
	//openssl对于io类型的抽象封装
	BIO* bio = BIO_new(BIO_s_mem());	//生成一个mem类型的BIO
	BIO_puts(bio, cert);	//输入字符串buf到bio中
	//对X509结构保存的证书进行PEM格式读取
	X509* ret = PEM_read_bio_X509(bio, NULL, NULL, NULL);
	BIO_free(bio);
	return ret;
}

/**/
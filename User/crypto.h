//
// Created by kore on 9/8/21.
//

#ifndef kNN_Client_CRYPTO_H
#define kNN_Client_CRYPTO_H

#include<gmp.h>
#include<gmpxx.h>
#include<cmath>
#include<vector>

#define PaillierKeySize 1024
#define KeyBitLength 1024
#define maxSizeOfDigits 1024

#define const_num 115 //1024, 2048, 4096, 8192, 16384
#define train_number 115  //480 if is liver knn/580 all  ;  1728 if car knn
#define test_number 1
#define k_value 5
#define attribute_number 5 // 11 if liver; 6 if car ; 784 if mnist.
#define BASE_SK 2

#define fractional_part_bit 16
#define max_bit 20
#define class_number 4

#define BIT2BYTE(a) (a+7)>>3

#define MAXLINE 1024


typedef struct {
    mp_bitcnt_t len; /**< bit length of n */
    mpz_t lambda;		/**< least common multiple of p and q */
    mpz_t mu;			/**< Modular inverse */
    mpz_t p2;			/**< square of prime number p */
    mpz_t q2;			/**< square of prime number q */
    mpz_t p2invq2;		/**< CRT parameter p^{-2} mod q^2 */
    mpz_t ninv;			/**< modular inverse n^{-1} mod 2^len */
    mpz_t n;			/**< n=p*q */
    mpz_t d;			/**< our variant key where d = lambda - mu mod n */
} paillier_private_key_v; //variant

typedef struct {
    mp_bitcnt_t len; /**< bit length of n */
    mpz_t n; 			/**< modulus n */
} paillier_public_key;

typedef struct {
    mpz_t result; /**< result of exponentiation */
    mpz_t basis; /**< basis of exponentiation */
    mpz_t exponent; /**< exponent of exponentiation */
    mpz_t modulus; /**< modulus of exponentiation */
} exp_args;

inline void debug_msg(const char *str);

int gen_random(mpz_t rnd, mp_bitcnt_t len) ;

int gen_prime(mpz_t prime, mp_bitcnt_t len);

int paillier_ell(mpz_t result, mpz_t input, mpz_t ninv, mp_bitcnt_t len) ;

int crt_exponentiation(mpz_t result, mpz_t base, mpz_t exp_p, mpz_t exp_q, mpz_t pinvq, mpz_t p, mpz_t q);

int gen_pseudorandom(mpz_t rnd, mp_bitcnt_t len);

int crt_findD(paillier_private_key_v *priv);

int paillier_keygen(
        paillier_public_key *pub,
        paillier_private_key_v *priv,
        mp_bitcnt_t len);

int paillier_encrypt(
        mpz_t ciphertext,
        mpz_t plaintext,
        paillier_public_key *pub);

int paillier_decrypt_v(
        mpz_t plaintext,
        mpz_t ciphertext,
        paillier_private_key_v *priv);


int paillier_homomorphic_add(
        mpz_t ciphertext3,
        mpz_t ciphertext1,
        mpz_t ciphertext2,
        paillier_public_key *pub);



int paillier_homomorphic_multc(
        mpz_t ciphertext2,
        mpz_t ciphertext1,
        mpz_t constant,
        paillier_public_key *pub);

int paillier_homomorphic_multc_random(mpz_t ciphertext2, mpz_t ciphertext1, mpz_t constant, mpz_t random_r, paillier_public_key* pub);

void paillier_public_init(paillier_public_key *pub);

void paillier_private_init_v(paillier_private_key_v *priv);

int paillier_keygen_variant(paillier_public_key *pub, paillier_private_key_v *priv, mp_bitcnt_t len);

#endif //kNN_Client_CRYPTO_H

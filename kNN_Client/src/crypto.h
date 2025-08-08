//
// Created by kore on 9/8/21.
//

#ifndef kNN_Client_CRYPTO_H
#define kNN_Client_CRYPTO_H

#include<gmp.h>
#include<gmpxx.h>
#include<cmath>
#include<vector>

#include "MyTime.h"

#define PaillierKeySize 1024
#define KeyBitLength 1024 //4:512
#define maxSizeOfDigits 1024 //4:512

#define const_num 1024 //1024, 2048, 4096, 8192, 16384
#define train_number 115  //480 if is liver knn/580 all  ;  1728 if car knn
#define test_number 35
#define k_value 5
#define attribute_number 5 // 11 if liver; 6 if car ; 784 if mnist.
#define BASE_SK 2

#define fractional_part_bit 16

#define BIT2BYTE(a) (a+7)>>3


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

typedef struct {
    paillier_public_key *pk;
    mpz_t Ds[KeyBitLength];
} hss_pk_p; //paillier hss pk

typedef struct {
    mpz_t kprf;
    //struct paillier_private_key pk;
    mpz_t ds[KeyBitLength];
} hss_ek_p; //paillier hss pk

typedef struct {
    mpz_t encx;
    mpz_t encdx[KeyBitLength];
} hss_input_p; //paillier hss input

typedef struct {
    mpz_t sharex;
    mpz_t sharedx[KeyBitLength];
} hss_men_p; //paillier hss memory value

struct node_paillier
{
    mpz_t labels; //encrypted? hss_men_p furthur
    mpz_t dis;
};

struct node
{
    int labels;
    int dis;
};

struct TrainingParams{
    int n, d;
    //double alpha = 5.0/LEARNING_RATE_INV;
};



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

int DDLog(mpz_t distance, mpz_t ciphertext1, paillier_public_key *pub);

void paillier_public_init(paillier_public_key *pub);

void paillier_private_init_v(paillier_private_key_v *priv);

//void decompose_into_digits(mpz_t number, vector<mpz_t> &digits, mpz_t base_sk, paillier_public_key* pub);

void decompose_into_digits(mpz_t number, mpz_t digits[], paillier_public_key* pub);

void setup_PKI_out(hss_pk_p *hpk, hss_ek_p *hek0, hss_ek_p *hek1, int * size);

void setup_PKI(hss_pk_p* hpk, hss_ek_p* hek0, hss_ek_p* hek1);

void input_paillier(hss_input_p* Ix, hss_pk_p* hpk, mpz_t number);

void input_paillier_value(hss_input_p *Ix, hss_pk_p *hpk, mpz_t number);

void input_paillier_clear(hss_input_p *Ix);

void mult_shares_paillier(hss_men_p* Mx2, hss_men_p* Mx, hss_input_p* Ix, hss_ek_p* ek, paillier_public_key* pub, int serverId);

void convert_shares_paillier(hss_men_p* Mx, hss_men_p* Mof1, hss_input_p* Ix, hss_ek_p* ek, paillier_public_key* pub, int serverId);

void add_shares_paillier(hss_men_p* Mz, hss_men_p* Mx, hss_men_p* My, hss_ek_p* ek, paillier_public_key* pub, int serverId);

void sub_shares_paillier(hss_men_p* Mz, hss_men_p* Mx, hss_men_p* My, hss_ek_p* ek, paillier_public_key* pub, int serverId);

void add_inputs_paillier(hss_input_p* Iz, hss_input_p* Ix, hss_input_p* Iy, hss_ek_p* ek, paillier_public_key* pub, int serverId);

void output_paillier(mpz_t out, hss_men_p* Mz, hss_ek_p* ek, paillier_public_key* pub, int serverId);

void men_share_init(hss_men_p* Mz);

void men_share_zeros(hss_men_p* Mz);

void men_share_clear(hss_men_p* Mz);

void share_divc(mpz_t sharex, mpz_t share, int constNum);

void men_share_divc(hss_men_p* Mz, int constNum);

void men_share_multc(hss_men_p* Mz, hss_men_p* Mx, int constNum, paillier_public_key *pub);

void input_copy_pailliar(hss_input_p* Mz, hss_input_p* Mx);

void men_copy_pailliar(hss_men_p* Mz, hss_men_p* Mx);

int fromINTtoMEN(hss_men_p * mnum11, hss_men_p * mnum12, hss_men_p * mof11, hss_men_p * mof12, hss_pk_p* hpk, hss_ek_p * ek0, hss_ek_p * ek1, mpz_t num, MyTime* iktime);

int fromINTtoMEN_value(hss_men_p * mnum11, hss_men_p * mnum12, hss_men_p * mof11, hss_men_p * mof12, hss_pk_p* hpk, hss_ek_p * ek0, hss_ek_p * ek1, mpz_t num, MyTime* iktime);

int fromINTtoMEN_single(hss_men_p * mnum11, hss_men_p * mnum12, hss_men_p * mof11, hss_men_p * mof12, hss_pk_p* hpk, hss_ek_p * ek0, hss_ek_p * ek1, mpz_t num, MyTime* iktime, int server_id);

int fromINTtoMEN_value_single(hss_men_p * mnum11, hss_men_p * mnum12, hss_men_p * mof11, hss_men_p * mof12, hss_pk_p* hpk, hss_ek_p * ek0, hss_ek_p * ek1, mpz_t num, MyTime* iktime, int server_id);

void input_share_multc(hss_input_p * Iz, hss_input_p * Ix, int constNum, paillier_public_key *pub);

int server_output(mpz_t data[], int server_id, char * output_file_name);

int server_input(mpz_t data[], int server_id, char * output_file_name);

int DO_write_input(std::vector<std::vector<hss_input_p> >* Xi1, mpz_t Yw1[], int server_id,
                   char * output_file1_name);

int client_write_input(std::vector<std::vector<hss_input_p> >* Xi1, int server_id,
                       char * output_file1_name);

int server_read_input(std::vector<std::vector<hss_input_p> >* Xi1, mpz_t Yw1[],
                      int server_id, char * output_file1_name);

int subtractive_share(mpz_t x1, mpz_t x2, int x, mpz_t n);

#endif //kNN_Client_CRYPTO_H

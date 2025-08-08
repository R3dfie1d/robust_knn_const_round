//
// Created by kore on 9/8/21.
//

#include <iostream>
#include <cmath>
#include "crypto.h"

using namespace std;

//paillier parameters
// where already have #define KeyBitLength 64 and maxSizeOfDigits 64
mpz_t base_sk, base_msg;
int secretKeyLength;
int securityK = 16; //furthur change to replace KeyBitLength, but not very sure about the genPrime algorithm.
int sizeOfDigits;


inline void debug_msg(const char *str) {
#ifdef PAILLIER_DEBUG
    fputs(str, stderr);
#endif
}

int paillier_ell(mpz_t result, mpz_t input, mpz_t ninv, mp_bitcnt_t len) {
    mpz_t mask;

    mpz_init(mask);

    mpz_sub_ui(result, input, 1);
    mpz_mul(result, result, ninv);
    mpz_setbit(mask, len);
    mpz_sub_ui(mask, mask, 1);
    mpz_and(result, result, mask);

    mpz_clear(mask);
    return 0;
}
int gen_random(mpz_t rnd, mp_bitcnt_t len) {
    FILE *dev_random, *dev_urandom;
    int byte_count, byte_read;
    char * seed;

    byte_count = BIT2BYTE(len);

    dev_random = fopen("/dev/random", "r");
    if(dev_random == NULL) {
        fprintf(stderr, "cannot open random number device!\n");
        exit(1);
    }
    dev_urandom = fopen("/dev/urandom", "r");
    if(dev_urandom == NULL) {
        fprintf(stderr, "cannot open random number device!\n");
        exit(1);
    }

    seed = (char *)malloc(sizeof(char)*byte_count);

    byte_read = 0;
    //generate the first 16 bytes with /dev/random
    while(byte_read  < 16 && byte_read < byte_count) {
        byte_read += fread(seed, sizeof(char), byte_count, dev_random);
    }
    fclose(dev_random);
    //generate the remaining bytes with /dev/urandom
    while(byte_read < byte_count) {
        byte_read += fread(seed, sizeof(char), byte_count, dev_urandom);
    }
    fclose(dev_urandom);

    mpz_import(rnd, byte_count, 1, sizeof(seed[0]), 0, 0, seed);
    free(seed);
    return 0;
}

int gen_prime(mpz_t prime, mp_bitcnt_t len) {
    mpz_t rnd;

    mpz_init(rnd);

    gen_random(rnd, len);

    //set most significant bit to 1
    mpz_setbit(rnd, len-1);
    //look for next prime
    mpz_nextprime(prime, rnd);

    mpz_clear(rnd);
    return 0;
}

int crt_exponentiation(mpz_t result, mpz_t base, mpz_t exp_p, mpz_t exp_q, mpz_t pinvq, mpz_t p, mpz_t q) {
    mpz_t pq;
    exp_args *args_p, *args_q;

#ifdef PAILLIER_THREAD
    pthread_t thread1, thread2;
#endif

    mpz_init(pq);

    //prepare arguments for exponentiation mod p
    args_p = (exp_args *)malloc(sizeof(exp_args));

    mpz_init(args_p->result);
    mpz_init(args_p->basis);
    mpz_init(args_p->exponent);
    mpz_init(args_p->modulus);

    mpz_set(args_p->basis, base);
    mpz_set(args_p->exponent, exp_p);
    mpz_set(args_p->modulus, p);

    //prepare arguments for exponentiation mod q
    args_q = (exp_args *)malloc(sizeof(exp_args));

    mpz_init(args_q->result);
    mpz_init(args_q->basis);
    mpz_init(args_q->exponent);
    mpz_init(args_q->modulus);

    mpz_set(args_q->basis, base);
    mpz_set(args_q->exponent, exp_q);
    mpz_set(args_q->modulus, q);

#ifdef PAILLIER_THREAD
    //compute exponentiation modulo p
	pthread_create(&thread1, NULL, do_exponentiate, (void *)args_p);

	//compute exponentiation modulo q
	pthread_create(&thread2, NULL, do_exponentiate, (void *)args_q);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

#else
    //compute exponentiation modulo p
    mpz_mod(args_p->result, base, p);
    mpz_powm(args_p->result, args_p->result, exp_p, p);

    //compute exponentiation modulo q
    mpz_mod(args_q->result, base, q);
    mpz_powm(args_q->result, args_q->result, exp_q, q);
#endif

    //recombination
    mpz_mul(pq, p, q);
    mpz_sub(result, args_q->result, args_p->result);
    mpz_mul(result, result, p);
    mpz_mul(result, result, pinvq);
    mpz_add(result, result, args_p->result);
    mpz_mod(result, result, pq);

    mpz_clear(pq);
    mpz_clear(args_p->result);
    mpz_clear(args_p->basis);
    mpz_clear(args_p->exponent);
    mpz_clear(args_p->modulus);
    mpz_clear(args_q->result);
    mpz_clear(args_q->basis);
    mpz_clear(args_q->exponent);
    mpz_clear(args_q->modulus);
    free(args_p);
    free(args_q);

    return 0;
}

int crt_findD(paillier_private_key_v *priv){
    mpz_t tmp1;
    mpz_t tmp2;
    mpz_t tmp3;
    mpz_init(tmp1);
    mpz_init(tmp2);
    mpz_init(tmp3);

    mpz_gcd(tmp1, priv->lambda, priv->n);
    if(mpz_cmp_ui(tmp1, 1) != 0) return 0;

    //mpz_invert(tmp1, priv->n, priv->lambda);

    mpz_invert(tmp2, priv->lambda, priv->n);

    mpz_mul(tmp3, priv->lambda, priv->n);

    mpz_mul(priv->d, priv->lambda, tmp2);
    mpz_mod(priv->d, priv->d, tmp3);

    return 1;
}

int paillier_keygen_variant(paillier_public_key *pub, paillier_private_key_v *priv, mp_bitcnt_t len) {
    mpz_t p, q, n2, temp, mask, g;

    mpz_t tmp1;
    mpz_t tmp2;
    mpz_t tmp3;
    mpz_init(tmp1);
    mpz_init(tmp2);
    mpz_init(tmp3);

    mpz_init(p);
    mpz_init(q);
    mpz_init(n2);
    mpz_init(temp);
    mpz_init(mask);
    mpz_init(g);

    //write bit lengths
    priv->len = len;
    pub->len = len;

    //generate p and q
    debug_msg("generating prime p\n");
    gen_prime(p, len/2);
    cout << "p: " << p << endl;
    debug_msg("generating prime q\n");
    gen_prime(q, len/2);
    cout << "q: " << q << endl;

    //calculate modulus n=p*q
    debug_msg("calculating modulus n=p*q\n");
    mpz_mul(pub->n, p, q);
    mpz_mul(priv->n, p, q);
    cout << "n: " << pub->n << endl;

    //set g = 1+n
    debug_msg("calculating basis g=1+n\n");
    mpz_add_ui(g, pub->n, 1);

    //compute n^{-1} mod 2^{len}
    debug_msg("computing modular inverse n^{-1} mod 2^{len}\n");
    mpz_setbit(temp, len);
    if(!mpz_invert(priv->ninv, pub->n, temp)) {
        fputs("Inverse does not exist!\n", stderr);
        mpz_clear(p);
        mpz_clear(q);
        mpz_clear(n2);
        mpz_clear(temp);
        mpz_clear(mask);
        mpz_clear(g);
        exit(1);
    }

    //compute p^2 and q^2
    mpz_mul(priv->p2, p, p);
    mpz_mul(priv->q2, q, q);

    //generate CRT parameter
    debug_msg("calculating CRT parameter p^{-2} mod q^2\n");
    mpz_invert(priv->p2invq2, priv->p2, priv->q2);

    //calculate lambda = lcm(p-1,q-1)
    debug_msg("calculating lambda=lcm((p-1)*(q-1))\n");
    mpz_clrbit(p, 0);
    mpz_clrbit(q, 0);
    mpz_lcm(priv->lambda, p, q);

    //calculate n^2
    mpz_mul(n2, pub->n, pub->n);

    //calculate mu
    debug_msg("calculating mu\n");
    crt_exponentiation(temp, g, priv->lambda, priv->lambda, priv->p2invq2, priv->p2, priv->q2);

    paillier_ell(temp, temp, priv->ninv, len);

    if(!mpz_invert(priv->mu, temp, pub->n)) {
        fputs("Inverse does not exist!\n", stderr);
        mpz_clear(p);
        mpz_clear(q);
        mpz_clear(n2);
        mpz_clear(temp);
        mpz_clear(mask);
        mpz_clear(g);
        exit(1);
    }
/*
  mpz_t tmp1;
  mpz_init(tmp1);

  if(!mpz_invert(tmp1, priv->lambda, pub->n)) {
		fputs("Inverse does not exist!\n", stderr);
		mpz_clear(p);
		mpz_clear(q);
		mpz_clear(n2);
		mpz_clear(temp);
		mpz_clear(mask);
		mpz_clear(g);
		exit(1);
	}
*/


    //mpz_add(priv->d, priv->lambda, priv->mu);
    //mpz_sub_ui(priv->d, priv->d, 2);

    crt_findD(priv);

    //mpz_lcm(tmp1, priv->lambda, priv->n);

    //mpz_mul();


    //mpz_sub(priv->d, priv->lambda, priv->mu);
    //mpz_mod(priv->d, priv->d, priv->n);

    //free memory and exit
    debug_msg("freeing memory\n");
    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(n2);
    mpz_clear(temp);
    mpz_clear(mask);
    mpz_clear(g);
    debug_msg("exiting\n");
    return 0;
}

int gen_pseudorandom(mpz_t rnd, mp_bitcnt_t len) {
    FILE *dev_urandom;
    int byte_count, byte_read;
    char * seed;

    byte_count = BIT2BYTE(len);

    dev_urandom = fopen("/dev/urandom", "r");
    if(dev_urandom == NULL) {
        fprintf(stderr, "cannot open random number device!\n");
        exit(1);
    }

    seed = (char *)malloc(sizeof(char)*byte_count);

    byte_read = 0;
    //generate the bytes with /dev/urandom
    while(byte_read < byte_count) {
        byte_read += fread(seed, sizeof(char), byte_count, dev_urandom);
    }
    fclose(dev_urandom);

    mpz_import(rnd, byte_count, 1, sizeof(seed[0]), 0, 0, seed);
    free(seed);
    return 0;
}

int paillier_encrypt(mpz_t ciphertext, mpz_t plaintext, paillier_public_key *pub) {
    mpz_t n2, r;
    //cout << pub->n << endl;
    if(mpz_cmp(pub->n, plaintext)) {
        //cout << "entering." << endl;
        mpz_init(n2);
        mpz_init(r);

        //re-compute n^2

        mpz_mul(n2, pub->n, pub->n);

        debug_msg("generating random number\n");
        //cout << "generating random number" << endl;
        //generate random r and reduce modulo n
        gen_pseudorandom(r, pub->len);
        mpz_mod(r, r, pub->n);
        if(mpz_cmp_ui(r, 0) == 0) {

            cout << pub->len << endl;
            cout << pub->n << endl;
            cout << r << endl;

            fputs("random number is zero!\n", stderr);

            mpz_clear(n2);
            mpz_clear(r);
            exit(1);
        }

        debug_msg("computing ciphertext\n");
        //cout << "computing ciphertext" << endl;
        //compute r^n mod n2
        mpz_powm(ciphertext, r, pub->n, n2);

        //compute (1+m*n)
        mpz_mul(r, plaintext, pub->n);
        mpz_add_ui(r, r, 1);

        //multiply with (1+m*n)
        mpz_mul(ciphertext, ciphertext, r);
        mpz_mod(ciphertext, ciphertext, n2);

        debug_msg("freeing memory\n");
        mpz_clear(n2);
        mpz_clear(r);
    }
    debug_msg("exiting\n");
    return 0;
}

int paillier_decrypt_v(mpz_t plaintext, mpz_t ciphertext, paillier_private_key_v *priv) {
    debug_msg("computing plaintext\n");
    //compute exponentiation c^lambda mod n^2
    crt_exponentiation(plaintext, ciphertext, priv->d, priv->d, priv->p2invq2, priv->p2, priv->q2);

    //compute exponentiation c^lambda mod n^2
    //compute n2
    //mpz_powm(plaintext, ciphertext, priv->d, priv->n2);

    //compute L(c^lambda mod n^2)
    paillier_ell(plaintext, plaintext, priv->ninv, priv->len);

    //compute L(c^lambda mod n^2)*mu mod n
    //mpz_mul(plaintext, plaintext, priv->mu);
    mpz_mod(plaintext, plaintext, priv->n);

    debug_msg("exiting\n");
    return 0;
}

/**
 * "Add" two plaintexts homomorphically by multiplying ciphertexts modulo n^2.
 * For example, given the ciphertexts c1 and c2, encryptions of plaintexts m1 and m2,
 * the value c3=c1*c2 mod n^2 is a ciphertext that decrypts to m1+m2 mod n.
 */
int paillier_homomorphic_add(mpz_t ciphertext3, mpz_t ciphertext1, mpz_t ciphertext2, paillier_public_key *pub) {
    mpz_t n2;

    mpz_init(n2);
    debug_msg("compute n^2");
    mpz_mul(n2, pub->n, pub->n);

    debug_msg("homomorphic add plaintexts");
    mpz_mul(ciphertext3, ciphertext1, ciphertext2);
    mpz_mod(ciphertext3, ciphertext3, n2);

    debug_msg("freeing memory\n");
    mpz_clear(n2);
    debug_msg("exiting\n");
    return 0;
}

/**
 * "Multiplies" a plaintext with a constant homomorphically by exponentiating the ciphertext modulo n^2 with the constant as exponent.
 * For example, given the ciphertext c, encryptions of plaintext m, and the constant 5,
 * the value c3=c^5 n^2 is a ciphertext that decrypts to 5*m mod n.
 */
int paillier_homomorphic_multc(mpz_t ciphertext2, mpz_t ciphertext1, mpz_t constant, paillier_public_key *pub) {
    mpz_t n2;

    mpz_init(n2);
    debug_msg("compute n^2");
    mpz_mul(n2, pub->n, pub->n);

    debug_msg("homomorphic multiplies plaintext with constant");
    mpz_powm(ciphertext2, ciphertext1, constant, n2);

    debug_msg("freeing memory\n");
    mpz_clear(n2);
    debug_msg("exiting\n");
    return 0;
}

int paillier_homomorphic_multc_random(mpz_t ciphertext2, mpz_t ciphertext1, mpz_t constant, mpz_t random_r, paillier_public_key* pub) {


    mpz_t n2;
    mpz_t rn;

    mpz_init(n2);
    mpz_init(rn);

    debug_msg("compute n^2");
    mpz_mul(n2, pub->n, pub->n);

    debug_msg("homomorphic multiplies plaintext with constant");
    mpz_powm(rn, random_r, pub->n, n2);

    mpz_powm(ciphertext2, ciphertext1, constant, n2);
    mpz_mul(ciphertext2, ciphertext2, rn);
    mpz_mod(ciphertext2, ciphertext2, n2);

    debug_msg("freeing memory\n");
    mpz_clear(n2);
    mpz_clear(rn);
    debug_msg("exiting\n");
    return 0;
}

void paillier_public_init(paillier_public_key *pub) {
    mpz_init(pub->n);
}


void paillier_private_init_v(paillier_private_key_v *priv) {
    mpz_init(priv->lambda);
    mpz_init(priv->mu);
    mpz_init(priv->p2);
    mpz_init(priv->q2);
    mpz_init(priv->p2invq2);
    mpz_init(priv->ninv);
    mpz_init(priv->n);
    mpz_init(priv->d);
}



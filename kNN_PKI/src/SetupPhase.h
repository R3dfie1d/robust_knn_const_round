//
// Created by kore on 9/8/21.
//

#ifndef kNN_PKI_SETUPPHASE_H
#define kNN_PKI_SETUPPHASE_H


#include "crypto.h"

class SetupPhase {
public:
    paillier_public_key pks;
    hss_pk_p hpk;
    hss_ek_p hek0, hek1;
    hss_men_p mof11;
    hss_men_p mof12;

    mpz_t prf_key;
    mpz_t prp_key;

    int KeySize;


    SetupPhase(){
        int result = 0;

        mpz_t randTmp;
        mpz_init(randTmp);

        mpz_init(pks.n);
        hpk.pk = &pks;

        //int KeySize;
        setup_PKI_out(&hpk, &hek0, &hek1, &KeySize);

        mpz_init(mof11.sharex);
        mpz_init(mof12.sharex);

        gmp_randstate_t rstate;
        gmp_randinit_mt(rstate);

        mpz_urandomm(randTmp, rstate, (hpk.pk)->n);
        mpz_set(mof11.sharex, randTmp);
        mpz_add_ui(randTmp, randTmp, 1);
        mpz_set(mof12.sharex, randTmp);

        mpz_init(prf_key);
        mpz_init(prp_key);

        mpz_urandomm(randTmp, rstate, (hpk.pk)->n);
        mpz_set(prf_key, randTmp);

        mpz_urandomm(randTmp, rstate, (hpk.pk)->n);
        mpz_set(prp_key, randTmp); //still not export

        mpz_set(hek0.kprf, prf_key);
        mpz_set(hek1.kprf, prf_key);


        for (int i = 0; i < KeySize; i++) {
            //cout << hek0.ds[i] << endl;
            mpz_init_set(mof11.sharedx[i], hek0.ds[i]);
            mpz_init_set(mof12.sharedx[i], hek1.ds[i]);
        }
    }

    SetupPhase(char const * file_path){
        int result = 0;

        mpz_t randTmp;
        mpz_init(randTmp);

        mpz_init(pks.n);
        hpk.pk = &pks;

        setup_PKI_out(&hpk, &hek0, &hek1, &KeySize);
        std::cout << "simulated initialize complete." << std::endl;
        //Not suppose to do this, but for convinient.

        mpz_init(mof11.sharex);
        mpz_init(mof12.sharex);
/*
        gmp_randstate_t rstate;
        gmp_randinit_mt(rstate);

        mpz_urandomm(randTmp, rstate, (hpk.pk)->n);
        mpz_set(mof11.sharex, randTmp);
        mpz_add_ui(randTmp, randTmp, 1);
        mpz_set(mof12.sharex, randTmp);
*/

        for (int i = 0; i < KeySize; i++) {
            //cout << hek0.ds[i] << endl;
            mpz_init(mof11.sharedx[i]);
            mpz_init(mof12.sharedx[i]);
        }
        importKeys(file_path);

    }

    int exprotKeys(char const * file_path);
    int importKeys(char const * file_path);
};



#endif //kNN_PKI_SETUPPHASE_H

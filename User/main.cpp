#include <iostream>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <cmath>


#include <gmp.h>
#include <gmpxx.h>

#include "polynomial.h"
#include "crypto.h"
#include "RSA.h"

#define N 1


using namespace std;

int server_port_0 = 8888;
int server_port_1 = 9999;

paillier_public_key * glo_pub;

public_key * glo_rsa_pub;

int client_enc_input(mpz_t Q[], mpz_t Q2, mpz_t rc, int q[], paillier_public_key * pk){
    int result, sum=0;
    mpz_t tmp;
    mpz_init(tmp);
    for (int t = 0; t < attribute_number; t++) { //read a train samples t to Xi
        mpz_set_ui(tmp, q[t]);
        result = paillier_encrypt(Q[t], tmp, pk);
        sum += q[t] * q[t];
        //mpz_set_ui(Q[t], t);
    }
    mpz_set_ui(tmp, sum);
    result = paillier_encrypt(Q2, tmp, pk);
    //if(server_id == 0) mpz_set_ui(Q2, 33333);
    //else mpz_add_ui(Q2, tmp, 33333);

    gmp_randstate_t state;
    gmp_randinit_mt(state);
    mpz_urandomm(rc, state, pk->n);

    mpz_clear(tmp);

    return 0;
}

int QueryEnc(){
    int result;
    paillier_public_key pub;
    paillier_private_key_v priv;

    //initialize keys
    cout << "System keygen: " << endl;
    paillier_public_init(&pub);
    paillier_private_init_v(&priv);
    //paillier_public_init(hpk->pk);
    //generate public key and private key of PAILLIER_DEBUG
    result = paillier_keygen_variant(&pub, &priv, PaillierKeySize/2);

    //query();
    int preqi[attribute_number] = {0};
    mpz_t preQ[attribute_number];
    mpz_t preQ2;
    mpz_t rc;

    //mpz_t result_values[train_number+100];

    for (int t = 0; t < attribute_number; t++) { //read a train samples t to Xi
        mpz_init(preQ[t]);
        mpz_set_ui(preQ[t], preqi[t]);
    }

    mpz_init(preQ2);
    mpz_init(rc);
    client_enc_input(preQ, preQ2, rc, preqi, &pub);

    return 0;
}

int QueryEnc_ERDKNN(){
    int result;
    paillier_public_key pub;
    paillier_private_key_v priv;

    //initialize keys
    cout << "System keygen: " << endl;
    paillier_public_init(&pub);
    paillier_private_init_v(&priv);
    //paillier_public_init(hpk->pk);
    //generate public key and private key of PAILLIER_DEBUG
    result = paillier_keygen_variant(&pub, &priv, PaillierKeySize/2);

    //query();
    int preqi[attribute_number] = {0};
    mpz_t preQ[attribute_number];
    mpz_t tmp;

    for (int t = 0; t < attribute_number; t++) { //read a train samples t to Xi
        mpz_init(preQ[t]);
        mpz_set_ui(preQ[t], preqi[t]);
    }
    mpz_init(tmp);

    /////////////////////////
    int sum=0;
    for (int t = 0; t < attribute_number; t++) { //read a train samples t to Xi
        mpz_set_ui(tmp, preqi[t]);
        result = paillier_encrypt(preQ[t], tmp, &pub);
    }


    return 0;
}

int SEEnc(mpz_t cipher1, mpz_t cipher2, mpz_t plain){
    paillier_public_key * pub = glo_pub;

    mpz_t c, m;
    int result;

    mpz_init(c);
    mpz_init(m);
    //paillier_public_init(&pub);

    //import public key
    //debug_msg("importing public key: \n");
    //paillier_public_in_str(pub, public_key);

    //convert plaintext from stream
    //debug_msg("importing plaintext: \n");
    //gmp_fscanf(plaintext, "%Zx\n", m);
    mpz_set(m, plain);
    if(mpz_cmp(m, pub->n) >= 0) {
        fputs("Warning, plaintext is larger than modulus n!\n", stderr);
    }

    //calculate encryption
    //result = paillier_encrypt(c, m, pub);
    ///
    mpz_t n2, r, tmp;
    mpz_init(tmp);

    if(mpz_cmp(pub->n, m)) {
        mpz_init(n2);
        mpz_init(r);

        //re-compute n^2
        mpz_mul(n2, pub->n, pub->n);

        debug_msg("generating random number\n");
        //generate random r and reduce modulo n
        gen_pseudorandom(r, pub->len);
        mpz_mod(r, r, pub->n);
        if(mpz_cmp_ui(r, 0) == 0) {
            fputs("random number is zero!\n", stderr);
            mpz_clear(n2);
            mpz_clear(r);
            exit(1);
        }

        debug_msg("computing ciphertext\n");
        mpz_mul(tmp, m, pub->n);
        mpz_add_ui(tmp, tmp, 1);
        mpz_add_ui(c, pub->n, 1);
        mpz_powm(c, c, r, n2);
        mpz_mul(c, c, tmp);
        mpz_mod(cipher1, c, n2);

        mpz_add_ui(c, pub->n, 1);
        mpz_powm(c, c, r, n2);
        mpz_mod(cipher2, c, n2);

        debug_msg("freeing memory\n");
        mpz_clear(n2);
        mpz_clear(r);
        mpz_clear(tmp);
    }

    //debug_msg("freeing memory\n");
    mpz_clear(c);
    mpz_clear(m);
    //paillier_public_clear(pub);

    //debug_msg("exiting\n");

    //modified paillier
    return 0;
}

int ACEnc(mpz_t rsa_C, mpz_t rsa_M){
    block_encrypt(rsa_C, rsa_M, *glo_rsa_pub);
    return 0;
}


int RSAKeyGen(private_key * ku, public_key * kp){
    // Initialize public key
    mpz_init(kp->n);
    mpz_init(kp->e);
    // Initialize private key
    mpz_init(ku->n);
    mpz_init(ku->e);
    mpz_init(ku->d);
    mpz_init(ku->p);
    mpz_init(ku->q);
    generate_keys(ku, kp);

    return 0;
}

int QueryEnc_RPKNN(){
    //================PKI====================================



    mpz_t result1, result2, op1, op2, modtmp;
    mpz_init_set_ui(result1, train_number);
    mpz_init_set_ui(result2, train_number);
    mpz_init_set_ui(op1, attribute_number);
    mpz_init_set_ui(op2, attribute_number);
    mpz_init_set_ui(modtmp, attribute_number);

    mpz_t a[attribute_number];
    mpz_t Q[attribute_number], V[attribute_number];
    for(int i =0; i< attribute_number; i++){
        mpz_init(Q[i]); //input
        mpz_init(V[i]); //randomly choose
    }

    //SQ=mpz_t
    mpz_t sumQ2;
    mpz_init(sumQ2);
    mpz_set_ui(sumQ2, 0);
    for(int i =0; i< attribute_number; i++){
        mpz_add(sumQ2, sumQ2, Q[i]);
    }
    //SQ2=mpz_t
    mpz_t sumQ, tmp;
    mpz_init(sumQ);
    mpz_init(tmp);
    mpz_set_ui(sumQ, 0);

    for(int i =0; i< attribute_number; i++){
        mpz_add(tmp, Q[i], V[i]);
        mpz_mul(tmp, tmp, a[i]);
        mpz_add(sumQ, sumQ, tmp);
    }
    mpz_mul_ui(sumQ, sumQ, 2);
    //SV=mpz_t
    mpz_t sumV;
    mpz_init(sumV);
    for(int i =0; i< attribute_number; i++){
        mpz_mul(tmp, a[i], V[i]);
        mpz_add(sumV, sumV, tmp);
    }
    mpz_mul_ui(sumV, sumV, 2);
    //Enc(SQ2, SV, SQ)
    SEEnc(result1, result2, op1);//SQ2
    ACEnc(result1, op1);// SV,SQ

    //B_to_C1(2);//ESQ2, CPSQ
    //B_to_C2(1);//CPSV
    return 0;
}

int OT_test(){

    int choice[k_value];
    //mpz_t random[k_value];
    for(int i=0; i<k_value; i++){
        choice[i] = i+1;
    }

    vector<int> r;
    vector<double> co;

    r.push_back(0);
    co.push_back(0);
    r.push_back(1);
    co.push_back(1);

    vector<polynomial*> choice_poly;

    for(int i=0; i<k_value; i++){
        co[0] = choice[i];

        polynomial * polyi = new polynomial(2, r, co);
        choice_poly.push_back(polyi);
    }

    polynomial product_poly = (*choice_poly[0]);
    for(int i=1; i<k_value; i++){
        product_poly = (product_poly) * (*choice_poly[i]);
    }
    /*
    vector<int> r;
    vector<double> co;
    for(int i=0; i<k_value; i++){
        r.push_back(i);
        co.push_back(i*1.1);
    }
    polynomial * poly = new polynomial(k_value, r, co);
    */
    long result_params[k_value+1] = {0};
    polynomial result = product_poly.sort(result_params);



    cout << result << endl;
    for(int i=0; i<k_value+1; i++){
        cout << "b" << i << "=" << result_params[i] << endl;
    }
    //polynomial(int K, std::vector<int> r, std::vector<double> co)

    return 0;
}

int mpz_poly(mpz_t result, mpz_t params[], int len, long input){
    mpz_t product;
    mpz_t tmp;
    mpz_init(product);
    mpz_init(tmp);
    mpz_set_ui(product, 0);

    double tmpi;

    for(int i=0; i<len; i++){//k_value
        tmpi = pow(input, i);
        mpz_mul_ui(tmp, params[i], tmpi);
        mpz_add(product, product, tmp);
    }
    mpz_set(result, product);

    mpz_clear(product);
    mpz_clear(tmp);
    return 0;
}

int oblivious_transfer(){
    cout << "Simulate k-out-of-n OT." << endl;
    struct timeval t1,t2;
    double timeuse;
    gettimeofday(&t1, NULL);

    //init
    mpz_t p, q, g, h, n;
    mpz_init(p);
    mpz_init(q);
    mpz_init(g);
    mpz_init(h);
    mpz_init(n);
    mpz_set_ui(q, 5);
    mpz_set_ui(p, 11);
    mpz_set_ui(g, 3);
    mpz_set_ui(h, 7);
    mpz_set_ui(n, 55);

    mpz_t tmp, tmp2;
    mpz_init(tmp);
    mpz_init(tmp2);

    gmp_randstate_t state;
    gmp_randinit_mt(state);

    mpz_t A[k_value];
    mpz_t coe_a[k_value+1];
    long coe_b[k_value+1] = {0};
    mpz_t ci[const_num][2];
    mpz_t result_label[k_value];

    for(int i=0; i< k_value; i++){
        mpz_init(A[i]);
        mpz_init(coe_a[i]);
        mpz_init(result_label[i]);
    }

    int choice[k_value];
    //mpz_t random[k_value];
    for(int i=0; i<k_value; i++){
        choice[i] = i+1;
    }

    //generating f(x) = a[]x, f'(x) = b[]x
    //1. randomly generate a[]
    for(int i=0; i<k_value; i++){
        mpz_urandomm(coe_a[i], state, q);
    }
    //2. calculating b[]
    vector<int> r;
    vector<double> co;

    r.push_back(0);
    co.push_back(0);
    r.push_back(1);
    co.push_back(1);

    vector<polynomial*> choice_poly;

    for(int i=0; i<k_value; i++){
        co[0] = choice[i];

        polynomial * polyi = new polynomial(2, r, co);
        choice_poly.push_back(polyi);
    }

    polynomial product_poly = (*choice_poly[0]);
    for(int i=1; i<k_value; i++){
        product_poly = (product_poly) * (*choice_poly[i]);
    }

    polynomial result = product_poly.sort(coe_b);
    /*
    cout << result << endl;
    for(int i=0; i<k_value+1; i++){
        cout << "b" << i << "=" << result_params[i] << endl;
    }*/


    //calculating A
    for(int i=0; i< k_value; i++){
        mpz_powm(A[i], g, coe_a[i], n);
        mpz_powm_ui(tmp, h, (int) coe_b[i], n);
        mpz_mul(A[i], A[i], tmp);
    }

    //send to Server

    //receive c from Server
    for(int i=0; i< const_num; i++) {
        mpz_init(ci[i][0]);
        mpz_init(ci[i][1]);
        mpz_set_ui(ci[i][0], i);
        mpz_set_ui(ci[i][1], i+1);
    }

    for(int i=0; i< k_value; i++){
        mpz_poly(tmp, coe_a, k_value, choice[i]);
        mpz_powm(tmp, ci[choice[i]][0], tmp, p);
        mpz_invert(tmp, tmp, p);
        mpz_mul(result_label[i], ci[choice[i]][1], tmp);
    }

    gettimeofday(&t2,NULL);
    timeuse = (t2.tv_sec - t1.tv_sec) + (double)(t2.tv_usec - t1.tv_usec)/1000000.0;
    cout<<"OT takes "<< timeuse << " seconds." << endl;  //seconds


    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(g);
    mpz_clear(h);
    mpz_clear(n);

    mpz_clear(tmp);
    mpz_clear(tmp2);


    for(int i=0; i< const_num; i++){
        mpz_clear(ci[i][0]);
        mpz_clear(ci[i][1]);
    }
    for(int i=0; i< k_value; i++){
        mpz_clear(A[i]);
        mpz_clear(result_label[i]);
    }

    return 0;
}

int compare_tree(array <mpz_t, (const_num * (const_num-1) / 2 ) > * delta, int k_result_label[k_value]){

    mpz_t randseed, randnum;
    mpz_init(randseed);
    mpz_init(randnum);

    gmp_randstate_t state;
    gmp_randinit_mt(state);

    int index;

    //signed long tmp_delta[train_number*(train_number-1)/2], tmp_delta0[train_number*(train_number-1)/2], tmp_delta1[train_number*(train_number-1)/2];
    bool tmp_cmp_result[train_number][train_number];
    index = 0;

    for(int i=0; i<train_number; i++){
        for(int j=train_number-1; j>i; j--){
            //tmp_delta0[index] = i;//mpz_get_si(delta0[index]);
            //tmp_delta1[index] = j;//mpz_get_si(delta1[index]);
            //tmp_delta[index] = tmp_delta1[index] - tmp_delta0[index];

/*
            if(tmp_delta[index] > 0){
                tmp_cmp_result[i][j] = true;
                //cmp_result[j][i] = false;
            }else if(tmp_delta[index] == 0){
                tmp_cmp_result[i][j] = false;
                //cmp_result[j][i] = false;
            }else{
                tmp_cmp_result[i][j] = false;
                //cmp_result[j][i] = true;
            }
*/
            if(mpz_cmp_ui((*delta)[index], 0) > 0){
                tmp_cmp_result[i][j] = true;
                //cmp_result[j][i] = false;
            }else{
                tmp_cmp_result[i][j] = false;
                //cmp_result[j][i] = true;
            }
            index++;
        }
        //cout << i << endl;
    }

/*
    cout << "Delta0: " << endl;
    index = 0;
    for(int i=0; i<10; i++){
        for(int j=10-1; j>i; j--){
            cout << "[" << i << "][" << j << "]: " << tmp_delta0[index] << ": " << delta0[index] << "; ";
            index ++;
        }
        cout  << endl;
    }

    cout << "Delta1: " << endl;
    index = 0;
    for(int i=0; i<10; i++){
        for(int j=10-1; j>i; j--){
            cout << "[" << i << "][" << j << "]: " << tmp_delta1[index] << ": " << delta1[index] << "; ";
            index ++;
        }
        cout  << endl;
    }
*/
//test tmp rm.
/*
    bool cmp_result[10][10];

    index = 0;
    for(int i=0; i<10; i++){
        for(int j=10-1; j>i; j--){
            mpz_init(delta[index]);
            mpz_sub(delta[index], delta1[index], delta0[index]);
            if(mpz_cmp_ui(delta[index], 0) > 0){
                cmp_result[i][j] = true;
                //cmp_result[j][i] = false;
            }else if(mpz_cmp_ui(delta[index], 0) == 0){
                cmp_result[i][j] = false;
                //cmp_result[j][i] = false;
            }else{
                cmp_result[i][j] = false;
                //cmp_result[j][i] = true;
            }
            index++;
        }
        //cout << i << endl;
    }*/
/*
    index = 0;
    for(int i=0; i<10; i++){
        for(int j=10-1; j>i; j--){
            cout << "[" << i << "][" << j << "]: " << cmp_result[i][j] << "; ";
            index ++;
        }
        cout  << endl;
    }
*/



    int topk_result[k_value];
    int min=0, flag = 1;
    for (int k = 0; k < k_value; k++){
        //while(flag){
        for(int j=min+1; j<train_number; j++){
            if(tmp_cmp_result[min][j]){
                min = j;
                j=min+1;
                continue;
            }else{
                continue;
            }
        }
        //cout << i << endl;
        //}
        //flag = 1;
        topk_result[k] = min;
        for(int j=min+1; j<train_number; j++){
            tmp_cmp_result[min][j] = true;
            //cmp_result[j][min] = true;
        }
        for(int j=0; j<min; j++){
            //cmp_result[min][j] = false;
            tmp_cmp_result[j][min] = false;
        }

        min = 0;

    }

    for(int i=0; i<k_value; i++){
        cout << topk_result[i] << endl;
        k_result_label[i] = topk_result[i];
    }

    return 0;
}

int read_query(char * query, char const * filepath){
    mpz_t Q[attribute_number];
    mpz_t Q2;
    mpz_t random;
    unsigned int result_flag, sum_in = 0;

    char tmp[100];

    FILE *fp_in = NULL;
    fp_in = fopen(filepath, "r");


    mpz_t tmpNum;
    mpz_init(tmpNum);

    for (int t = 0; t < attribute_number-1; t++) { //read a train samples t to Xi

        result_flag = mpz_inp_raw(tmpNum, fp_in);
        if (result_flag == 0) {
            cerr << "Unable to read!" << endl;
            return -1;
        } else {
            sum_in += result_flag;
        }

        mpz_init(Q[t]);
        mpz_set(Q[t], tmpNum);
    }

    result_flag = mpz_inp_raw(tmpNum, fp_in);
    if (result_flag == 0) {
        cerr << "Unable to read!" << endl;
        return -1;
    } else {
        sum_in += result_flag;
    }
    mpz_init(Q2);
    mpz_set(Q2, tmpNum);


    result_flag = mpz_inp_raw(tmpNum, fp_in);
    if (result_flag == 0) {
        cerr << "Unable to read!" << endl;
        return -1;
    } else {
        sum_in += result_flag;
    }

    mpz_init(random);
    mpz_set(random, tmpNum);

    cout << "Result: Read " << sum_in << " bytes." << endl;
    fclose(fp_in);

    //*****
    const char * sp = "-";

    mpz_get_str(query, 36, Q[0]);

    for (int i = 1; i < attribute_number-1; i++) {
        strcat(query, sp);
        mpz_get_str(tmp, 36, Q[i]);
        strcat(query, tmp);
    }

    strcat(query, sp);
    mpz_get_str(tmp, 36, Q2);
    strcat(query, tmp);

    strcat(query, sp);
    mpz_get_str(tmp, 36, random);
    strcat(query, tmp);

    //cout << endl;
    //printf("Query messages: %s", query);

    return 0;
}

void * myThread(void *arg){
    int id = *(int *)arg;
    printf("current thread is :%d\n",id);

    struct timeval t1,t2;
    double timeuse;
    gettimeofday(&t1,NULL);


    ///////////////  SOCKET ///////////////////////
    int lfd0 = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in cli_addr0;
    memset(&cli_addr0,0,sizeof(cli_addr0));
    cli_addr0.sin_family = AF_INET;
    cli_addr0.sin_port = htons(0);
    if(bind(lfd0,(struct sockaddr*)&cli_addr0,sizeof(cli_addr0)))
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }
    struct sockaddr_in serv0;
    serv0.sin_family = AF_INET;
    serv0.sin_port = htons(server_port_0);
    inet_pton(AF_INET,"127.0.0.1",&(serv0.sin_addr.s_addr));
    if(connect(lfd0,(struct sockaddr*)&serv0,sizeof(serv0)))
    {
        printf("Can Not Connect To port!\n");
        exit(1);
    }
    ////////////////////another socket///////////////////////////

    int lfd1 = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in cli_addr1;
    memset(&cli_addr1,0,sizeof(cli_addr1));
    cli_addr1.sin_family = AF_INET;
    cli_addr1.sin_port = htons(0);
    if(bind(lfd1,(struct sockaddr*)&cli_addr1,sizeof(cli_addr1)))
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }
    struct sockaddr_in serv1;
    serv1.sin_family = AF_INET;
    serv1.sin_port = htons(server_port_1);
    inet_pton(AF_INET,"127.0.0.1",&(serv1.sin_addr.s_addr));
    if(connect(lfd1,(struct sockaddr*)&serv1,sizeof(serv1)))
    {
        printf("Can Not Connect To port!\n");
        exit(1);
    }

    ////////////////////////////////////////////////////////////
    char buf[1024] = {0};
    char * treebuf0 = new char[4*(train_number*(train_number-1)/2)+100];
    char *longbuf0 = new char[16*train_number+100] ;
    char * treebuf1 = new char[4*(train_number*(train_number-1)/2)+100];
    char * longbuf1 = new char[16*train_number+100];

    char query0[1024*(attribute_number+1+1)+1];
    char ot_buf0[16*(k_value)+1];
    char query1[16*(attribute_number+1+1)+1];
    char ot_buf1[16*(k_value)+1];
    char refresh_label[16+1];
    char refresh_return0[16+1];
    char refresh_return1[16+1];

    int i;

    //User:
    QueryEnc();

    //HC:
    mpz_t Q[attribute_number];
    mpz_t Q2;
    mpz_t random;
    unsigned int result_flag, sum_in = 0;

    char tmp[100];

    FILE *fp_in = NULL;

    mpz_t tmpNum;
    mpz_init(tmpNum);
    char query_path[1024];
    //sprintf(query_path, "client_query_%d.data", s);
    //fp_in = fopen(query_path, "r");

    for (int t = 0; t < attribute_number-1; t++) { //read a train samples t to Xi
        mpz_init(Q[t]);
        mpz_set(Q[t], tmpNum);
    }
    mpz_init(Q2);
    mpz_init(random);

    char tmp_str[100];

    for(int s=0; s<test_number; s++){
        sprintf(query_path, "client_query_%d.data", s);
        fp_in = fopen(query_path, "r");
        //read_query(query0, query_path);

        for (int t = 0; t < attribute_number-1; t++) { //read a train samples t to Xi

            result_flag = mpz_inp_raw(tmpNum, fp_in);
            if (result_flag == 0) {
                cerr << "Unable to read!" << endl;
                //return -1;
                exit(-1);
            } else {
                sum_in += result_flag;
            }
            mpz_set(Q[t], tmpNum);
        }

        result_flag = mpz_inp_raw(tmpNum, fp_in);
        if (result_flag == 0) {
            cerr << "Unable to read!" << endl;
            exit(-1);
        } else {
            sum_in += result_flag;
        }

        mpz_set(Q2, tmpNum);


        result_flag = mpz_inp_raw(tmpNum, fp_in);
        if (result_flag == 0) {
            cerr << "Unable to read!" << endl;
            exit(-1);
        } else {
            sum_in += result_flag;
        }


        mpz_set(random, tmpNum);

        cout << "Result: Read " << sum_in << " bytes." << endl;
        fclose(fp_in);

        //*****
        const char * sp = "-";

        mpz_get_str(query0, 36, Q[0]);

        for (int i = 1; i < attribute_number-1; i++) {
            strcat(query0, sp);
            mpz_get_str(tmp_str, 36, Q[i]);
            strcat(query0, tmp_str);
        }

        strcat(query0, sp);
        mpz_get_str(tmp_str, 36, Q2);
        strcat(query0, tmp_str);

        strcat(query0, sp);
        mpz_get_str(tmp_str, 36, random);
        strcat(query0, tmp_str);


        ///
    }


    //memset(query0, '1', 16*(attribute_number+1+1));
    //query0[16*(attribute_number+1+1)-1] = '\0';
    cout << "Totally: " << strlen(query0) << endl;

    write(lfd0,query0,300*(attribute_number+1+1));

    //memset(query1, '1', 16*(attribute_number+1+1));
    //query1[16*(attribute_number+1+1)-1] = '\0';
    write(lfd1,query0,300*(attribute_number+1+1));

    cout << " sent. " << endl;



    //HC:
    //Top-K Select
    long result_values[train_number+100];
    long min;
    long mins[k_value];
    //mpz_init(min);
    //mpz_set(min, result_values[0]);
    result_values[0] = min;

    for(int k=0; k<train_number; k++){
        result_values[k]=0;
    }

/*
    int len = read(lfd0, treebuf0, 4*(train_number*(train_number-1)/2));
    if(len == -1)
    {
        perror("read err");
        pthread_exit(NULL);
    }else if(len == 0)
    {
        printf("conncet close\n");
        close(lfd0);
    }else{
        treebuf0[len] = '\0';
        //printf("[1] Receive %d Bytes: %s---\n",len, treebuf0);
        printf("[1] Receive %d Bytes (delta) from server A.\n",len);
    }


    len = read(lfd1, treebuf1, 4*(train_number*(train_number-1)/2)+1024);
    if(len == -1)
    {
        perror("read err");
        pthread_exit(NULL);
    }else if(len == 0)
    {
        printf("conncet close\n");
        close(lfd1);
    }else{
        treebuf1[len] = '\0';
        //printf("[1] Receive %d Bytes: %s---\n",len, treebuf1);
        printf("[1] Receive %d Bytes (delta) from server B.\n",len);
    }
*/
    //FILE UPLOAD RECEIVE
    FILE *fp0, *fp1;
    bool flag;
    char  buff[MAXLINE+1];
    int  n, len;
    len = 0;

    if((fp0 = fopen("Compare_tree_0.data","ab") ) == NULL )
    {
        printf("File open error.\n");
        close(lfd0);
        exit(1);
    }
    flag = true;
    while(flag){
        n = read(lfd0, buff, MAXLINE);
        len += n;
        if(n == 0)
            break;
        /*if(strchr(buff, term_char) == NULL)//在a中查找b，如果不存在，
          printf("recv %d bytes\n", n);
        else//否则存在。
          printf("End of file\n");
          flag = false;*/
        if(n == MAXLINE){
            printf("recv %d bytes\n", n);
        }
        else{
            printf("recv %d bytes\n", n);
            //buff[n] = '\0';
            //printf("recv msg from Server 0: %s\n", buff[n-1], buff);

            if(buff[n-1]== '#'){
                flag = false;
                printf("End of file.\n");
            }else{
                printf("Not End.\n");
            }

            //printf("End of file\n");
            //sleep(1);
            //flag = false;
        }
        fwrite(buff, 1, n, fp0);
    }

    printf("File0 received.\n");
    printf("[1] Receive %d Bytes (delta) from server A.\n",len);

    //close(connfd);
    fclose(fp0);


    len = 0;
    if((fp1 = fopen("Compare_tree_1.data","ab") ) == NULL )
    {
        printf("File open error.\n");
        close(lfd0);
        exit(1);
    }
    flag = true;
    while(flag){
        n = read(lfd1, buff, MAXLINE);
        len += n;
        if(n == 0)
            break;
        /*if(strchr(buff, term_char) == NULL)//在a中查找b，如果不存在，
          printf("recv %d bytes\n", n);
        else//否则存在。
          printf("End of file\n");
          flag = false;*/
        if(n == MAXLINE){
            printf("recv %d bytes\n", n);
        }
        else{
            printf("recv %d bytes\n", n);
            //buff[n] = '\0';
            /*
            printf("recv msg from Server 1: %s\n", buff);

            if(strchr(buff, '#')){
                flag = false;
                printf("End of file.\n");
            }*/
            if(buff[n-1]== '#'){
                flag = false;
                printf("End of file.\n");
            }else{
                printf("Not End.\n");
            }

            //printf("End of file\n");
            //sleep(1);
            //flag = false;
        }
        fwrite(buff, 1, n, fp1);
    }

    printf("File1 received.\n");
    printf("[1] Receive %d Bytes (delta) from server B.\n",len);
    //close(connfd);
    fclose(fp1);

    ///////////////////////
    array <mpz_t, ( const_num * (const_num-1) / 2 ) > * delta = new array <mpz_t, ( const_num * ( const_num-1) / 2 ) >();

    //cout << "Tree0: " << endl;
    const char * output_file1_name = "Compare_tree_0.data";
    FILE *fp_in0 = NULL;
    fp_in0 = fopen(output_file1_name, "r");

    const char * output_file2_name = "Compare_tree_1.data";
    FILE * fp_in1 = fopen(output_file2_name, "r");

    mpz_t tmpNum0, tmpNum1;
    //mpz_init(tmpNum);
    mpz_init(tmpNum0);
    mpz_init(tmpNum1);

    //unsigned int result_flag, sum_in = 0;
    int index = 0;

    for(int i=0; i<const_num; i++){
        for(int j=const_num-1; j>i; j--){

            result_flag = mpz_inp_raw(tmpNum0, fp_in0);
            if(result_flag == 0){
                cerr << "Unable to read!" << endl;
                exit(-1);
            }else{
                sum_in += result_flag;
            }

            result_flag = mpz_inp_raw(tmpNum1, fp_in1);
            if(result_flag == 0){
                cerr << "Unable to read!" << endl;
                exit(-1);
            }else{
                sum_in += result_flag;
            }

            mpz_init((*delta)[index]);
            mpz_sub((*delta)[index], tmpNum1, tmpNum0);

            //if(index < 10) cout << tmpNum << "; ";
            index++;
        }

    }
    cout << endl;
    /*
    char end_char[2];
    end_char[0] = '#';
    end_char[1] = '\0';
    write(fp_out, end_char, 1);*/
    //fputc('#', fp_out);
    fclose(fp_in0);
    fclose(fp_in1);


/*
    result_flag = 0, sum_in = 0;
    index = 0;

    cout << "Tree1: " << endl;
    for(int i=0; i<const_num; i++){
        for(int j=const_num-1; j>i; j--){





            if(index < 10) cout << tmpNum << "; ";
            index++;
        }

    }
    cout << endl;
    /*
    char end_char[2];
    end_char[0] = '#';
    end_char[1] = '\0';
    write(fp_out, end_char, 1);*/
    //fputc('#', fp_out);
    //fclose(fp_in1);

    //////

/*
    //sorting...
    for(int j=0; j<k_value; j++){
        //long min;
        //mpz_init(min);
        int mini = 0;
        while((result_values[mini] == -1) && mini < train_number) mini++;
        min = result_values[mini];

        for(int i=0;i<train_number;i++){
            if((result_values[i]== -1)) continue;
            if((result_values[i]< min)){
                (min= result_values[i]);
                (result_values[i]= -1);
                continue;
            }
        }
        (mins[j]= min);
    }
*/

    int result_labels[k_value];
    compare_tree(delta, result_labels);
    char select_label[16];
    char select_labels[k_value * (16+1) + 1];
    const char * sp = "-";
    //itoa((int)result_values[0], select_labels, 10);
    sprintf(select_labels, "%ld", result_labels[0]);
    for(int i=1; i<k_value; i++){
        sprintf(select_label, "%ld", result_labels[i]);
        strcat(select_labels, sp);
        strcat(select_labels, select_label);
    }
    printf("Result labels: %s", select_labels);

    //HC:
    //k-out-of-n Oblivious transfer
    oblivious_transfer();

    mpz_t k_labels_0[k_value];
    mpz_t k_labels_1[k_value];

    //memset(ot_buf0, '2', (16+1)*(k_value));
    //ot_buf0[(16+1)*(k_value)-1] = '\0';
    //select_labels[16*(k_value)-1] = '\0';
    write(lfd0,select_labels,(16+1)*(k_value));

    len = read(lfd0, longbuf0, 16*train_number);
    if(len == -1)
    {
        perror("read err");
        pthread_exit(NULL);
    }else if(len == 0)
    {
        printf("conncet close!\n");
        close(lfd0);
    }else{
        //longbuf0[len] = '\0';
        //printf("[2] Receive %d Bytes: %s---\n",len, longbuf0);
        printf("[2] Receive %d Bytes (oblivious transfer) from server A.\n",len);


        const char sep[2] = "-";
        char * token;


        int lab_ind = 1;

        /* 获取第一个子字符串 */
        token = strtok(longbuf0, sep);
        if(token != NULL){
            mpz_init(k_labels_0[0]);
            mpz_set_str(k_labels_0[0], token, 10);
        }else{
            cout << "Sep Error!" << endl;
            cout << longbuf0 << endl;
        }


        /* 继续获取其他的子字符串 */
        while( token != NULL ) {
            if(lab_ind >= k_value){
                cout << "Too many selections!" << endl;
                break;
            }
            cout  << "Extract " << lab_ind << " labels..." << endl;
            printf( "%s\n", token );
            token = strtok(NULL, sep);
            mpz_init(k_labels_0[lab_ind]);
            mpz_set_str(k_labels_0[lab_ind], token, 10);
            lab_ind++;
        }
        //cout << "Show selected labels: " << endl;
        //for(int j=0; j<k_value; j++){
        //    cout << j << ": " << k_labels[j] ;
        //}

    }

    //memset(ot_buf1, '2', (16+1)*(k_value));
    //ot_buf1[(16+1)*(k_value)-1] = '\0';
    write(lfd1,select_labels,(16+1)*(k_value));

    len = read(lfd1, longbuf1, 16*train_number);
    if(len == -1)
    {
        perror("read err");
        pthread_exit(NULL);
    }else if(len == 0)
    {
        printf("conncet close!\n");
        close(lfd0);
    }else{
        //longbuf1[len] = '\0';
        //printf("Receive2: %s---\n",longbuf1);
        printf("[2] Receive %d Bytes (oblivious transfer) from server B.\n",len);

        const char sep[2] = "-";
        char * token;
        int lab_ind = 1;

        /* 获取第一个子字符串 */
        token = strtok(longbuf1, sep);
        if(token != NULL){
            mpz_init(k_labels_1[0]);
            mpz_set_str(k_labels_1[0], token, 10);
        }else{
            cout << "Sep Error!" << endl;
            cout << longbuf1 << endl;
        }


        /* 继续获取其他的子字符串 */
        while( token != NULL ) {
            if(lab_ind >= k_value){
                cout << "Too many selections!" << endl;
                break;
            }
            cout  << "Extract " << lab_ind << " labels..." << endl;
            printf( "%s\n", token );
            token = strtok(NULL, sep);
            mpz_init(k_labels_1[lab_ind]);
            mpz_set_str(k_labels_1[lab_ind], token, 10);
            lab_ind++;
        }
        //cout << "Show selected labels: " << endl;
        //for(int j=0; j<k_value; j++){
        //    cout << j << ": " << k_labels[j] ;
        //}

    }

    cout << "Show selected labels: " << endl;
    ///mpz_t k_labels[k_value];
    mpz_t tmplabel;
    mpz_init(tmplabel);
    int k_labels[k_value];
    for(int j=0; j<k_value; j++){
        //mpz_init(k_labels[j]);
        mpz_sub(tmplabel, k_labels_1[j], k_labels_0[j]);
        k_labels[j] = mpz_get_ui(tmplabel);
        cout << j << ": " << k_labels[j] << ", " ;
    }
    cout << endl;

    sort(k_labels, k_labels+k_value);
    cout << "Predict label: ";
    switch(k_labels[k_value/2]){
        case 0:
            cout << "setosa" << endl;
            break;
        case 100:
            cout << "versicolor" << endl;
            break;
        case 200:
            cout << "virginica" << endl;
            break;
        default:
            cout << "Error!" << endl;
            break;
    }



    /////////////////////////////////////////
    mpz_t final_label, returned_label0, returned_label1;//, final_label1;
    mpz_init(final_label);
    mpz_init(returned_label0);
    mpz_init(returned_label1);
    //mpz_init(final_label1);

    //char select_labels[k_value * (16+1) + 1];
    //const char * sp = "-";
    //itoa((int)result_values[0], select_labels, 10);
    sprintf(refresh_label, "%ld", final_label);

    //memset(refresh_label, '3', 16+1);
    //refresh_label[16] = '\0';
    write(lfd0,refresh_label,16);

    len = read(lfd0, refresh_return0, 16);
    if(len == -1)
    {
        perror("read err");
        pthread_exit(NULL);
    }else if(len == 0)
    {
        printf("conncet close!\n");
        close(lfd0);
    }else{
        //refresh_return0[len] = '\0';
        mpz_set_str(returned_label0, refresh_return0, 10);
        //printf("[2] Receive %d Bytes: %s---\n",len, longbuf0);
        printf("[3] Receive %d Bytes (refresh) from server A.\n",len);
    }

    //sprintf(refresh_label, "%ld", final_label0);
    //memset(ot_buf1, '2', 16*(train_number));
    //ot_buf1[16*(train_number)-1] = '\0';
    write(lfd1,refresh_label,16);

    len = read(lfd1, refresh_return1, 16);
    if(len == -1)
    {
        perror("read err");
        pthread_exit(NULL);
    }else if(len == 0)
    {
        printf("conncet close!\n");
        close(lfd0);
    }else{
        //refresh_return1[len] = '\0';
        mpz_set_str(returned_label1, refresh_return1, 10);
        //printf("Receive2: %s---\n",longbuf1);
        printf("[3] Receive %d Bytes (oblivious transfer) from server B.\n",len);
    }

    mpz_sub(returned_label1, returned_label1, returned_label0);
    cout << "Retrieve final label..." << endl;

    /////////////////////////////////////////



    //User:
    //recover the labels
    for(int i=0;i<k_value;i++){
        (result_values[i]= result_values[i]+ 11);
    }

    gettimeofday(&t2,NULL);
    timeuse = (t2.tv_sec - t1.tv_sec) + (double)(t2.tv_usec - t1.tv_usec)/1000000.0;
    cout<<"Online execution for one queries takes "<< timeuse << " seconds." << endl;  //seconds

    delete [] treebuf0;// = new char[16*(train_number*(train_number-1)/2)+100];
    delete [] longbuf0;// = new char[16*train_number+100] ;
    delete []  treebuf1;// = new char[16*(train_number*(train_number-1)/2)+100];
    delete []  longbuf1;// = new char[16*train_number+100];




    close(lfd0);
    //close(lfd1);
    /////////////////////////////////
    //printf("Receive 2: %s---\n",longbuf2);
}



void * myThread_ERDKNN(void *arg){
    int id = *(int *)arg;
    printf("current thread is :%d\n",id);


    ///////////////  SOCKET ///////////////////////
    int lfd0 = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in cli_addr0;
    memset(&cli_addr0,0,sizeof(cli_addr0));
    cli_addr0.sin_family = AF_INET;
    cli_addr0.sin_port = htons(0);
    if(bind(lfd0,(struct sockaddr*)&cli_addr0,sizeof(cli_addr0)))
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }
    struct sockaddr_in serv0;
    serv0.sin_family = AF_INET;
    serv0.sin_port = htons(server_port_0);
    inet_pton(AF_INET,"127.0.0.1",&(serv0.sin_addr.s_addr));
    if(connect(lfd0,(struct sockaddr*)&serv0,sizeof(serv0)))
    {
        printf("Can Not Connect To port!\n");
        exit(1);
    }
    ////////////////////another socket///////////////////////////

    int lfd1 = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in cli_addr1;
    memset(&cli_addr1,0,sizeof(cli_addr1));
    cli_addr1.sin_family = AF_INET;
    cli_addr1.sin_port = htons(0);
    if(bind(lfd1,(struct sockaddr*)&cli_addr1,sizeof(cli_addr1)))
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }
    struct sockaddr_in serv1;
    serv1.sin_family = AF_INET;
    serv1.sin_port = htons(server_port_1);
    inet_pton(AF_INET,"127.0.0.1",&(serv1.sin_addr.s_addr));
    if(connect(lfd1,(struct sockaddr*)&serv1,sizeof(serv1)))
    {
        printf("Can Not Connect To port!\n");
        exit(1);
    }

    ////////////////////////////////////////////////////////////
    char buf[1024] = {0};
    char treebuf0[16+1];
    char treebuf1[16+1];

    char query0[16*(attribute_number)+1];
    int i;

    //User:
    QueryEnc_ERDKNN();

    memset(query0, '1', 16*(attribute_number)+1);
    query0[16*(attribute_number)] = '\0';
    write(lfd0,query0,16*(attribute_number)+1);


    int len = read(lfd0, treebuf0, 16);
    if(len == -1)
    {
        perror("read err");
        pthread_exit(NULL);
    }else if(len == 0)
    {
        printf("conncet close\n");
        close(lfd0);
    }else{
        treebuf0[len] = '\0';
        printf("[1] Receive %d Bytes: %s---\n",len, treebuf0);
    }

    len = read(lfd1, treebuf1, 16);
    if(len == -1)
    {
        perror("read err");
        pthread_exit(NULL);
    }else if(len == 0)
    {
        printf("conncet close\n");
        close(lfd1);
    }else{
        treebuf1[len] = '\0';
        printf("[1] Receive %d Bytes: %s---\n",len, treebuf1);
    }

    mpz_t result, r_q, gamma_q;
    mpz_init(r_q);
    mpz_init(gamma_q);
    mpz_init(result);
    mpz_set_ui(r_q, 1999);
    mpz_set_ui(gamma_q, 2999);
    mpz_sub(result, gamma_q, r_q);

    close(lfd0);
    close(lfd1);
    /////////////////////////////////
    //printf("Receive 2: %s---\n",longbuf2);
}


void * myThread_RPKNN(void *arg){
    int id = *(int *)arg;
    printf("current thread is :%d\n",id);


    ///////////////  SOCKET ///////////////////////
    int lfd0 = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in cli_addr0;
    memset(&cli_addr0,0,sizeof(cli_addr0));
    cli_addr0.sin_family = AF_INET;
    cli_addr0.sin_port = htons(0);
    if(bind(lfd0,(struct sockaddr*)&cli_addr0,sizeof(cli_addr0)))
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }
    struct sockaddr_in serv0;
    serv0.sin_family = AF_INET;
    serv0.sin_port = htons(server_port_0);
    inet_pton(AF_INET,"127.0.0.1",&(serv0.sin_addr.s_addr));
    if(connect(lfd0,(struct sockaddr*)&serv0,sizeof(serv0)))
    {
        printf("Can Not Connect To port!\n");
        exit(1);
    }
    ////////////////////another socket///////////////////////////


    int lfd1 = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in cli_addr1;
    memset(&cli_addr1,0,sizeof(cli_addr1));
    cli_addr1.sin_family = AF_INET;
    cli_addr1.sin_port = htons(0);
    if(bind(lfd1,(struct sockaddr*)&cli_addr1,sizeof(cli_addr1)))
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }
    struct sockaddr_in serv1;
    serv1.sin_family = AF_INET;
    serv1.sin_port = htons(server_port_1);
    inet_pton(AF_INET,"127.0.0.1",&(serv1.sin_addr.s_addr));
    if(connect(lfd1,(struct sockaddr*)&serv1,sizeof(serv1)))
    {
        printf("Can Not Connect To port!\n");
        exit(1);
    }

    ////////////////////////////////////////////////////////////
    char buf[1024] = {0};
    char treebuf0[16*3*k_value+1];

    char query0[16*2+1];
    char query1[16+1];
    int i;

    //User:
    cout << "Key Gen..." << endl;
    int result;
    paillier_public_key pub;
    paillier_private_key_v priv;

    //initialize keys
    cout << "System keygen: " << endl;
    paillier_public_init(&pub);
    paillier_private_init_v(&priv);
    result = paillier_keygen_variant(&pub, &priv, PaillierKeySize/2);

    private_key rsa_ku;
    public_key rsa_kp;
    RSAKeyGen(&rsa_ku, &rsa_kp);

    glo_pub = &pub;
    glo_rsa_pub = &rsa_kp;
    QueryEnc_RPKNN();

    memset(query0, '1', 16*2+1);
    query0[16*2] = '\0';
    write(lfd0,query0,16*2+1);

    memset(query1, '1', 16+1);
    query1[16] = '\0';
    write(lfd1,query1,16+1);


    int len = read(lfd0, treebuf0, 16*3*k_value+1);
    if(len == -1)
    {
        perror("read err");
        pthread_exit(NULL);
    }else if(len == 0)
    {
        printf("conncet close\n");
        close(lfd0);
    }else{
        treebuf0[len] = '\0';
        printf("[1] Receive %d Bytes: %s---\n",len, treebuf0);
    }

    mpz_t label, r_q, gamma_q;
    mpz_init(r_q);
    mpz_init(gamma_q);
    mpz_init(label);
    mpz_set_ui(r_q, 1999);
    mpz_set_ui(gamma_q, 2999);

    cout << "Decrypt result." << endl;
    for(int i=0; i<k_value; i++){
        SEEnc(label, r_q, gamma_q);//INFACT SEDEC
    }

    close(lfd0);
    close(lfd1);
    /////////////////////////////////
    //printf("Receive 2: %s---\n",longbuf2);
}

int main(int argc, char *argv[])
{
    //oblivious_transfer();
    //OT_test();
    //return 0;

    if(argc != 3)
    {
        printf("exe: app port\n");
        exit(1);
    }
    server_port_0 = atoi(argv[1]);
    server_port_1 = atoi(argv[2]);

    //server_port_0 = 10240;

    struct timeval t1,t2;
    double timeuse;
    gettimeofday(&t1,NULL);

    pthread_t thread[N];
    int id[N],i;
    for(i=0;i<N;i++){
        id[i] = i;
        pthread_create(&thread[i], NULL, myThread, &id[i]);
    }

    for(i=0;i<N;i++){
        pthread_join(thread[i],NULL);
    }

    gettimeofday(&t2,NULL);
    timeuse = (t2.tv_sec - t1.tv_sec) + (double)(t2.tv_usec - t1.tv_usec)/1000000.0;
    cout<<"Online execution for " << N << " queries takes "<< timeuse << " seconds." << endl;  //seconds

    return 0;
}


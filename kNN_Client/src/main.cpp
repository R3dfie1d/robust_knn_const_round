#include <iostream>
#include <vector>
#include <gmpxx.h>
#include <cmath>

#include "KNN_single.h"
#include "KNN_client.h"
#include "KNN_AC.h"
#include "readData.h"
#include "CSV.h"

using namespace std;

int NUM_IMAGES = 10240;

bool cmpt(node a,node b)
{
    return a.dis<b.dis;
}

template <typename T>
void div_m2c(vector<vector<T> > & Mat2,vector<vector<T> > & Mat1,\
double constNum, int IMAX, int JMAX){

    //Mat2.clear();
    vector<vector<T> > tempm;
    for(int i = 0; i < IMAX; ++i){
        vector<T> tempv;
        for(int k = 0; k < JMAX; ++k){

            tempv.push_back( (Mat1[i][k] / constNum));

        }
        //Mat2[i].clear();
        tempm.push_back(tempv);
    }
    Mat2.clear();
    for(int i = 0; i < IMAX; ++i){
        Mat2.push_back(tempm[i]);
    }
}

template <typename T>
void div_v2c(vector<T> &Vec2,vector<T> &Vec1,\
double constNum, int IMAX){

    vector<T> tempv;
//cout << "Who are you!"<< constNum << endl;
    for(int i = 0; i < IMAX; ++i){
        //if(Vec1[i] != 0) cout << Vec1[i]<< " " ;
        tempv.push_back((T) (Vec1[i] / constNum));
        //if(tempv[i] != 0) cout << tempv[i]<< " " ;
        //cout << Vec1[i]<<" ";
    }
    Vec2.clear();
    for(int i = 0; i < IMAX; ++i){
        Vec2.push_back(tempv[i]);
        //if(tempv[i] != 0) cout << tempv[i]<< " " ;
    }

}

template <typename T>
void mul_v2c(vector<T> &Vec2,vector<T> &Vec1,\
double constNum, int IMAX){

    vector<T> tempv;
//cout << "Who are you!"<< constNum << endl;
    for(int i = 0; i < IMAX; ++i){
        //if(Vec1[i] != 0) cout << Vec1[i]<< " " ;
        tempv.push_back((T) (Vec1[i] * constNum));
        //if(tempv[i] != 0) cout << tempv[i]<< " " ;
        //cout << Vec1[i]<<" ";
    }
    Vec2.clear();
    for(int i = 0; i < IMAX; ++i){
        Vec2.push_back(tempv[i]);
        //if(tempv[i] != 0) cout << tempv[i]<< " " ;
    }

}
template <typename T>
void v_expand(vector<T> &Vec1,\
double constNum, int IMAX){
    for(int i = 0; i < IMAX; ++i){
        Vec1[i] = (T)((int) (Vec1[i] * constNum));
    }

}

template <typename T>
void m_expand(vector<vector<T> > & Mat1,\
double constNum, int IMAX, int JMAX){
    for(int i = 0; i < IMAX; ++i){
        for(int k = 0; k < JMAX; ++k){
            Mat1[i][k] = (T)((int) (Mat1[i][k] * constNum));
        }
    }

}

int SecureCmp(mpz_t a0, mpz_t a1, mpz_t b0, mpz_t b1){
    int flag = 0;
    mpz_t a,b,result;
    mpz_init(a);
    mpz_init(b);
    mpz_init(result);


    mpz_add(a, a0, a1);
    mpz_add(b, b0, b1);
    mpz_add(result, a, b);


    if(mpz_cmp_ui(result, 0) < 0 ){
        return 1;
    }
    else if (mpz_cmp_ui(result, 0) == 0 ){
        flag = 0;
    }
    else{
        flag = -1;
    }

    mpz_clear(result);
    return flag;
}


int SecureSort(mpz_t shared_distances0[20], mpz_t shared_distances1[20], int sortResult[20]){


    //share_dis0[20], share_dis1[20],
    mpz_t r, result0[20], result1[20];
    mpz_init(r);

    int flag=0, ind[20];





    //Server 0: d10~dn0
    //Server 1: d11~dn1

    //S0: cal d10-d20, d10-d30, ..., d10-dn0, d20-d30, ..., ..., dn-10-dn0 : n*(n-1)/2
    //S1: cal d11-d21, ... , dn-11-dn1 .


    //HC: recover d1-d2, d1-d3, ..., dn-1 - dn : n*n-1 / 2
    //HC: find the largest one, repeat k time.
    //HC: first find who is greater than d1, then find who is greater than it, until it cant find greater one.





    for(int i=0; i< 20; i++){
        mpz_init(result0[i]);
        mpz_init(result1[i]);

        mpz_mul(result0[i], shared_distances0[i], r);
        mpz_mul(result1[i], shared_distances1[i], r);

        ind[i] = i;
    }

    //HC:

    int top[k_value], cmp[20][20];
    int i=0, j=0, min = 0;
    for(i=0; i<20; i++){
        for(j=0; j<20; j++){
            if(i = j) cmp[i][j] = 0;
            else
            cmp[i][j] = SecureCmp(result0[i], result1[i], result0[j], result1[j]);
        }
    }


    for(int ii=0; ii<k_value; ii++){
        while(1){
            for(j=0; j<20; j++){
                if(cmp[i][j] < 0){
                    min = j;
                    break;
                }
                //flag = SecureCmp(result0[i], result1[i], result0[j], result1[j]);
            }
            if(j = 20) break;
            i = min;
            min = -1;
        }
        // get i as the greatest one
        top[ii] = i;
        // loop to set cmp[i][j] > 0 for all j
        for(j=0; j<20; j++){
            cmp[i][j] = 1;
        }
    }







    //
    return 0;
}

int client_enc_input(mpz_t Q[], mpz_t Q2, int q[], SetupPhase * setup){
    int result, sum=0;
    mpz_t tmp;
    mpz_init(tmp);
    for (int t = 0; t < attribute_number; t++) { //read a train samples t to Xi
        mpz_set_ui(tmp, q[t]);
        result = paillier_encrypt(Q[t], tmp, setup->hpk.pk);
        sum += q[t] * q[t];
        //mpz_set_ui(Q[t], t);
    }
    mpz_set_ui(tmp, sum);
    result = paillier_encrypt(Q2, tmp, setup->hpk.pk);
    //if(server_id == 0) mpz_set_ui(Q2, 33333);
    //else mpz_add_ui(Q2, tmp, 33333);

    mpz_clear(tmp);

    return 0;
}

int client_write_input(mpz_t Q[], mpz_t Q2, mpz_t random, char * filepath){
    unsigned int result_flag, sum_out = 0;

    FILE * fp_out = NULL;
    fp_out = fopen(filepath, "w+");

    for (int t = 0; t < attribute_number-1; t++) { //read a train samples t to Xi

        result_flag = mpz_out_raw(fp_out, Q[t]);
        if(result_flag == 0){
            cerr << "Unable to write!" << endl;
            return -1;
        }else{
            sum_out += result_flag;
        }
    }

    result_flag = mpz_out_raw(fp_out, Q2);
    if(result_flag == 0){
        cerr << "Unable to write!" << endl;
        return -1;
    }else{
        sum_out += result_flag;
    }

    result_flag = mpz_out_raw(fp_out, random);
    if(result_flag == 0){
        cerr << "Unable to write!" << endl;
        return -1;
    }else{
        sum_out += result_flag;
    }

    cout << "Result: Write " << sum_out << " bytes." << endl;
    fclose(fp_out);

    return 0;
}

int server_read_input(mpz_t Q[], mpz_t Q2, char * filepath){
    unsigned int result_flag, sum_in = 0;

    FILE *fp_in = NULL;
    fp_in = fopen(filepath, "r");


    mpz_t tmpNum;
    mpz_init(tmpNum);

    for (int t = 0; t < attribute_number; t++) { //read a train samples t to Xi



        result_flag = mpz_inp_raw(tmpNum, fp_in);
        if (result_flag == 0) {
            cerr << "Unable to read!" << endl;
            return -1;
        } else {
            sum_in += result_flag;
        }

        mpz_set(Q[t], tmpNum);
    }

    result_flag = mpz_inp_raw(tmpNum, fp_in);
    if (result_flag == 0) {
        cerr << "Unable to read!" << endl;
        return -1;
    } else {
        sum_in += result_flag;
    }

    mpz_set(Q2, tmpNum);

    cout << "Result: Read " << sum_in << " bytes." << endl;
    fclose(fp_in);

    return 0;
}

int knn_DO(){
    cout << "========" << endl;
    cout << "Preprocessing" << endl;
    cout << "========" << endl;
    char const * KeyFilePath = (char*) "KeyFilePath.data";
    SetupPhase * setup = new SetupPhase(KeyFilePath);

    //check:
    /*
    cout << "Keysize: " << setup->KeySize << endl;
    cout << "Check Ds of hpk: " << endl;
    //cout << "> " << setup->hpk.Ds << " <" <<endl;

    //cout << "Ds:***" << endl;
    for (int j = 0; j < setup->KeySize; j++) {
        cout << setup->hpk.Ds[j] << ", ";
    }
    cout << "================================" << endl;*/

    MyTime * ktime = new MyTime();



    char const * file_path_his = (char *) "history_record.log";
    string message;


    std::cout << "Check setup params:" << std::endl;
    std::cout << setup->mof11.sharex << ", " << setup->hek0.ds[3] << ", " << setup->hpk.pk->n << ", " << 0 << ", " << (setup->KeySize) << ", " << std::endl;
    std::cout << setup->mof12.sharex << ", " << setup->hek1.ds[3] << ", " << setup->hpk.pk->n << ", " << 1 << ", " << (setup->KeySize) << ", " << std::endl;



    vector<vector<long> > training_data;
    vector<long> training_labels;
    //vector<vector<double> > X_out;
    //vector<double> Y_out;

    TrainingParams params;

    int n_;
    vector<long> testing_labels;
    vector<vector<long> > testing_data;
    //vector<vector<double> > testX;
    //vector<double> testY;


    //////////////////////////////////////////////
/*
    cout << "Read training data..." << endl;
    read_MNIST_data(true, training_data, params.n, params.d, NUM_IMAGES);
    cout << "Read training label..." << endl;
    read_MNIST_labels(true, training_labels, NUM_IMAGES);
*/


    //cout << "Read testing data..." << endl;
    //read_MNIST_data(false, testing_data, n_, params.d, NUM_IMAGES);
    //cout << "Read testing label..." << endl;
    //read_MNIST_labels(false, testing_labels, NUM_IMAGES);


    //////////////////////////////////////////////
    //cout << "Read data..." << endl;
    //read_LIVER_data_label(true, training_data, training_labels, testing_data, testing_labels, params.n, params.d, NUM_IMAGES);

    ///////////////////////////////////////////////
    //cout << "Read data..." << endl;
    char file_path[1024];
    //sprintf(file_path, "testTrain10.csv");
    sprintf(file_path, "iris.csv");
    read_csv_data_label(true, training_data, training_labels, testing_data, testing_labels, file_path);
    cout << "Training data" << endl;
    for(int i=0;i<train_number;i++){
        if(i < 5 or i == train_number-1){
            if(i == train_number-1) cout << "   ...........   " << endl;
            for(int j=0;j<attribute_number-1;j++){
                cout << training_data[i][j] << ", ";
            }
            cout << training_labels[i];
            cout << endl;
        }
    }

    cout << "Testing data" << endl;
    for(int i=0;i<test_number;i++){
        if(i < 5 or i == test_number-1){
            if(i == test_number-1) cout << "   ...........   " << endl;
            for(int j=0;j<attribute_number-1;j++){
                cout << testing_data[i][j] << ", ";
            }
            cout << testing_labels[i];
            cout << endl;
        }
    }
    //////////////////////////////////////////////

    cout << "Reading Complete." << endl;



    cout << "=======" << endl;
    cout << "Data Owner Encrypting" << endl;
    cout << "=======" << endl;

    message = "!!@@!Protocol formally begin!@@!!";
    ktime->TimeRecorded(message, file_path_his);

    KNN_client knn_c(&training_data, &training_labels, params,
                      setup, ktime);

    //for check only
    //training_data[100][6] = 1;
    //for(int ki=0; ki< 20; ki++){
    //    std::cout << training_data[20][ki] << ", " ;
    //}
    std::cout  << std::endl;
    //return 0;


    message = "DO finish encrypting";
    ktime->TimeRecorded(message, file_path_his, 0);



    //KNN_AC knn_ac(&testing_data, &testing_labels, params,
    //              &test_Xi1, &test_Xi2, setup, ktime);
    //message = "AC finish encrypting";
    //ktime->TimeRecorded(message, file_path_his, 2);
    //&setup->mof11, &setup->mof12, &setup->hpk, &setup->hek0, &setup->hek1,


    //std::vector<hss_input_p> Xw1_test; //input
    //std::vector<hss_input_p> Xw2_test;
    //std::vector<std::vector<hss_input_p> > Xi1_test; //input
    //std::vector<std::vector<hss_input_p> > Xi2_test;

    //mpz_t Yw1_test[train_number];//labels that are subtractively shared
    //mpz_t Yw2_test[train_number];

    //char * const DO_file_name_out_0 = (char*) "DO_HSS_Input_Data_0.data";
    //char * const DO_file_name_out_1 = (char*) "DO_HSS_Input_Data_1.data";

    char * const C_file_name_out_0 = (char*) "C_HSS_Input_Data_0.data";
    char * const C_file_name_out_1 = (char*) "C_HSS_Input_Data_1.data";

    //for(int test_i; test_i < 10; test_i++) cout << Xi1[0][test_i].encx << endl;

    //DO_write_input(&Xi1, Yw1, 0, DO_file_name_out_0);
    //DO_write_input(&Xi2, Yw2, 1, DO_file_name_out_1);

    //client_write_input(&test_Xi1, 0, C_file_name_out_0);
    //client_write_input(&test_Xi2, 1, C_file_name_out_1);
    //int preqi[attribute_number] = {2,	2628,	3631,	2387,	595,	4630,	4980,	4027,	1737,	3020};
    int preqi[attribute_number] = {0};
    mpz_t preQ[attribute_number];
    mpz_t preQ2;
    mpz_t result_values[train_number+100];

    for (int t = 0; t < attribute_number; t++) { //read a train samples t to Xi
        //preqi[t] = testing_data[0][t];
        mpz_init(preQ[t]);
        //mpz_set_ui(Q[t], t);
    }
    mpz_init(preQ2);
    //mpz_set_ui(Q2, 1000);
    /*
    for(int k=0; k<train_number; k++){
        mpz_init(result_values[k]);
        mpz_set_ui(result_values[k], 0);
    }

     */

    mpz_t random_for_answer;
    mpz_init(random_for_answer);

    gmp_randstate_t rstate;
    gmp_randinit_mt(rstate);
    cout << "* Values of query: " << endl;

    char query_path[100];



    for(int s=0; s<test_number; s++){
        mpz_urandomm(random_for_answer, rstate, setup->hpk.pk->n);
        for (int t = 0; t < attribute_number; t++) { //read a train samples t to Xi
            preqi[t] = testing_data[s][t];
        }
        client_enc_input(preQ, preQ2, preqi, setup);
        sprintf(query_path, "client_query_%d.data", s);
        client_write_input(preQ, preQ2, random_for_answer, query_path);
    }

    /*
    for (int j = 0; j < attribute_number; j++) {
        cout << preQ[j] << "; ";
    }
    cout << endl;
    cout << preQ2 << endl;
    */


    ktime->Time_report();
    return 0;
}

int Write_Train_Result(char * const file_name_recover_0, node_paillier (*qX1)[train_number+100]){
    unsigned int result_flag, sum_out = 0;

    FILE * fp_out = NULL;
    fp_out = fopen(file_name_recover_0, "w+");


    for (int i = 0; i < train_number; i++)  //for each train sample
    {
        
        result_flag = mpz_out_raw(fp_out, (*qX1)[i].labels);
        if(result_flag == 0){
            cerr << "Unable to write!" << endl;
            return -1;
        }else{
            sum_out += result_flag;
        }
        
        result_flag = mpz_out_raw(fp_out, (*qX1)[i].dis);
        if(result_flag == 0){
            cerr << "Unable to write!" << endl;
            return -1;
        }else{
            sum_out += result_flag;
        }

    }

    cout << "Result: Write " << sum_out << " bytes." << endl;
    fclose(fp_out);

    return 0;

}


int server_result_input(node_paillier (*input_qX1)[train_number+100], int server_id, char * output_file_name){
    unsigned int result_flag, sum_in = 0;
    FILE *fp_in = NULL;

    cout << "Output of server " << server_id << endl;

    fp_in = fopen(output_file_name, "r");

    int result_num;
    for(int i=0; i<train_number; i++) {
        //mpzDataIn >> bits2;
        //mpz_init(data[i]);
        //mpzDataIn.read(bits2, sizeof(mpz_t));
        //result_num = mpz_set_str(colors[i], bits2, 2);
        //if(result_num == -1){
        //    cerr << "Invalid number!" << endl;
        //    return -1;
        //}
        result_flag = mpz_inp_raw((*input_qX1)[i].dis, fp_in);
        if(result_flag == 0){
            cerr << "Unable to read!" << endl;
            return -1;
        }else{
            sum_in += result_flag;
        }

        result_flag = mpz_inp_raw((*input_qX1)[i].labels, fp_in);
        if(result_flag == 0){
            cerr << "Unable to read!" << endl;
            return -1;
        }else{
            sum_in += result_flag;
        }

        //cout << data[i] << endl;
    }

    cout << "Result: Read " << sum_in << " bytes." << endl;

    fclose(fp_in);

    return 0;
}

int KNN_Recover(){

    MyTime * ktime = new MyTime();



    cout << "========" << endl;
    cout << "Preprocessing" << endl;
    cout << "========" << endl;

    TrainingParams params;

    int n_;
    vector<double> testing_labels;
    vector<vector<double> > testing_data;

    cout << "Read testing labels..." << endl;
    read_MNIST_labels(false, testing_labels, NUM_IMAGES);
    cout << "Reading Complete." << endl;


    struct node_paillier qX1[train_number+100], qX2[train_number+100];
    struct node qP_recover[train_number+100];
    int a_r[20] = {0};

    memset(qP_recover,0,sizeof(qP_recover));
    memset(a_r,0,sizeof(a_r));


    //cout << "KNN processing..." << endl;
    int sum=0;

    cout << "=======" << endl;
    cout << "KNN Recovering" << endl;
    cout << "=======" << endl;

    string message_begin = "---*** Test for No. ";
    string message_end = " sample is over. ***---";
    string message;
    char const * file_path_his = (char *) "history_record_clients.log";
    char * const file_name_recover_0 = (char*) "Server_Result_Data_0.data";
    char * const file_name_recover_1 = (char*) "Server_Result_Data_1.data";




    //for(int test_index=0; test_index < test_number; test_index++){
    int test_index = 0;
        //memset(qP_recover,0,sizeof(qP_recover));
        //memset(a_r,0,sizeof(a_r));


        mpz_t tmpmpz;
        mpz_init(tmpmpz);

        //Read_Train_Result(file_name_recover_0, file_name_recover_1, &qX1, &qX2);
    for(int i=0;i<train_number;i++){
        mpz_init(qX1[i].dis);
        mpz_init(qX2[i].dis);
        mpz_init(qX1[i].labels);
        mpz_init(qX2[i].labels);
    }

        server_result_input(&qX1, 0, file_name_recover_0);
        server_result_input(&qX2, 1, file_name_recover_1);


        for(int i=0;i<train_number;i++){
            mpz_sub(tmpmpz,qX1[i].dis,qX2[i].dis);
            //cout << qX1[i].dis<< ", " << qX2[i].dis << endl;
            cout << "0" << endl;
            qP_recover[i].dis = mpz_get_ui(tmpmpz);

            mpz_sub(tmpmpz,qX2[i].labels,qX1[i].labels);
            cout << "1" << endl;
            //cout << qX2[i].labels<< ", " << qX1[i].labels << endl;
            qP_recover[i].labels = mpz_get_ui(tmpmpz);

            //cout << qP_recover[i].dis << ", " << qP_recover[i].labels << endl;
        }

        cout << "Distance Recover Complete." << endl;

        //int k = 1;

        sort(qP_recover,qP_recover+train_number,cmpt);
        for(int i=0;i<k_value;i++)
        {
            a_r[qP_recover[i].labels]++;
        }

        int ans_r=-1,minn_r=-1;
        for(int i=0;i<10;i++)
        {
            if(a_r[i]>minn_r)
            {
                minn_r=a_r[i];
                ans_r=i;
            }
        }

        // keep for test


        //cout << "*** Let's compare! ***" << endl;
        cout << "~~~~~~~~~~~~~Result show:~~~~~~~~~" << endl;
        for(int i=0;i<train_number;i++){
            cout << "qP[" << i << "]" << qP_recover[i].dis << ", " << qP_recover[i].labels << ": "  << endl;
        }
        for(int i=0;i<10;i++)
        {
            cout << "qP[" << i << "]" << a_r[i] << endl;
        }
        cout << "ans: " << ans_r << endl;
        cout << "True ans: " << testing_labels[test_index] << endl;

        if(ans_r==(int)testing_labels[test_index]) sum++;

        //cout << "*******Test for No. " << 1 << " sample is over.*********" << endl;
        message = message_begin + to_string(1) + message_end;
        ktime->TimeRecorded(message, file_path_his);

    //}

    //cout << "*****************Summary******************" << endl;
    //printf("When k=%d    precision: %.5f\n", k_value,1.0*sum/test_number);

    ktime->Time_report();

    return 0;
}

int main() {
    int mnist = 10;
    //cout << data[7][3] << endl;
    mnist = knn_DO();
    //server train:
    //mnist = KNN_Recover();
    return 0;
}

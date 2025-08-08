//
// Created by kore on 9/12/21.
//
#include <string>
#include "KNN_single.h"

using namespace std;



int KNN_single::newKNN(int number)//预测函数
{
    ktime->Time_begin(12);
    memset(qP,0,sizeof(qP));
    memset(a,0,sizeof(a));
    memset(qP_recover,0,sizeof(qP_recover));
    memset(a_r,0,sizeof(a_r));

    //cout << "Clear Complet." << endl;
    int dis=0;
    int midint1,midint2;
    double middou1,middou2;

    mpz_t tmpmpz;
    mpz_init(tmpmpz);

    //cout << "Check 0" << endl;

    //HssShare tmp,tmpXi1,tmpXi2,disX1,disX2,disX1sum,disX2sum;
    hss_men_p tmp,tmpXi1,disX1,disX1sum;



    men_share_init(&tmp);  //men
    men_share_init(&tmpXi1); //men of sum
    //men_share_init(&tmpXi2);
    men_share_init(&disX1);  //men of tmp result
    //men_share_init(&disX2);
    men_share_init(&disX1sum); //men of tmp sum
    //men_share_init(&disX2sum);
    ktime->Time_end(12);

    //cout << "OK" << endl;
/*
    Xw1.clear();
    Xw2.clear();

    Xw1m.clear();
    Xw2m.clear();
*/

    //cout << "initialize w..." << endl;
    mpz_t numw;
    mpz_init(numw);


    cout << "Training... " << endl;
    for(int i=0;i<train_number;i++)  //for each train sample
    {
        cout << "Sample " << i << "," << endl;

        //cout << "Sample load:";
        for(int t=0;t<attribute_number;t++){ //read a train samples t to Xi
            convert_shares_paillier(&Xi1m[t], mof11, &(*Xi1)[i][t], hek0, hpk->pk, server_id);
        }
/*        cout << "Params: " << mof11->sharex << ", " << hek0->ds[0] << endl;

        cout << "Xim: " << Xi1m[153].sharex << endl;
        cout << "Xim: " << Xi1m[154].sharex << endl;
        cout << "Xim: " << Xi1m[155].sharex << endl;
        cout << "Xim: " << Xi1m[152].sharex << endl;
*/


        mpz_set_ui(numw, 0);
        //men_share_init(&disX1sum);
        men_share_zeros(&disX1sum);
        //fromINTtoMEN_value(&disX1sum, &disX2sum, &setup->mof11, &setup->mof12, &setup->hpk, &setup->hek0, &setup->hek1, numw, ktime);
        //gmp_printf("the 0 sum check:(%Zd)\n",disX1sum.sharex);
        //mpz_set_ui(numw, 0);
        //men_share_init(&disX1);
        men_share_zeros(&disX1);
        //fromINTtoMEN_value(&disX1, &disX2, &setup->mof11, &setup->mof12, &setup->hpk, &setup->hek0, &setup->hek1, numw, ktime);
        //gmp_printf("the 0 check:(%Zd)\n",disX1.sharex);
        //fromINTtoHSS(&disX1sum,&disX2sum, 0); //init params

        //cout << disX1sum.sharex << endl;


        //cout << "point 2" << endl;
        for(int j=0;j<attribute_number;j++){  // calculate the distance between the t train sample and the prediction sample

            mult_shares_paillier(&tmp,&Xi1m[j],&((*Xi1)[i][j]), hek0, hpk->pk, server_id);

            mult_shares_paillier(&disX1,&Xw1m[j],&((*Xw1)[j]), hek0, hpk->pk, server_id);

            add_shares_paillier(&disX1,&disX1,&tmp, hek0, hpk->pk, server_id);

            mult_shares_paillier(&tmp,&Xi1m[j],&(*Xw1)[j], hek0, hpk->pk, server_id);

            men_share_multc(&tmp, &tmp, 2, hpk->pk);

            sub_shares_paillier(&disX1, &disX1, &tmp, hek0, hpk->pk, server_id);

            add_shares_paillier(&disX1sum,&disX1,&disX1sum,  hek0, hpk->pk, server_id);

        }
        mpz_init_set(((*qX1)[i].dis), disX1sum.sharex);
        mpz_init_set(((*qX1)[i].labels), (*Yw1)[i]);

    }
    cout << "Test over once." << endl;

    men_share_clear(&tmp);  //men
    men_share_clear(&tmpXi1); //men of sum
    //men_share_clear(&tmpXi2);
    men_share_clear(&disX1);  //men of tmp result
    //men_share_clear(&disX2);
    men_share_clear(&disX1sum); //men of tmp sum
    //men_share_clear(&disX2sum);

    mpz_clear(tmpmpz);

    return 0;
}

void KNN_single::start_knn_origin(){
    //int k = 1;

    //int sum=0;
    //int records_test[test_number+100] =  {0};
    //int labels_test[test_number+100] =  {0};

    //string message_begin = "Test for No. ";
    //string message_end = " sample is over. ";
    //string message;

    //for(int i=0;i<test_number;i++){
        int predict=newKNN(1);
        //cout << "Test for No. " << 1 << " sample is over." << endl;
        //message = message_begin + to_string(1) + message_end;
        //ktime->TimeRecorded(message, file_path_his);
    //}
/*
    printf("read records\n");
    //rewind(fp);
    //STU out_stu[2] = {0};

    int tmp_i;
    sum=0;
    for(int i =0;i<test_number;i++){
        message = "Result:" + to_string(tmp_i) + "," + to_string(records_test[i]) + "," + to_string(labels_test[i]);
        ktime->TimeRecorded(message, file_path_his);
        if(records_test[i]==labels_test[i]) sum++;
    }

    message = "k=" + to_string(k) + " precision: " + to_string(1.0*sum/test_number) + ";" ;
    ktime->TimeRecorded(message, file_path_his);*/
}

void KNN_single::start_knn(){
    int k = 1;

    int pid[test_number];
    int sum=0;
    int records_test[test_number+100] =  {0};
    int labels_test[test_number+100] =  {0};


    string message_begin = "Test for No. ";
    string message_end = "Test for No. ";
    string message;
    //&setup->mof11, &setup->mof12, &setup->hpk, &setup->hek0, &setup->hek1,

    for(int i=0;i<test_number;i++){
        cout << "Now fork for " << i << " son." << endl;
        pid[i] = fork();

        if (pid[i] < 0){
            printf("error\n");
            //return 1;
        }
        else if (pid[i] == 0){
            printf("fork success,this is son process\n");
            //int predict=KNN(i,k, hpk, hek0, hek1);
            int predict=newKNN(i);//KNN_seperate//no k for now
            //printf("pre:%d label:%d\n",predict,(int)labels1[i]);
            //ktime->TimeRecorded("knn ends.", file_path_his);

            exit(0);

        }
        else{
            printf("fork success,this is father process,son process id is %d \n",pid[i]);
        }

        if(pid[i]!=0)
        {
            int StateVal;
            pid_t ChildPid;
            ChildPid = wait(&StateVal);
            cout << "The child has finished with the PID of " << ChildPid << endl;
            cout << "Test for No. " << i << " test sample is over." << endl;


            message = message_begin + to_string(i) + message_end;

            //ktime->TimeRecorded(message, file_path_his);

            //sleep(10);
            if(WIFEXITED(StateVal)){
                cout << "the child process has exit with code " << WEXITSTATUS(StateVal) << endl;
            }else cout << "the child has terminated abnormally" << endl;
        }

    }

    printf("read records\n");
    //rewind(fp);
    //STU out_stu[2] = {0};

    int tmp_i;
    sum=0;
    for(int i =0;i<test_number;i++){
        //(fp, "%d,%d,%d;", i, predict, (int)labels1[i]);
        //fscanf(fp_test, "%d,%d,%d;", &tmp_i,&records_test[i], &labels_test[i]);
        message = "Result:" + to_string(tmp_i) + "," + to_string(records_test[i]) + "," + to_string(labels_test[i]);
        //ktime->TimeRecorded(message, file_path_his);
        if(records_test[i]==labels_test[i]) sum++;
    }

    message = "k=" + to_string(k) + " precision: " + to_string(1.0*sum/test_number) + ";" ;
    //ktime->TimeRecorded(message, file_path_his);

}

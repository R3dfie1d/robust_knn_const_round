#include <iostream>
#include <vector>
#include <gmpxx.h>
#include <cmath>
#include<sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <thread>
#include<arpa/inet.h>
#include <queue>
#include <functional>

#include "KNN_single.h"
#include "readData.h"
#include "Poly.h"
#include "polynomial.h"
#include "spec_sort.h"

#define CLIENT 3
#define MAXLINE 1024
#define SERVER_PORT 1234
#define MAXDATASIZE 1024    //缓冲区大小

using namespace std;

int NUM_IMAGES = 10240;
//int PARTY;
int pp_cmp_tree[10][10];

int server_id;
int server_port;

typedef struct SockInfo{
    int fd;
    struct sockaddr_in addr;
    socklen_t addr_len;
    pthread_t id;
}SockInfo;


struct knnInfo{
    int fd;
    struct sockaddr_in addr;
    socklen_t addr_len;
    pthread_t id;
    KNN_single * server = NULL;
};

int Simulate_case = 0; //1 if S1-S2 ; 2 if C-S1 ; 3 if C-S2;
int comm_times_SS = 0, comm_times_CS = 0; //count call times
int comm_overhead_SS = 0, comm_overhead_CS = 0;  //count mpz_t
int n=100, m=8, k_num=5, l=32, w=20; //l=bits of encryption
int enc_times = 0, dec_times = 0;

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

int server_read_input(mpz_t Q[attribute_number], mpz_t Q2, mpz_t random, char const * filepath){
    unsigned int result_flag, sum_in = 0;

    FILE *fp_in = NULL;
    fp_in = fopen(filepath, "r");

    mpz_t tmpNum;
    mpz_init(tmpNum);

    for (int t = 0; t < attribute_number-1; t++) {//read a train samples t to Xi

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


    result_flag = mpz_inp_raw(tmpNum, fp_in);
    if (result_flag == 0) {
        cerr << "Unable to read!" << endl;
        return -1;
    } else {
        sum_in += result_flag;
    }

    mpz_set(random, tmpNum);

    cout << "Result: Read " << sum_in << " bytes." << endl;
    fclose(fp_in);

    return 0;
}

void* pthread_Callback(void* arg){
    knnInfo *info = (knnInfo*)arg;

    while(1)
    {
        char ip[64]={0};
        char buf[300*(attribute_number+1)+1024] = {0};

        char * ot1 = new char[16*train_number+1024];
        char * ot2 = new char[16*train_number+1024];
        char * refreshed_recv = new char[16+1];
        char * refreshed_return = new char[16+1];

        //0. New client connect, receive query. 
        printf("New Client IP: %s, Port: %d\n",
               inet_ntop(AF_INET,&info->addr.sin_addr.s_addr,ip,sizeof(ip)),
               ntohs(info->addr.sin_port));
        int len = read(info->fd,buf,300*(attribute_number+1+1));
        if(len == -1){
            perror("read err");
            pthread_exit(NULL);
        }else if(len == 0){
            printf("Client disconnect\n");
            close(info->fd);
            break;
        }else{
            printf("[1] Receive %d Bytes (query) from client.\n",len);

            mpz_t preQ[attribute_number];
            mpz_t preQ2;
            mpz_t random;
            for (int t = 0; t < attribute_number; t++){ //read a train samples t to Xi
                mpz_init(preQ[t]);
            }
            mpz_init(preQ2);
            mpz_init(random);
        //0.1 Read in value of q and q^2 from query.
            const char sep[2] = "-";
            char * token;
            int lab_ind = 1;
            token = strtok(buf, sep);
            if(token != NULL){
                mpz_set_str(preQ[0], token, 36);
            }else{
                cout << "Sep Error!" << endl;
                cout << buf << endl;
            }
            while( token != NULL ){
                if(lab_ind >= attribute_number-1){
                    //cout << "Too many selections!" << endl;
                    break;
                }
                cout  << "Extract " << lab_ind << " labels..." << endl;
                printf( "%s\n", token );
                token = strtok(NULL, sep);
                mpz_set_str(preQ[lab_ind], token, 36);
                lab_ind++;
            }
            token = strtok(NULL, sep);
            mpz_set_str(preQ2, token, 36);
            token = strtok(NULL, sep);
            mpz_set_str(random, token, 36);
            cout << "Receive query from clients, q[0] is " << preQ[0] << endl;
        //0.2 Record query values
            info->server->set_query_values(preQ, preQ2, random, info->id);
            cout << info->id << endl;
            for(int t = 0; t < attribute_number; t++){ //read a train samples t to Xi
                mpz_clear(preQ[t]);
            }
            mpz_clear(preQ2);

        //1. Run the local distance computation protocol using received query
            info->server->query(info->id);

        //2. Run the non-interactive distance compare protocol with client (agent)
        //2.1 Write the complete comparison result to files and send to client.
            char buffer[MAXLINE];
            FILE *fq;
            char output_file1_name[128];
            sprintf(output_file1_name,"Compare_tree_%d.data",server_id); //Replaced with absolute Path of file?
            if( ( fq = fopen(output_file1_name,"rb") ) == NULL ){
                printf("File open error.\n");
                exit(1);
            }
            bzero(buffer,sizeof(buffer));
            while(!feof(fq)){
                len = fread(buffer, 1, sizeof(buffer), fq);
                if(len != write(info->fd, buffer, len)){
                    printf("write error.\n");
                    break;
                }
            }
            fclose(fq);
            //delete [] delta;
        }

        //2.2 Read in the k-selection result from client
        int select_i[k_value];
        len = read(info->fd,ot1,(16+1)*(k_value));
        if(len == -1){
            perror("read err");
            pthread_exit(NULL);
        }else if(len == 0){
            printf("Client disconnect\n");
            close(info->fd);
            break;
        }else{
            printf("[2] Receive %d Bytes (k-selection between distances) from client.\n",len);

            const char sep[2] = "-";
            char *token;
            mpz_t select[k_value];

            int lab_ind = 1;

        //2.2.1 Read k tokens that the client select
            token = strtok(ot1, sep);
            if(token != NULL){
                mpz_init(select[0]);
                mpz_set_str(select[0], token, 10);
                select_i[0] = atoi(token);
            }else{
                cout << "Sep Error!" << endl;
                cout << ot1 << endl;
            }
            while( token != NULL ){
                if(lab_ind >= k_value){
                    cout << "Too many selections!" << endl;
                    break;
                }
                cout  << "Extract " << lab_ind << " labels..." << endl;
                printf( "%s\n", token );
                token = strtok(NULL, sep);
                mpz_init(select[lab_ind]);
                mpz_set_str(select[lab_ind], token, 10);
                select_i[lab_ind] = atoi(token);
                lab_ind++;
            }
            cout << "Show selected indexes: " << endl;
            for(int j=0; j<k_value; j++){
                cout << j << ": " << select[j] << " ";
            }
            cout  << endl;

            //2.3 Using secure two-party comparisons to obtain top-k labels
            char k_message[16*k_value];
            // Reconstruct top-k with secure compare trees from both servers
            info->server->topk_select(info->id, k_message);
            // [Legacy-cheat-impl] Previous simplified top-k (kept for reference)
            // ---------------------------------------------------------------
            // //2.3 Using selected index of distances to obtain the majority of them
            // //For simplicity, using this function to directly obtain labels of correpsonding top-k distances
            // info->server->cheat_select_top_k(info->id, select_i, k_message);
            // //Not applied. 
            // ---------------------------------------------------------------
            // Continue with Oblivious Transfer phase
            info->server->oblivious_transfer(info->id);

            strncpy(ot2, k_message, 16*train_number);
            //sprintf(ot,"%1600.10s \n","Good bye!");
            //output the label corresponds to k training samples
            write(info->fd,ot2,16*train_number);
        }
        
        //Read another selection from client.
        len = read(info->fd,refreshed_return,16);
        if(len == -1)
        {
            perror("read err");
            pthread_exit(NULL);
        }else if(len == 0)
        {
            printf("Client disconnect\n");
            close(info->fd);
            break;
        }else{
            refreshed_recv[len] = '\0';
            //printf("recv2:%d, %s\n", len, ot1);
            printf("[3] Receive %d Bytes (refresh label) from client.\n", len);
            mpz_set_ui(info->server->refreshed_label[info->id], info->id);
            info->server->refresh(info->id);

            //Return the final offseted label
            memset(refreshed_return, '/', 16);
            refreshed_return[16] = '\0';
            //sprintf(ot,"%1600.10s \n","Good bye!");
            write(info->fd,refreshed_return,16);

            close(info->fd);
            printf("task finished.\n");
            break;
        }
        delete [] ot1;
        delete [] ot2;
    }
    pthread_exit(NULL);
}

int server_evaluate(){
    //Log, Input and Output Files
    char const * file_path_his = (char *) "history_record.log";
    char const * KeyFilePath = (char*) "KeyFilePath.data";
    char const * file_name_server_out_0 = (char *) "Server_Result_Data_0.data";
    char const * file_name_server_out_1 = (char *) "Server_Result_Data_1.data";
    char const * C_file_name_out_0 = (char *) "C_HSS_Input_Data_0.data";
    char const * C_file_name_out_1 = (char *) "C_HSS_Input_Data_1.data";

    //Time record
    struct timeval t1,t2;
    double timeuse;
    gettimeofday(&t1,NULL);

    cout << "========================================" << endl;
    cout << "Server " << server_id << " is performing offline preprocess, please wait..." << endl;
    cout << "========================================" << endl;
    SetupPhase * setup = new SetupPhase(KeyFilePath, server_id);

    cout << "========================================" << endl;
    cout << "KNN Predicting" << endl;
    cout << "========================================" << endl;
    TrainingParams params;
    KNN_single * kp = new KNN_single(&setup->mof11,  &setup->hpk, &setup->hek0, params, &setup->KeySize, setup->base_sk, server_id);

    gettimeofday(&t2,NULL);
    cout << "Offline preprocess of this server takes = "<< ((t2.tv_sec - t1.tv_sec) + (double)(t2.tv_usec - t1.tv_usec) / 1000000.0) << " seconds. " << endl;
    
    //Get ready for connections from clients
    int listenfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_port = htons(server_port+server_id);
    serv_addr.sin_family = AF_INET;
    socklen_t serv_len = sizeof(serv_addr);
    int ret;
    cout << "Try to bind" << server_port + server_id << endl;
    if((ret =bind(listenfd,(struct sockaddr*)&serv_addr,serv_len)) != 0){
        perror("err: bind");
        exit(1);
    }
    if((ret =listen(listenfd,36))!=0){
        perror("err: listen");
        exit(2);
    }
    cout << "========================================" << endl;
    cout << "Server " << server_id << " is now online available. " << endl;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

    knnInfo infos[pthread_max_number];
    for (int j=0;j<sizeof(infos)/sizeof(knnInfo);++j)
        infos[j].fd = -1;

    int i;
    while(1){
        for (i=0;i<sizeof(infos)/sizeof(knnInfo);++i){
            if(infos[i].fd == -1)
                break;
        }
        if(i == pthread_max_number)
            break;

        struct sockaddr_in cli_addr;
        socklen_t cli_len = sizeof(cli_addr);
        infos[i].fd =accept(listenfd,(struct sockaddr*)&cli_addr,&cli_len);
        infos[i].addr = cli_addr;
        infos[i].addr_len = cli_len;
        infos[i].server = kp;
        infos[i].id = i;

        pthread_t tid;
        pthread_create(&tid,&attr,pthread_Callback,&(infos[i]));
    }
    close(listenfd);
    pthread_attr_destroy(&attr);
    pthread_exit(NULL);

    return 0;
}

int main(int argc, char** argv) {
    string address;

    server_id = atoi(argv[1]) - 1; //server_id == 0 or 1
    server_port = atoi(argv[2]);
    try{
        int x = -1;
        if(argc <= 3)
            throw x;
        address = argv[3];
    }catch(int x) {
        address = "127.0.0.1";
    }

    return server_evaluate();
}

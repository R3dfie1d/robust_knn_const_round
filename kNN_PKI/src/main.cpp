#include <iostream>
#include <vector>
#include <gmpxx.h>
#include <cmath>

// Removed unused headers; keep only what's necessary for key setup
#include "SetupPhase.h"
#include "MyTime.h"

using namespace std;

int knn_KeySetup(){

    cout << "========" << endl;
    cout << "Setup" << endl;
    cout << "========" << endl;

    char const * file_path_his = (char *) "history_record_PKI.log";

    char const * KeyFilePath = (char*) "KeyFilePath.data";

    SetupPhase * setup = new SetupPhase();
    cout << "========" << endl;
    cout << "Setup complete. Now exporting..." << endl;
    cout << "========" << endl;
    setup->exprotKeys(KeyFilePath);

    cout << "========" << endl;
    cout << "Export complete. Now importing..." << endl;
    cout << "========" << endl;
    SetupPhase * new_setup = new SetupPhase(KeyFilePath);


    MyTime * ktime = new MyTime();

    string message;
    message = "Checking the validation.";
    ktime->TimeRecorded(message, file_path_his);
    cout << setup->hek0.ds[10]<< endl;
    cout << setup->hek1.ds[10]<< endl;
    cout << new_setup->hek0.ds[10]<< endl;
    cout << new_setup->hek1.ds[10]<< endl;

    cout << setup->hpk.pk->n<< endl;
    cout << new_setup->hpk.pk->n<< endl;

    cout << setup->hpk.pk->len<< endl;
    cout << new_setup->hpk.pk->len<< endl;

    cout << setup->mof11.sharex<< endl;
    cout << new_setup->mof11.sharex<< endl;

    cout << setup->KeySize << endl;
    cout << new_setup->KeySize << endl;

    message = "PKI finish setup";
    ktime->TimeRecorded(message, file_path_his, 2);
    //&setup->mof11, &setup->mof12, &setup->hpk, &setup->hek0, &setup->hek1,

    return 0;
}


int main() {
    int mnist;
    mnist = knn_KeySetup();
    return 0;
}

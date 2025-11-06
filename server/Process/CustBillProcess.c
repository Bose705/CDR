// CustBillProcess.c - Customer billing CDR processing
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "CustBillProcess.h"

void* custbillprocess(void *arg) {
    (void)arg;
    // simulate work (e.g., parsing CDRs for customer billing)
    sleep(2); // replace with real customer billing logic
    
    // simulate writing an output file
    FILE *f = fopen("data/CB.txt", "w");
    if (f) {
        fprintf(f, "Customer Billing processed at %ld\n", time(NULL));
        fprintf(f, "MSISDN|CallDuration|ChargeAmount\n");
        fprintf(f, "9876543210|120|24.50\n");
        fprintf(f, "9876543211|85|17.00\n");
        fclose(f);
    }
    return NULL;
}

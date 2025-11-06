// IntopBillProcess.c - Interoperator billing CDR processing
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "IntopBillProcess.h"

void* intopbillprocess(void *arg) {
    (void)arg;
    // simulate work (e.g., aggregation for interoperator billing)
    sleep(3); // replace with real interoperator billing logic
    
    // simulate writing an output file
    FILE *f = fopen("data/IOSB.txt", "w");
    if (f) {
        fprintf(f, "Interoperator Settlement Billing processed at %ld\n", time(NULL));
        fprintf(f, "Operator|TotalCalls|TotalAmount\n");
        fprintf(f, "Airtel|150|3750.00\n");
        fprintf(f, "Vodafone|220|5500.00\n");
        fclose(f);
    }
    return NULL;
}

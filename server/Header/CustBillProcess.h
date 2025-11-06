#ifndef CUSTBILLPROCESS_H
#define CUSTBILLPROCESS_H

#include <pthread.h>

#define HASH_SIZE 1000

// Customer structure for billing
typedef struct Customer {
    long msisdn;
    char operatorName[64];
    int operatorCode;
    
    // Voice call durations (within and outside operator)
    float inVoiceWithin;
    float outVoiceWithin;
    float inVoiceOutside;
    float outVoiceOutside;
    
    // SMS counts
    int smsInWithin;
    int smsOutWithin;
    int smsInOutside;
    int smsOutOutside;
    
    // Data usage
    float mbDownload;
    float mbUpload;
    
    struct Customer *next; // for hash collision chaining
} Customer;

// Thread argument structure for passing output directory
typedef struct {
    char output_dir[256];
} ProcessThreadArg;

// Function declarations
void* custbillprocess(void *arg);

// Customer Billing search and display functions
void search_msisdn(int client_fd, const char *filename, long msisdn);
void display_customer_billing_file(int client_fd, const char *filename);

#endif // CUSTBILLPROCESS_H

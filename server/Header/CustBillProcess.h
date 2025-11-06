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

// Function declarations
void* custbillprocess(void *arg);

#endif // CUSTBILLPROCESS_H

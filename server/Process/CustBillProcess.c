// CustBillProcess.c - Customer billing CDR processing
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../Header/CustBillProcess.h"

// Hash table for customer records
static Customer *hashTable[HASH_SIZE];
static int totalRecords = 0;

// Hash function - simple modulo hash
static unsigned int hashFunction(long key) {
    return (unsigned int)(key % HASH_SIZE);
}

// Create a new customer entry
static Customer *createCustomer(long msisdn, const char *operatorName, int operatorCode) {
    Customer *cust = (Customer *)malloc(sizeof(Customer));
    if (!cust) return NULL;
    
    cust->msisdn = msisdn;
    strncpy(cust->operatorName, operatorName, sizeof(cust->operatorName) - 1);
    cust->operatorName[sizeof(cust->operatorName) - 1] = '\0';
    cust->operatorCode = operatorCode;
    
    cust->inVoiceWithin = cust->outVoiceWithin = 0;
    cust->inVoiceOutside = cust->outVoiceOutside = 0;
    cust->smsInWithin = cust->smsOutWithin = 0;
    cust->smsInOutside = cust->smsOutOutside = 0;
    cust->mbDownload = cust->mbUpload = 0;
    cust->next = NULL;
    
    return cust;
}

// Get or create a customer in the hash map
static Customer *getCustomer(long msisdn, const char *operatorName, int operatorCode) {
    unsigned int index = hashFunction(msisdn);
    Customer *curr = hashTable[index];
    
    // Search for existing customer
    while (curr) {
        if (curr->msisdn == msisdn)
            return curr;
        curr = curr->next;
    }
    
    // Create new customer and add to chain
    Customer *newCust = createCustomer(msisdn, operatorName, operatorCode);
    if (newCust) {
        newCust->next = hashTable[index];
        hashTable[index] = newCust;
    }
    
    return newCust;
}

// Process CDR file and aggregate customer data
static void processCDRFile(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error opening CDR file '%s': %s\n", filename, strerror(errno));
        return;
    }
    
    char line[512];
    totalRecords = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        // Remove newline characters
        line[strcspn(line, "\r\n")] = 0;
        
        long msisdn = 0, thirdPartyMsisdn = 0;
        char opName[64] = {0}, callType[16] = {0};
        int opCode = 0, thirdPartyOpCode = 0;
        float duration = 0, download = 0, upload = 0;
        
        // Parse CDR line: MSISDN|OPERATOR NAME|OPERATOR CODE|CALL TYPE|DURATION|DOWNLOAD|UPLOAD|THIRD PARTY MSISDN|THIRD PARTY OPERATOR
        int matched = sscanf(line, "%ld|%63[^|]|%d|%15[^|]|%f|%f|%f|%ld|%d",
                            &msisdn, opName, &opCode, callType,
                            &duration, &download, &upload,
                            &thirdPartyMsisdn, &thirdPartyOpCode);
        
        if (matched < 9) {
            // Handle GPRS with missing third party MSISDN (two consecutive ||)
            matched = sscanf(line, "%ld|%63[^|]|%d|%15[^|]|%f|%f|%f||%d",
                            &msisdn, opName, &opCode, callType,
                            &duration, &download, &upload, &thirdPartyOpCode);
            if (matched < 8)
                continue; // skip invalid lines
        }
        
        Customer *cust = getCustomer(msisdn, opName, opCode);
        if (!cust) continue;
        
        int sameOperator = (opCode == thirdPartyOpCode);
        
        // Update based on call type
        if (strcmp(callType, "MOC") == 0) {
            if (sameOperator)
                cust->outVoiceWithin += duration;
            else
                cust->outVoiceOutside += duration;
        }
        else if (strcmp(callType, "MTC") == 0) {
            if (sameOperator)
                cust->inVoiceWithin += duration;
            else
                cust->inVoiceOutside += duration;
        }
        else if (strcmp(callType, "SMS-MO") == 0) {
            if (sameOperator)
                cust->smsOutWithin++;
            else
                cust->smsOutOutside++;
        }
        else if (strcmp(callType, "SMS-MT") == 0) {
            if (sameOperator)
                cust->smsInWithin++;
            else
                cust->smsInOutside++;
        }
        else if (strcmp(callType, "GPRS") == 0) {
            cust->mbDownload += download;
            cust->mbUpload += upload;
        }
        
        totalRecords++;
    }
    
    fclose(fp);
}

// Write aggregated customer billing data to output file
static void writeCBFile(const char *outputFile) {
    FILE *fp = fopen(outputFile, "w");
    if (!fp) {
        fprintf(stderr, "Error creating output file '%s': %s\n", outputFile, strerror(errno));
        return;
    }
    
    int customerCount = 0;
    fprintf(fp, "#Customers Data Base:\n");
    
    for (int i = 0; i < HASH_SIZE; i++) {
        Customer *cust = hashTable[i];
        while (cust) {
            fprintf(fp, "\nCustomer ID: %ld (%s)\n", cust->msisdn, cust->operatorName);
            fprintf(fp, "* Services within the mobile operator *\n");
            fprintf(fp, "Incoming voice call durations: %.2f\n", cust->inVoiceWithin);
            fprintf(fp, "Outgoing voice call durations: %.2f\n", cust->outVoiceWithin);
            fprintf(fp, "Incoming SMS messages: %d\n", cust->smsInWithin);
            fprintf(fp, "Outgoing SMS messages: %d\n", cust->smsOutWithin);
            fprintf(fp, "* Services outside the mobile operator *\n");
            fprintf(fp, "Incoming voice call durations: %.2f\n", cust->inVoiceOutside);
            fprintf(fp, "Outgoing voice call durations: %.2f\n", cust->outVoiceOutside);
            fprintf(fp, "Incoming SMS messages: %d\n", cust->smsInOutside);
            fprintf(fp, "Outgoing SMS messages: %d\n", cust->smsOutOutside);
            fprintf(fp, "* Internet use *\n");
            fprintf(fp, "MB downloaded: %.2f | MB uploaded: %.2f\n",
                    cust->mbDownload, cust->mbUpload);
            fprintf(fp, "----------------------------------------\n");
            customerCount++;
            cust = cust->next;
        }
    }
    
    fclose(fp);
}

// Free all allocated memory
static void cleanupHashTable() {
    for (int i = 0; i < HASH_SIZE; i++) {
        Customer *cust = hashTable[i];
        while (cust) {
            Customer *temp = cust;
            cust = cust->next;
            free(temp);
        }
        hashTable[i] = NULL;
    }
}

// Main thread function - processes CDR and generates customer billing
void* custbillprocess(void *arg) {
    (void)arg;
    
    const char *inputPath = "data/data.cdr";
    const char *outputPath = "Output/CB.txt";
    
    // Initialize hash table
    for (int i = 0; i < HASH_SIZE; i++)
        hashTable[i] = NULL;
    
    // Process CDR file
    processCDRFile(inputPath);
    
    // Write customer billing output
    writeCBFile(outputPath);
    
    // Cleanup
    cleanupHashTable();
    
    return NULL;
}

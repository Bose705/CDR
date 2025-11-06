// process.c - CDR processing helpers
// Implement processCDRdata which runs two functions concurrently using pthreads

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../Header/process.h"
#include "../Header/CustBillProcess.h"
#include "../Header/IntopBillProcess.h"

#define BUFSIZE 1024

// send all helper for a socket fd
static int sendall_fd(int sock, const char *buf, size_t len) {
    size_t total = 0;
    while (total < len) {
        ssize_t n = send(sock, buf + total, len - total, 0);
        if (n <= 0) return -1;
        total += n;
    }
    return 0;
}

// send a line (null terminated) with newline appended
static int send_line_fd(int sock, const char *s) {
    char tmp[BUFSIZE];
    snprintf(tmp, sizeof(tmp), "%s\n", s);
    return sendall_fd(sock, tmp, strlen(tmp));
}

// Public API: run two processing functions in parallel and notify client when done
int processCDRdata(int client_fd) {
    pthread_t t1, t2;
    int rc;

    // Inform client that processing has started
    send_line_fd(client_fd, "Processing CDR data: started...");

    rc = pthread_create(&t1, NULL, custbillprocess, NULL);
    if (rc != 0) {
        send_line_fd(client_fd, "Error: failed to start Customer Billing processing thread");
        return 0;
    }

    rc = pthread_create(&t2, NULL, intopbillprocess, NULL);
    if (rc != 0) {
        send_line_fd(client_fd, "Error: failed to start Interoperator Billing processing thread");
        // join thread 1 if needed
        pthread_join(t1, NULL);
        return 0;
    }

    // Optionally, while waiting, we could stream progress updates. For now just join.
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Both parts done
    send_line_fd(client_fd, "Processing CDR data: completed.");
    return 1;
}

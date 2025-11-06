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

#include "process.h"

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

// Simulated processing functions
static void *process_part_a(void *arg) {
    (void)arg;
    // simulate work (e.g., parsing CDRs)
    sleep(2); // replace with real work
    // simulate writing an output file
    FILE *f = fopen("data/part_a_out.txt", "w");
    if (f) {
        fprintf(f, "part A processed at %ld\n", time(NULL));
        fclose(f);
    }
    return NULL;
}

static void *process_part_b(void *arg) {
    (void)arg;
    // simulate work (e.g., aggregation)
    sleep(3); // replace with real work
    FILE *f = fopen("data/part_b_out.txt", "w");
    if (f) {
        fprintf(f, "part B processed at %ld\n", time(NULL));
        fclose(f);
    }
    return NULL;
}

// Public API: run two processing functions in parallel and notify client when done
int processCDRdata(int client_fd) {
    pthread_t t1, t2;
    int rc;

    // Inform client that processing has started
    send_line_fd(client_fd, "Processing CDR data: started...");

    rc = pthread_create(&t1, NULL, process_part_a, NULL);
    if (rc != 0) {
        send_line_fd(client_fd, "Error: failed to start CDR processing thread A");
        return 0;
    }

    rc = pthread_create(&t2, NULL, process_part_b, NULL);
    if (rc != 0) {
        send_line_fd(client_fd, "Error: failed to start CDR processing thread B");
        // join thread A if needed
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

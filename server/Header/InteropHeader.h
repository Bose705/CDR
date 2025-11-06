#ifndef INTEROP_HEADER_H
#define INTEROP_HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

/* ============================================================
   Constants
   ============================================================ */
#define NUM_BUCKETS 4096

/* ============================================================
   Data Structures
   ============================================================ */

/**
 * @struct OperatorStats
 * @brief Statistics for a telecom operator
 */
typedef struct OperatorStats
{
    char *operator_name;     // first seen operator name, this value is also unique.
    long total_moc_duration; // Mobile Originated Call duration (Outgoing)
    long total_mtc_duration; // Mobile Terminated Call duration (Incoming)
    long sms_mo_count;       // SMS Mobile Originated (Outgoing) Count
    long sms_mt_count;       // SMS Mobile Terminated (Incoming) Count
    long total_download;     // MB Downloaded
    long total_upload;       // MB Uploaded
} OperatorStats;

/**
 * @struct OpNode
 * @brief Node structure for operator hash map
 */
typedef struct OpNode
{
    char *operator_id;   // hash map (key) operator_id
    OperatorStats stats; // Hash map (Value)
    struct OpNode *next; // Chaining (linked list)
} OpNode;

/* ============================================================
   Function Declarations
   ============================================================ */

/**
 * @brief Processes a CDR file and generates statistics
 * @param filename Path to the input CDR file
 * @param output_filename Path to the output statistics file
 */
void InteroperatorBillingFile(const char *filename, const char *output_filename);

void search_operator(const char *filename, const char *operator);

void display_file(const char *filename);

/**
 * @brief Processes a single line from the CDR file
 * @param line The line to process
 */
void process_line(char *line);

/**
 * @brief Generates a hash value for a string
 * @param s The string to hash
 * @return unsigned long The hash value
 */
unsigned long str_hash(const char *s);

/**
 * @brief Gets or creates an operator node in the hash map
 * @param operator_id The operator ID to look up
 * @param operator_name The operator name (used if creating new node)
 * @return OpNode* Pointer to the operator node
 */
OpNode* get_or_create_opnode(const char *operator_id, const char *operator_name);

/**
 * @brief Removes trailing newline characters from a string
 * @param s The string to process
 */
void chomp(char *s);

/**
 * @brief Splits a string by pipe character
 * @param line The line to split
 * @param tokens Array to store the resulting tokens
 * @param max_tokens Maximum number of tokens to split into
 * @return int Number of tokens found
 */
int split_pipe(char *line, char **tokens, int max_tokens);

/**
 * @brief Converts a string to long, returns 0 if invalid
 * @param s The string to convert
 * @return long The converted value or 0
 */
long to_long_or_zero(const char *s);

#endif /* INTEROP_HEADER_H */
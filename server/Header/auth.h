#ifndef AUTH_H
#define AUTH_H
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // For isupper(), islower(), isdigit()
 
#define EMAIL_MAX 64
#define PASS_MAX 32
#define USER_FILE "data/user.txt"
 
// Authentication function declarations
int is_valid_email(const char *email);
int is_valid_password(const char *password);
int save_user(const char *email, const char *password);
int verify_user(const char *email, const char *password);
int user_exists(const char *email);  // 👈 new function
void encrypt_decrypt(char *data);
 
#endif // AUTH_H
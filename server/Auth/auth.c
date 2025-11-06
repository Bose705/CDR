#include "../Header/auth.h"
 
const char encryption_key[] = "SECRETKEY123";
 
// XOR encryption/decryption
void encrypt_decrypt(char *data) {
    int key_len = strlen(encryption_key);
    int data_len = strlen(data);
    for (int i = 0; i < data_len; i++)
        data[i] = data[i] ^ encryption_key[i % key_len];
}
 
// Validate email format
int is_valid_email(const char *email) {
    int at_count = 0, dot_after_at = 0;
    int len = strlen(email);
    if (len >= EMAIL_MAX || len < 5) return 0;
 
    for (int i = 0; i < len; i++) {
        if (email[i] == '@') at_count++;
        if (at_count == 1 && email[i] == '.') dot_after_at = 1;
    }
 
    return (at_count == 1 && dot_after_at == 1);
}
 
// ✅ Strong password validation
int is_valid_password(const char *password) {
    int len = strlen(password);
    if (len < 6 || len >= PASS_MAX) return 0;
 
    int has_upper = 0, has_lower = 0, has_digit = 0, has_special = 0;
 
    for (int i = 0; i < len; i++) {
        if (isupper(password[i])) has_upper = 1;
        else if (islower(password[i])) has_lower = 1;
        else if (isdigit(password[i])) has_digit = 1;
        else if (strchr("!@#$%^&*()-_=+[]{}|;:'\",.<>?/\\`~", password[i]))
            has_special = 1;
    }
 
    return (has_upper && has_lower && has_digit && has_special);
}
 
// 👇 NEW FUNCTION: Check if user already exists
int user_exists(const char *email) {
    FILE *file = fopen(USER_FILE, "r");
    if (!file) return 0; // If no file yet, no users exist
 
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        char *sep = strchr(line, '|');
        if (!sep) continue;
        *sep = '\0';
 
        char enc_email[EMAIL_MAX];
        strncpy(enc_email, line, EMAIL_MAX - 1);
        enc_email[EMAIL_MAX - 1] = '\0';
 
        encrypt_decrypt(enc_email);
 
        if (strcmp(enc_email, email) == 0) {
            fclose(file);
            return 1;  // ✅ user already exists
        }
    }
 
    fclose(file);
    return 0; // not found
}
 
// Save encrypted user credentials (after checking existence)
int save_user(const char *email, const char *password) {
    // check if already exists
    if (user_exists(email)) return -1;  // -1 = duplicate
 
    FILE *file = fopen(USER_FILE, "a");
    if (!file) return 0;
 
    char enc_email[EMAIL_MAX];
    char enc_pass[PASS_MAX];
    strncpy(enc_email, email, EMAIL_MAX - 1);
    strncpy(enc_pass, password, PASS_MAX - 1);
    enc_email[EMAIL_MAX - 1] = '\0';
    enc_pass[PASS_MAX - 1] = '\0';
 
    encrypt_decrypt(enc_email);
    encrypt_decrypt(enc_pass);
 
    fprintf(file, "%s|%s\n", enc_email, enc_pass);
    fclose(file);
    return 1;  // success
}
 
// Verify credentials by decrypting stored values
int verify_user(const char *email, const char *password) {
    FILE *file = fopen(USER_FILE, "r");
    if (!file) return 0;
 
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        char *sep = strchr(line, '|');
        if (!sep) continue;
        *sep = '\0';
 
        char enc_email[EMAIL_MAX];
        char enc_pass[PASS_MAX];
        strncpy(enc_email, line, EMAIL_MAX - 1);
        strncpy(enc_pass, sep + 1, PASS_MAX - 1);
        enc_email[EMAIL_MAX - 1] = '\0';
        enc_pass[PASS_MAX - 1] = '\0';
 
        encrypt_decrypt(enc_email);
        encrypt_decrypt(enc_pass);
 
        if (strcmp(enc_email, email) == 0 && strcmp(enc_pass, password) == 0) {
            fclose(file);
            return 1;
        }
    }
 
    fclose(file);
    return 0;
}
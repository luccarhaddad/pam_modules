//
// Created by Lucca Haddad on 02/07/24.
//

#include <security/pam_appl.h>
#include <security/openpam.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int function_conversation(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr) {
    struct pam_response *reply = (struct pam_response *)malloc(num_msg * sizeof(struct pam_response));
    if (!reply) {
        return PAM_CONV_ERR;
    }
    for (int i = 0; i < num_msg; i++) {
        if (msg[i]->msg_style == PAM_PROMPT_ECHO_OFF || msg[i]->msg_style == PAM_PROMPT_ECHO_ON) {
            reply[i].resp = strdup((char *)appdata_ptr);
            reply[i].resp_retcode = 0;
        } else {
            free(reply);
            return PAM_CONV_ERR;
        }
    }
    *resp = reply;
    return PAM_SUCCESS;
}

void disable_echo() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void enable_echo() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void print_pam_error(pam_handle_t *pamh, int retval) {
    const char *err_msg = pam_strerror(pamh, retval);
    fprintf(stderr, "PAM Error: %s\n", err_msg);
}

void check_file_access(const char *username, const char *filepath) {
    struct passwd *pwd = getpwnam(username);
    if (!pwd) {
        printf("Invalid user account.\n");
        return;
    }

    if (access(filepath, F_OK) != 0) {
        printf("File %s does not exist.\n", filepath);
        return;
    }

    if (access(filepath, R_OK) == 0) {
        printf("User %s can read the file %s\n", username, filepath);
    } else {
        printf("User %s cannot read the file %s\n", username, filepath);
    }

    if (access(filepath, W_OK) == 0) {
        printf("User %s can write to the file %s\n", username, filepath);
    } else {
        printf("User %s cannot write to the file %s\n", username, filepath);
    }

    if (access(filepath, X_OK) == 0) {
        printf("User %s can execute the file %s\n", username, filepath);
    } else {
        printf("User %s cannot execute the file %s\n", username, filepath);
    }
}

int main() {
    pam_handle_t *pamh = NULL;
    char username[100], password[100];
    int retval;

    printf("PAM Authentication Test\n");
    printf("Enter username: ");
    if (scanf("%99s", username) != 1) {
        fprintf(stderr, "Failed to read username\n");
        return 1;
    }

    printf("Enter password: ");
    disable_echo();
    if (scanf("%99s", password) != 1) {
        fprintf(stderr, "Failed to read password\n");
        enable_echo();
        return 1;
    }
    enable_echo();
    printf("\n");

    struct pam_conv conv = { function_conversation, (void *)password };
    retval = pam_start("login", username, &conv, &pamh);

    if (retval == PAM_SUCCESS) {
        retval = pam_authenticate(pamh, 0);
    }

    if (retval == PAM_SUCCESS) {
        printf("Authenticated successfully\n");
        pam_end(pamh, retval);
        check_file_access(username, "teste.txt");
        return 0;
    }
    print_pam_error(pamh, retval);

    pam_end(pamh, retval);
    return 1;
}
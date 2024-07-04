//
// Created by Lucca Haddad on 02/07/24.
//

#include <stdio.h>
#include <string.h>
#include <security/pam_appl.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

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

int main() {
    pam_handle_t *pamh = NULL;
    char username[100], password[100];

    printf("PAM AUTH MODULE TEST\n");
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
    int retval;

    retval = pam_start("login", username, &conv, &pamh);

    printf("Initializing PAM module...\n");
    sleep(1);
    if (retval == PAM_SUCCESS) {
        retval = pam_authenticate(pamh, 0);
    }

    printf("Authenticating user...\n");
    sleep(1);
    if (retval == PAM_SUCCESS) {
        printf("User Authenticated (SUCCESS)\n");
    } else {
        printf("User Not Authenticated (FAILED)\n");
    }

    if (pam_end(pamh, retval) != PAM_SUCCESS) {
        pamh = NULL;
        printf("Failed to release PAM handle\n");
        return 1;
    }

    return retval == PAM_SUCCESS ? 0 : 1;
}
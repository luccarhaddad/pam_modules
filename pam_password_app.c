//
// Created by Lucca Haddad on 02/07/24.
//

#include <security/pam_appl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

int function_conversation(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr) {
    struct pam_response *reply = (struct pam_response *)malloc(num_msg * sizeof(struct pam_response));
    if (!reply) {
        return PAM_CONV_ERR;
    }

    const char **passwords = (const char **)appdata_ptr;
    static int current_prompt = 0;

    for (int i = 0; i < num_msg; i++) {
        reply[i].resp = strdup(passwords[current_prompt++]);
        if (reply[i].resp == NULL) {
            free(reply);
            return PAM_CONV_ERR;
        }
        reply[i].resp_retcode = 0;
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

int main() {
    pam_handle_t *pamh = NULL;
    struct pam_conv conv;
    int retval;
    char username[100], current_password[100], new_password[100], confirm_new_password[100];
    const char *passwords[3];

    printf("PAM PASSWORD CHANGE MODULE\n");
    printf("Enter username: ");
    if (scanf("%99s", username) != 1) {
        fprintf(stderr, "Failed to read username\n");
        return 1;
    }

    printf("Enter current password: ");
    disable_echo();
    if (scanf("%99s", current_password) != 1) {
        fprintf(stderr, "Failed to read current password\n");
        enable_echo();
        return 1;
    }
    enable_echo();
    printf("\n");

    printf("Enter new password: ");
    disable_echo();
    if (scanf("%99s", new_password) != 1) {
        fprintf(stderr, "Failed to read new password\n");
        enable_echo();
        return 1;
    }
    enable_echo();
    printf("\n");

    printf("Confirm new password: ");
    disable_echo();
    if (scanf("%99s", confirm_new_password) != 1) {
        fprintf(stderr, "Failed to read confirm new password\n");
        enable_echo();
        return 1;
    }
    enable_echo();
    printf("\n");

    passwords[0] = current_password;
    passwords[1] = new_password;
    passwords[2] = confirm_new_password;

    conv.conv = function_conversation;
    conv.appdata_ptr = (void *)passwords;

    retval = pam_start("passwd", username, &conv, &pamh);
    if (retval != PAM_SUCCESS) {
        print_pam_error(pamh, retval);
        return 1;
    }

    retval = pam_chauthtok(pamh, PAM_CHANGE_EXPIRED_AUTHTOK);
    if (retval == PAM_SUCCESS) {
        printf("Password changed (SUCCESS)\n");
    } else {
        printf("Password not changed (FAILED)\n");
        print_pam_error(pamh, retval);
    }

    if (pam_end(pamh, retval) != PAM_SUCCESS) {
        pamh = NULL;
        printf("Failed to release PAM handle\n");
        return 1;
    }

    return retval == PAM_SUCCESS ? 0 : 1;
}
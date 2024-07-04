# Makefile

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lpam

TARGETS = pam_auth_app pam_account_app pam_password_app

all: $(TARGETS)

pam_auth_app: pam_auth_app.c
	$(CC) $(CFLAGS) -o pam_auth_app pam_auth_app.c $(LDFLAGS)

pam_account_app: pam_account_app.c
	$(CC) $(CFLAGS) -o pam_account_app pam_account_app.c $(LDFLAGS)

pam_password_app: pam_password_app.c
	$(CC) $(CFLAGS) -o pam_password_app pam_password_app.c $(LDFLAGS)

auth: pam_auth_app
	@echo "Running pam_auth_app"
	./pam_auth_app

account: pam_account_app
	@echo "Running pam_account_app"
	./pam_account_app

password: pam_password_app
	@echo "Running pam_password_app"
	./pam_password_app

clean:
	rm -f $(TARGETS)
	rm -f *.o

.PHONY: all clean auth account password
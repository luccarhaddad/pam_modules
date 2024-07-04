# PAM Modules

Este projeto contém três aplicativos de exemplo que utilizam os Módulos de Autenticação Plugável (PAM) para autenticação, gerenciamento de contas e gerenciamento de senhas.

## Estrutura do Projeto

O projeto contém os seguintes aplicativos:
- `pam_auth_app`: Aplicativo de autenticação.
- `pam_account_app`: Aplicativo de gerenciamento de contas.
- `pam_password_app`: Aplicativo de gerenciamento de senhas.

## Requisitos

- `gcc`: Um compilador C.
- `libpam`: A biblioteca PAM.

## Compilação

Para compilar todos os aplicativos, execute o seguinte comando:

```bash
make all
```

Para executar o aplicativo de autenticação:

```bash
make auth
```

Para executar o aplicativo de gerenciamento de senhas:

```bash
make password
```

Para limpar os arquivos compilados, execute:

```bash
make clean
```

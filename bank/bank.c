
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

#define FILENAME "accounts.dat"
#define INTEREST_RATE 0.05

typedef struct
{
    char accountNumber[10];
    char name[50];
    char surname[50];
    char address[100];
    char pesel[12];
    double balance;
    double loanBalance;
    double loanInterest;
} Account;

void showMenu();
void createAccount();
void listAccounts();
void searchAccount();
void makeDeposit();
void makeWithdrawal();
void transferMoney();
void takeLoan();
void payDebt();
void clearBuff();
void displayAccount(const Account *acc);
void saveAccount(const Account *acc);
bool updateAccountBalance(const char *accNumber, double amount);
bool readAccount(const char *accNumber, Account *acc);
bool updateAccount(const Account *acc);
bool containEndLine(char *text);
char *getAccountNumber();
int getConfirm();
int validatePesel(char *pesel);
int validateText(char *text);
int intCheck(char *number);

int main()
{
    char choice;
    while (1)
    {
        showMenu();
        scanf("%c", &choice);
        if (feof(stdin))
            return 0;
        getchar();

        switch (choice)
        {
        case '1':
            createAccount();
            break;
        case '2':
            listAccounts();
            break;
        case '3':
            searchAccount();
            break;
        case '4':
            makeDeposit();
            break;
        case '5':
            makeWithdrawal();
            break;
        case '6':
            transferMoney();
            break;
        case '7':
            takeLoan();
            break;
        case '8':
            payDebt();
            break;
        case '9':
            printf("Exiting program.\n");
            return 0;
        default:
            printf("Invalid choice. Try again.\n");
            break;
        }
    }
    return 0;
}

void showMenu()
{
    printf("\nBank Account Management System\n");
    printf("1. Create a new account\n");
    printf("2. List all accounts\n");
    printf("3. Search for an account\n");
    printf("4. Make a deposit\n");
    printf("5. Make a withdrawal\n");
    printf("6. Make a money transfer\n");
    printf("7. Take a loan\n");
    printf("8. Pay the debt\n");
    printf("9. Exit\n");
    printf("Enter your choice: ");
}

char *getAccountNumber()
{
    FILE *file = fopen(FILENAME, "rb");
    if (!file)
    {
        perror("Failed to open file");
        return NULL;
    }

    Account acc;
    char lastAccountNumber[10] = "10";

    while (fread(&acc, sizeof(Account), 1, file))
    {
        if (strcmp(acc.accountNumber, lastAccountNumber) > 0)
            strcpy(lastAccountNumber, acc.accountNumber);
    }
    fclose(file);

    static char newAccountNumber[10];
    int newAccountNum = atoi(lastAccountNumber) + 1;
    sprintf(newAccountNumber, "%d", newAccountNum);
    strcpy(lastAccountNumber, newAccountNumber);
    return newAccountNumber;
}

void createAccount()
{
    int check;
    FILE *file = fopen(FILENAME, "ab");
    if (!file)
    {
        perror("Failed to open file");
        return;
    }

    Account acc;
    memset(&acc, 0, sizeof(Account));
    strcpy(acc.accountNumber, getAccountNumber());

    do
    {
        printf("Enter name: ");
        fgets(acc.name, sizeof(acc.name), stdin);
        if (feof(stdin))
        {
            fclose(file);
            exit(1);
        }
    } while ((check = validateText(acc.name)) == 0);
    if (check == 1)
        clearBuff();

    do
    {
        printf("Enter surname: ");
        fgets(acc.surname, sizeof(acc.surname), stdin);
        if (feof(stdin))
        {
            fclose(file);
            exit(1);
        }
    } while ((check = validateText(acc.surname)) == 0);
    if (check == 1)
        clearBuff();

    do
    {
        printf("Enter address: ");
        fgets(acc.address, sizeof(acc.address), stdin);
        if (feof(stdin))
        {
            fclose(file);
            exit(1);
        }
    } while (strlen(acc.address) <= 1);
    if (!containEndLine(acc.address))
        clearBuff();

    while (true)
    {
        printf("Enter PESEL: ");
        fgets(acc.pesel, sizeof(acc.pesel), stdin);
        if (feof(stdin))
        {
            fclose(file);
            exit(1);
        }
        if ((check = validatePesel(acc.pesel)))
        {
            if (check == 1)
                clearBuff();
            break;
        }
        else
        {
            printf("Invalid PESEL. Please enter an max 11-digit number.\n");
        }
    }
    fwrite(&acc, sizeof(Account), 1, file);
    fclose(file);

    printf("Account created successfully with account number: %s\n", acc.accountNumber);
}

int validatePesel(char *pesel)
{
    if (strlen(pesel) > 11 && strlen(pesel) <= 0)
    {
        return 0;
    }
    return intCheck(pesel);
}

int intCheck(char *number)
{
    for (int selector = 0; selector < strlen(number); selector++)
    {
        if (!isdigit(number[selector]))
        {
            if (number[selector] == '\n' && selector != 0)
            {
                number[selector] = '\0';
                return 2;
            }
            return 0;
        }
    }
    return 1;
}

int validateText(char *text)
{
    for (int selector = 0; selector < strlen(text); selector++)
    {
        if (!(isalpha(text[selector])) && !(text[selector] == ' ' && !(text[selector] == '-')))
        {
            if (text[selector] == '\n' && selector != 0)
            {
                text[selector] = '\0';
                return 2;
            }
            return 0;
        }
    }
    return 1;
}

bool containEndLine(char *text)
{
    for (int selector = 0; selector < strlen(text); selector++)
    {
        if (text[selector] == '\n')
        {
            text[selector] = '\0';
            return true;
        }
    }
    return false;
}

void listAccounts()
{
    FILE *file = fopen(FILENAME, "rb");
    if (!file)
    {
        perror("Failed to open file");
        return;
    }

    Account acc;
    while (fread(&acc, sizeof(Account), 1, file))
    {
        displayAccount(&acc);
    }
    fclose(file);
}

void displayAccount(const Account *acc)
{
    printf("\nAccount Number: %s\n", acc->accountNumber);
    printf("Name: %s\n", acc->name);
    printf("Surname: %s\n", acc->surname);
    printf("Address: %s\n", acc->address);
    printf("PESEL: %s\n", acc->pesel);
    printf("Balance: %.2f\n", acc->balance);
    printf("Loan Balance: %.2f\n", acc->loanBalance);
    printf("Loan Interest: %.2f\n\n", acc->loanInterest);
}

void searchAccount()
{
    int searchOption, check, check2;
    char searchKey[100];

    printf("Search by: 1. Account Number 2. Name 3. Surname 4. Address 5. PESEL\n");
    check = scanf("%d", &searchOption);
    if (feof(stdin))
        exit(1);
    if (check != 1)
    {
        printf("invalid choice");
        clearBuff();
        return;
    }
    clearBuff();
    do
    {
        printf("Enter search value: ");
        fgets(searchKey, sizeof(searchKey), stdin);
        if (feof(stdin))
            exit(1);
    } while (((check = validateText(searchKey)) == 0 && (check2 = validatePesel(searchKey)) == 0));
    if (check == 1 || check2 == 1)
        clearBuff();

    FILE *file = fopen(FILENAME, "rb");
    if (!file)
    {
        perror("Failed to open file");
        return;
    }

    Account acc;
    bool found = false;
    while (fread(&acc, sizeof(Account), 1, file))
    {
        if ((searchOption == 1 && strcmp(acc.accountNumber, searchKey) == 0) ||
            (searchOption == 2 && strcmp(acc.name, searchKey) == 0) ||
            (searchOption == 3 && strcmp(acc.surname, searchKey) == 0) ||
            (searchOption == 4 && strcmp(acc.address, searchKey) == 0) ||
            (searchOption == 5 && strcmp(acc.pesel, searchKey) == 0))
        {
            displayAccount(&acc);
            found = true;
        }
    }
    fclose(file);

    if (!found)
    {
        printf("No matching account found.\n");
    }
}

void makeDeposit()
{
    char accNumber[10];
    double amount;
    int check;
    do
    {
        printf("Enter account number: ");
        fgets(accNumber, sizeof(accNumber), stdin);
        if (feof(stdin))
            exit(1);
    } while ((check = intCheck(accNumber)) == 0);
    if (check == 1)
        clearBuff();

    printf("Enter deposit amount: ");
    check = scanf("%lf", &amount);
    if (feof(stdin))
        exit(1);

    if (check == 1 && amount >= 0)
        amount = floorf(amount * 100) / 100;
    else
    {
        printf("Invalid amount\n");
        clearBuff();
        return;
    }

    clearBuff();
    if (getConfirm() == 0)
        return;
    if (updateAccountBalance(accNumber, amount))
    {
        printf("Deposit successful.\n");
    }
    else
    {
        printf("Account not found.\n");
    }
}

void makeWithdrawal()
{
    char accNumber[10];
    double amount;
    int check;
    do
    {
        printf("Enter account number: ");
        fgets(accNumber, sizeof(accNumber), stdin);
        if (feof(stdin))
            exit(1);
    } while ((check = intCheck(accNumber)) == 0);
    if (check == 1)
        clearBuff();

    printf("Enter withdrawal amount: ");
    check = scanf("%lf", &amount);
    if (feof(stdin))
        exit(1);

    if (check == 1 && amount >= 0)
        amount = floorf(amount * 100) / 100;
    else
    {
        printf("Invalid amount\n");
        clearBuff();
        return;
    }
    clearBuff();

    if (updateAccountBalance(accNumber, -amount))
    {
        if (getConfirm() == 0)
            return;
        printf("Withdrawal successful.\n");
    }
    else
    {
        printf("Account not found or insufficient funds.\n");
    }
}

void transferMoney()
{
    char fromAccNumber[10], toAccNumber[10];
    double amount;
    int check;
    do
    {
        printf("Enter sender account number: ");
        fgets(fromAccNumber, sizeof(fromAccNumber), stdin);
        if (feof(stdin))
            exit(1);
    } while ((check = intCheck(fromAccNumber)) == 0);
    if (check == 1)
        clearBuff();

    do
    {
        printf("Enter receiver account number: ");
        fgets(toAccNumber, sizeof(toAccNumber), stdin);
        if (feof(stdin))
            exit(1);
    } while ((check = intCheck(toAccNumber)) == 0);
    if (check == 1)
        clearBuff();

    printf("Enter transfer amount: ");
    check = scanf("%lf", &amount);
    if (feof(stdin))
        exit(1);

    if (check == 1 && amount >= 0)
        amount = floorf(amount * 100) / 100;
    else
    {
        printf("Invalid amount\n");
        clearBuff();
        return;
    }
    clearBuff();

    if (getConfirm() == 0)
        return;
    if (updateAccountBalance(fromAccNumber, -amount) && updateAccountBalance(toAccNumber, amount))
    {
        printf("Transfer successful.\n");
    }
    else
    {
        printf("Transfer failed. Check account numbers and balances.\n");
    }
}

void takeLoan()
{
    char accNumber[10];
    double loanAmount;
    int check;
    do
    {
        printf("Enter account number: ");
        fgets(accNumber, sizeof(accNumber), stdin);
        if (feof(stdin))
            exit(1);
    } while ((check = intCheck(accNumber)) == 0);
    if (check == 1)
        clearBuff();

    printf("Enter loan amount: ");
    if (feof(stdin))
        exit(1);
    check = scanf("%lf", &loanAmount);

    if (check == 1 && loanAmount >= 0)
        loanAmount = floorf(loanAmount * 100) / 100;
    else
    {
        printf("Invalid amount\n");
        clearBuff();
        return;
    }

    clearBuff();
    if (getConfirm() == 0)
        return;
    Account acc;
    if (readAccount(accNumber, &acc))
    {
        acc.loanBalance += loanAmount;
        acc.loanInterest += loanAmount * INTEREST_RATE;
        acc.balance += loanAmount;

        updateAccount(&acc);
        printf("Loan granted successfully.\n");
    }
    else
    {
        printf("Account not found.\n");
    }
}

void payDebt()
{
    char accNumber[10];
    double amount;
    int check;
    do
    {
        printf("Enter account number: ");
        fgets(accNumber, sizeof(accNumber), stdin);
        if (feof(stdin))
            exit(1);
    } while ((check = intCheck(accNumber)) == 0);
    if (check == 1)
        clearBuff();

    printf("Enter payment amount: ");
    check = scanf("%lf", &amount);
    if (feof(stdin))
        exit(1);
    clearBuff();

    if (check == 1 && amount >= 0)
        amount = floorf(amount * 100) / 100;
    else
    {
        printf("Invalid amount\n");
        return;
    }

    Account acc;
    if (readAccount(accNumber, &acc) && acc.balance >= amount)
    {
        if (getConfirm() == 0)
            return;
        acc.balance -= amount;
        acc.loanBalance -= amount;
        if (acc.loanBalance < 0)
        {
            acc.loanBalance = 0;
            acc.loanInterest = 0;
        }
        else
        {
            acc.loanInterest = acc.loanBalance * INTEREST_RATE;
        }

        updateAccount(&acc);
        printf("Debt payment successful.\n");
    }
    else
    {
        printf("Account not found or insufficient balance.\n");
    }
}

bool updateAccountBalance(const char *accNumber, double amount)
{
    Account acc;
    if (readAccount(accNumber, &acc))
    {
        if (acc.balance + amount >= 0)
        {
            acc.balance += amount;
            updateAccount(&acc);
            return true;
        }
    }
    return false;
}

void saveAccount(const Account *acc)
{
    FILE *file = fopen(FILENAME, "ab");
    if (!file)
    {
        perror("Failed to open file");
        return;
    }
    fwrite(acc, sizeof(Account), 1, file);
    fclose(file);
}

bool readAccount(const char *accNumber, Account *acc)
{
    FILE *file = fopen(FILENAME, "rb");
    if (!file)
    {
        perror("Failed to open file");
        return false;
    }

    bool found = false;
    while (fread(acc, sizeof(Account), 1, file))
    {
        if (strcmp(acc->accountNumber, accNumber) == 0)
        {
            found = true;
            break;
        }
    }
    fclose(file);
    return found;
}

bool updateAccount(const Account *acc)
{
    FILE *file = fopen(FILENAME, "rb");
    if (!file)
    {
        perror("Failed to open file");
        return false;
    }

    FILE *tempFile = fopen("temp.dat", "wb");
    if (!tempFile)
    {
        perror("Failed to open temp file");
        fclose(file);
        return false;
    }

    Account tempAcc;
    bool updated = false;
    while (fread(&tempAcc, sizeof(Account), 1, file))
    {
        if (strcmp(tempAcc.accountNumber, acc->accountNumber) == 0)
        {
            fwrite(acc, sizeof(Account), 1, tempFile);
            updated = true;
        }
        else
        {
            fwrite(&tempAcc, sizeof(Account), 1, tempFile);
        }
    }

    fclose(file);
    fclose(tempFile);

    remove(FILENAME);
    rename("temp.dat", FILENAME);

    return updated;
}

int getConfirm()
{
    char c, ret = 0;
    printf("Confirm? (Y, N)");
    c = getchar();
    if (feof(stdin))
        exit(1);
    if (c == 'y' || c == 'Y')
        ret = 1;
    clearBuff();
    return ret;
}

void clearBuff()
{
    char character;
    while ((character = getchar()) != '\n' && character != EOF)
    {
    }
}

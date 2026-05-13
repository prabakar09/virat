// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACCOUNTS 100
// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
};                        // end structure clientData

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void displayAllRecords(FILE *readPtr);
void searchByLastName(FILE *readPtr);
void resetAllRecords(FILE *fPtr);
void displayTotalBalance(FILE *readPtr);
void displayOverdrawnAccounts(FILE *readPtr);
void transferFunds(FILE *fPtr);

int main(int argc, char *argv[])
{
    FILE *cfPtr;         // file pointer
    unsigned int choice; // user's choice
    const char *fileName = "credit.dat";

    // check if a filename was provided as a command-line argument
    if (argc >= 2) {
        fileName = argv[1];
    }

    // fopen opens the file; exits if file cannot be opened
    if ((cfPtr = fopen(fileName, "rb+")) == NULL)
    {
        // Try creating and initializing the file if it does not exist
        if ((cfPtr = fopen(fileName, "wb+")) == NULL)
        {
            printf("%s: File could not be opened.\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        struct clientData blankClient = {0, "", "", 0.0};
        for (int i = 0; i < MAX_ACCOUNTS; ++i)
        {
            fwrite(&blankClient, sizeof(struct clientData), 1, cfPtr);
        }
        rewind(cfPtr); // go back to the beginning of the file
    }

    // enable user to specify action
    while ((choice = enterChoice()) != 11)
    {
        switch (choice)
        {
        // create text file from record file
        case 1:
            textFile(cfPtr);
            break;
        // update record
        case 2:
            updateRecord(cfPtr);
            break;
        // create record
        case 3:
            newRecord(cfPtr);
            break;
        // delete existing record
        case 4:
            deleteRecord(cfPtr);
            break;
        // display all records to console
        case 5:
            displayAllRecords(cfPtr);
            break;
        // search by last name
        case 6:
            searchByLastName(cfPtr);
            break;
        // reset all records
        case 7:
            resetAllRecords(cfPtr);
            break;
        // display total balance
        case 8:
            displayTotalBalance(cfPtr);
            break;
        // display overdrawn accounts
        case 9:
            displayOverdrawnAccounts(cfPtr);
            break;
        // transfer funds
        case 10:
            transferFunds(cfPtr);
            break;
        // display if user does not select valid choice
        default:
            puts("Incorrect choice");
            break;
        } // end switch
    }     // end while

    fclose(cfPtr); // fclose closes the file
    return 0;      // indicate successful termination
} // end main

// create formatted text file for printing
void textFile(FILE *readPtr)
{
    FILE *writePtr; // accounts.txt file pointer
    int result;     // used to test whether fread read any bytes
    // create clientData with default information
    struct clientData client = {0, "", "", 0.0};

    // fopen opens the file; exits if file cannot be opened
    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
    } // end if
    else
    {
        rewind(readPtr); // sets pointer to beginning of file
        fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

        // copy all records from random-access file into text file
        while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
        {
            // write single record to text file
            if (client.acctNum != 0)
            {
                fprintf(writePtr, "%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName,
                        client.balance);
            } // end if
        }     // end while

        fclose(writePtr); // fclose closes the file
    }                     // end else
} // end function textFile

// update balance in record
void updateRecord(FILE *fPtr)
{
    unsigned int account; // account number
    double transaction;   // transaction amount
    // create clientData with no information
    struct clientData client = {0, "", "", 0.0};

    // obtain number of account to update
    do {
        printf("Enter account to update ( 1 - %d ): ", MAX_ACCOUNTS);
        if (scanf("%u", &account) != 1) {
            while (getchar() != '\n'); // clear buffer
            account = 0;
        }
    } while (account < 1 || account > MAX_ACCOUNTS);

    // move file pointer to correct record in file
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if account does not exist
    if (client.acctNum == 0)
    {
        printf("Account #%u has no information.\n", account);
    }
    else
    { // update record
        printf("%-6u%-16s%-11s%10.2f\n\n", client.acctNum, client.lastName, client.firstName, client.balance);

        // request transaction amount from user
        printf("%s", "Enter charge ( + ) or payment ( - ): ");
        if (scanf("%lf", &transaction) != 1) {
            while (getchar() != '\n'); // clear buffer
            puts("Invalid input. Transaction cancelled.");
            return;
        }
        client.balance += transaction; // update record balance

        printf("%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);

        // move file pointer to correct record in file
        // move back by 1 record length
        fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
        // write updated record over old record in file
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    } // end else
} // end function updateRecord

// delete an existing record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;                       // stores record read from file
    struct clientData blankClient = {0, "", "", 0}; // blank client
    unsigned int accountNum;                        // account number

    // obtain number of account to delete
    do {
        printf("Enter account number to delete ( 1 - %d ): ", MAX_ACCOUNTS);
        if (scanf("%u", &accountNum) != 1) {
            while (getchar() != '\n'); // clear buffer
            accountNum = 0;
        }
    } while (accountNum < 1 || accountNum > MAX_ACCOUNTS);

    // move file pointer to correct record in file
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if record does not exist
    if (client.acctNum == 0)
    {
        printf("Account %u does not exist.\n", accountNum);
    } // end if
    else
    { // delete record
        // move file pointer to correct record in file
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        // replace existing record with blank record
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    } // end else
} // end function deleteRecord

// create and insert record
void newRecord(FILE *fPtr)
{
    // create clientData with default information
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum; // account number

    // obtain number of account to create
    do {
        printf("Enter new account number ( 1 - %d ): ", MAX_ACCOUNTS);
        if (scanf("%u", &accountNum) != 1) {
            while (getchar() != '\n'); // clear buffer
            accountNum = 0;
        }
    } while (accountNum < 1 || accountNum > MAX_ACCOUNTS);

    // move file pointer to correct record in file
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if account already exists
    if (client.acctNum != 0)
    {
        printf("Account #%u already contains information.\n", client.acctNum);
    } // end if
    else
    { // create record
        // user enters last name, first name and balance
        printf("%s", "Enter lastname, firstname, balance\n? ");
        if (scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance) != 3) {
            while (getchar() != '\n'); // clear buffer
            puts("Invalid input. Record not created.");
            return;
        }

        client.acctNum = accountNum;
        // move file pointer to correct record in file
        fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
        // insert record in file
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    } // end else
} // end function newRecord

// enable user to input menu choice
unsigned int enterChoice(void)
{
    unsigned int menuChoice; // variable to store user's choice
    // display available options
    printf("%s", "\nEnter your choice\n"
                 "1 - store a formatted text file of accounts called\n"
                 "    \"accounts.txt\" for printing\n"
                 "2 - update an account\n"
                 "3 - add a new account\n"
                 "4 - delete an account\n"
                 "5 - display all active accounts\n"
                 "6 - search for an account by last name\n"
                 "7 - reset all accounts to zero\n"
                 "8 - display total bank balance\n"
                 "9 - display overdrawn accounts\n"
                 "10 - transfer funds between accounts\n"
                 "11 - end program\n? ");

    if (scanf("%u", &menuChoice) != 1) {
        while (getchar() != '\n'); // clear buffer
        menuChoice = 0; // forces default case in switch
    }
    return menuChoice;
} // end function enterChoice

// display all active records to the console
void displayAllRecords(FILE *readPtr)
{
    struct clientData client = {0, "", "", 0.0};
    
    rewind(readPtr); // sets pointer to beginning of file
    printf("\n%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
    
    while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            printf("%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);
        }
    }
    printf("\n");
}

// search for an account by last name
void searchByLastName(FILE *readPtr)
{
    char searchName[15];
    struct clientData client = {0, "", "", 0.0};
    int found = 0;

    printf("Enter last name to search for: ");
    if (scanf("%14s", searchName) != 1) {
        while (getchar() != '\n');
        puts("Invalid input.");
        return;
    }

    rewind(readPtr); // sets pointer to beginning of file
    printf("\n%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
    {
        if (client.acctNum != 0 && strcmp(client.lastName, searchName) == 0)
        {
            printf("%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);
            found = 1;
        }
    }
    
    if (!found) {
        printf("No accounts found with last name '%s'.\n", searchName);
    }
    printf("\n");
}

// reset all accounts to zero
void resetAllRecords(FILE *fPtr)
{
    struct clientData blankClient = {0, "", "", 0.0};
    char confirm;

    printf("Are you sure you want to reset all accounts? This cannot be undone. (y/n): ");
    if (scanf(" %c", &confirm) != 1) {
        while (getchar() != '\n'); // clear buffer
        puts("Invalid input. Reset cancelled.");
        return;
    }
    
    if (confirm == 'y' || confirm == 'Y') {
        rewind(fPtr);
        for (int i = 0; i < MAX_ACCOUNTS; ++i)
        {
            fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
        }
        puts("All accounts have been reset to zero.");
    } else {
        puts("Reset cancelled.");
    }
}

// display total balance of all accounts
void displayTotalBalance(FILE *readPtr)
{
    struct clientData client = {0, "", "", 0.0};
    double totalBalance = 0.0;
    
    rewind(readPtr);
    while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            totalBalance += client.balance;
        }
    }
    printf("\nTotal balance of all active accounts: %.2f\n\n", totalBalance);
}

// display accounts with a negative balance
void displayOverdrawnAccounts(FILE *readPtr)
{
    struct clientData client = {0, "", "", 0.0};
    int found = 0;
    
    rewind(readPtr);
    printf("\n--- Overdrawn Accounts ---\n");
    printf("%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
    
    while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
    {
        if (client.acctNum != 0 && client.balance < 0)
        {
            printf("%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);
            found = 1;
        }
    }
    
    if (!found) {
        printf("No overdrawn accounts found.\n");
    }
    printf("--------------------------\n\n");
}

// transfer funds between two accounts
void transferFunds(FILE *fPtr)
{
    unsigned int sourceAccount, targetAccount;
    double transferAmount;
    struct clientData sourceClient = {0, "", "", 0.0};
    struct clientData targetClient = {0, "", "", 0.0};

    // get source account
    do {
        printf("Enter source account number ( 1 - %d ): ", MAX_ACCOUNTS);
        if (scanf("%u", &sourceAccount) != 1) {
            while (getchar() != '\n');
            sourceAccount = 0;
        }
    } while (sourceAccount < 1 || sourceAccount > MAX_ACCOUNTS);

    // read source record
    fseek(fPtr, (sourceAccount - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&sourceClient, sizeof(struct clientData), 1, fPtr);

    if (sourceClient.acctNum == 0) {
        printf("Source account #%u has no information.\n", sourceAccount);
        return;
    }

    // get target account
    do {
        printf("Enter target account number ( 1 - %d ): ", MAX_ACCOUNTS);
        if (scanf("%u", &targetAccount) != 1) {
            while (getchar() != '\n');
            targetAccount = 0;
        }
    } while (targetAccount < 1 || targetAccount > MAX_ACCOUNTS);

    if (sourceAccount == targetAccount) {
        puts("Cannot transfer funds to the same account.");
        return;
    }

    // read target record
    fseek(fPtr, (targetAccount - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&targetClient, sizeof(struct clientData), 1, fPtr);

    if (targetClient.acctNum == 0) {
        printf("Target account #%u has no information.\n", targetAccount);
        return;
    }

    // get transfer amount
    printf("Enter transfer amount: ");
    if (scanf("%lf", &transferAmount) != 1) {
        while (getchar() != '\n');
        puts("Invalid input. Transfer cancelled.");
        return;
    }

    if (transferAmount <= 0) {
        puts("Transfer amount must be greater than zero.");
        return;
    }

    if (sourceClient.balance < transferAmount) {
        printf("Warning: Source account has insufficient funds. Overdrafting...\n");
    }

    // update balances
    sourceClient.balance -= transferAmount;
    targetClient.balance += transferAmount;

    // save source record
    fseek(fPtr, (sourceClient.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&sourceClient, sizeof(struct clientData), 1, fPtr);

    // save target record
    fseek(fPtr, (targetClient.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&targetClient, sizeof(struct clientData), 1, fPtr);

    printf("Successfully transferred $%.2f from account #%u to account #%u.\n", 
           transferAmount, sourceAccount, targetAccount);
}
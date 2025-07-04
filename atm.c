// 🔥 FINAL C ATM PROJECT: Nova Edition – Terminal Based, Multi-User, Secure, Logged 🔥

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h> // for getch() on Windows
#include <time.h>

#define MAX_USERS 100
#define USER_FILE "users.txt"

#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

struct User {
    char name[20];
    int pin;
    unsigned long balance;
};

struct User users[MAX_USERS];
int userCount = 0;
int currentUser = -1;

// 🔐 Encrypt/Decrypt simple (Caesar cipher style)
void encrypt(char *str) {
    for (int i = 0; str[i]; i++) str[i] += 3;
}
void decrypt(char *str) {
    for (int i = 0; str[i]; i++) str[i] -= 3;
}

// 🧾 Log transactions
void logTransaction(const char *username, const char *action) {
    char filename[40];
    sprintf(filename, "%s_log.txt", username);
    FILE *fp = fopen(filename, "a");
    if (!fp) return;

    time_t now = time(NULL);
    fprintf(fp, "[%s] %s\n", strtok(ctime(&now), "\n"), action);
    fclose(fp);
}

void viewTransactionLogs(const char *username) {
    char filename[40];
    sprintf(filename, "%s_log.txt", username);
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf(RED "⚠️  No logs found for user %s\n" RESET, username);
        return;
    }
    printf(MAGENTA "\n📜 Transaction History for %s:\n" RESET, username);
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);
}

// 📂 Load users from file
void loadUsers() {
    FILE *fp = fopen(USER_FILE, "r");
    if (!fp) return;

    while (fscanf(fp, "%s %d %lu", users[userCount].name, &users[userCount].pin, &users[userCount].balance) != EOF) {
        decrypt(users[userCount].name);
        userCount++;
    }
    fclose(fp);
}

// 💾 Save users to file
void saveUsers() {
    FILE *fp = fopen(USER_FILE, "w");
    for (int i = 0; i < userCount; i++) {
        char tempName[20];
        strcpy(tempName, users[i].name);
        encrypt(tempName);
        fprintf(fp, "%s %d %lu\n", tempName, users[i].pin, users[i].balance);
    }
    fclose(fp);
}

// 👥 Register user
void registerUser() {
    char uname[20];
    int pin;

    printf(CYAN "📝 Enter your new username: " RESET);
    scanf("%s", uname);

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].name, uname) == 0) {
            printf(RED "❌ Oops! Username already taken. Try again.\n" RESET);
            return;
        }
    }

    printf(CYAN "🔐 Set your 4-digit secret PIN: " RESET);
    scanf("%d", &pin);

    strcpy(users[userCount].name, uname);
    users[userCount].pin = pin;
    users[userCount].balance = 1000; // Default balance
    userCount++;

    saveUsers();
    printf(GREEN "✅ Welcome aboard, %s! Your account is live with Rs 1000 💸\n" RESET, uname);
}

// 🔑 Masked PIN entry
int inputPIN() {
    int pin = 0, digit;
    char ch;
    printf(YELLOW "🔒 Enter 4-digit PIN (hidden): " RESET);
    for (int i = 0; i < 4; i++) {
        ch = getch();
        if (ch >= '0' && ch <= '9') {
            digit = ch - '0';
            pin = pin * 10 + digit;
            printf("*");
        } else {
            i--;
        }
    }
    printf("\n");
    return pin;
}

// 🔓 Login
int login() {
    char uname[20];
    int pin;
    int attempts = 0;

    printf(BLUE "👤 Enter your username: " RESET);
    scanf("%s", uname);

    do {
        pin = inputPIN();
        for (int i = 0; i < userCount; i++) {
            if (strcmp(users[i].name, uname) == 0 && users[i].pin == pin) {
                currentUser = i;
                printf(GREEN "🎉 Access granted. Hello, %s! 💼\n" RESET, users[i].name);
                logTransaction(users[i].name, "Logged In");
                return 1;
            }
        }
        printf(RED "❌ Incorrect PIN. Try again.\n" RESET);
        attempts++;
    } while (attempts < 3);

    printf(RED "🔒 Too many failed attempts. Access blocked.\n" RESET);
    return 0;
}

// 🏦 ATM Menu
void atmMenu() {
    int choice;
    unsigned long amount;
    char action[100];

    do {
        printf("\n" YELLOW "📍 What would you like to do today?\n" RESET);
        printf("1️⃣  Check Balance\n2️⃣  Withdraw Funds\n3️⃣  Deposit Cash\n4️⃣  Change PIN\n5️⃣  View Logs\n6️⃣  Logout\n👉 Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf(CYAN "💰 Current Balance: Rs %lu\n" RESET, users[currentUser].balance);
                break;
            case 2:
                printf("💸 Enter amount to withdraw: ");
                scanf("%lu", &amount);
                if (amount <= users[currentUser].balance) {
                    users[currentUser].balance -= amount;
                    printf(GREEN "✅ Rs %lu withdrawn! Remaining: Rs %lu\n" RESET, amount, users[currentUser].balance);
                    sprintf(action, "Withdrew Rs %lu", amount);
                    logTransaction(users[currentUser].name, action);
                    saveUsers();
                } else {
                    printf(RED "🚫 Insufficient funds.\n" RESET);
                }
                break;
            case 3:
                printf("📥 Enter amount to deposit: ");
                scanf("%lu", &amount);
                users[currentUser].balance += amount;
                printf(GREEN "✅ Rs %lu deposited! New Balance: Rs %lu\n" RESET, amount, users[currentUser].balance);
                sprintf(action, "Deposited Rs %lu", amount);
                logTransaction(users[currentUser].name, action);
                saveUsers();
                break;
            case 4:
                printf("🔁 Set your new 4-digit PIN: ");
                users[currentUser].pin = inputPIN();
                printf(GREEN "🔐 PIN successfully changed. Stay secure!\n" RESET);
                logTransaction(users[currentUser].name, "PIN Changed");
                saveUsers();
                break;
            case 5:
                viewTransactionLogs(users[currentUser].name);
                break;
            case 6:
                logTransaction(users[currentUser].name, "Logged Out");
                printf(BLUE "👋 See you next time, %s! Stay wealthy 💼\n" RESET, users[currentUser].name);
                currentUser = -1;
                return;
            default:
                printf(RED "⚠️ Invalid choice. Try again.\n" RESET);
        }
    } while (1);
}

void showTop3() {
    printf("\n💎 Top 3 Wealthiest Customers:\n");
    for (int i = 0; i < userCount - 1; i++) {
        for (int j = i + 1; j < userCount; j++) {
            if (users[i].balance < users[j].balance) {
                struct User temp = users[i];
                users[i] = users[j];
                users[j] = temp;
            }
        }
    }
    for (int i = 0; i < 3 && i < userCount; i++) {
        printf(GREEN "%d️⃣  %s – Rs %lu\n" RESET, i + 1, users[i].name, users[i].balance);
    }
}

int main() {
    loadUsers();
    int opt;
    char again;

    printf(MAGENTA "\n💻 Welcome to the Ultimate ATM Experience – Nova Edition 🏦✨\n" RESET);

    do {
        printf("\n🚀 Main Menu:\n1️⃣  Login\n2️⃣  Register\n3️⃣  View Top 3\n4️⃣  Exit\n👉 Choose your action: ");
        scanf("%d", &opt);

        switch (opt) {
            case 1:
                if (login()) atmMenu();
                break;
            case 2:
                registerUser();
                break;
            case 3:
                showTop3();
                break;
            case 4:
                printf(YELLOW "🙏 Exiting the ATM. Thanks for stopping by! 💸\n" RESET);
                return 0;
            default:
                printf(RED "❌ Invalid selection. Try again.\n" RESET);
        }
        printf("\n🔁 Back to main menu? (y/n): ");
        scanf(" %c", &again);
    } while (again == 'y' || again == 'Y');

    printf("💤 Session ended. Stay safe and stack that cash! 🤑\n");
    return 0;
}

#include "Store.h"
#include "Bank.h"
#include "User.h" 
#include "DataTypes.h" 


void buyerMenu(Buyer& buyer, Store& store) {
    int choice;
    do {
        std::cout << "\n--- Buyer Menu (" << buyer.getUsername() << " | Bal: " << std::fixed << std::setprecision(2) << buyer.getBankAccount().getBalance() << ") ---\n";
        std::cout << "1. List Bank Features\n";
        std::cout << "2. Purchase Item\n";
        std::cout << "3. List All Orders (Filter)\n";
        std::cout << "4. Check Spending (k days)\n";
        std::cout << "5. Logout\n";
        std::cout << "Enter choice: ";
        if (!(std::cin >> choice)) { clearInputBuffer(); continue; }

        if (choice == 1) {
            int bankChoice;
            std::cout << "\n--- Bank Features ---\n";
            std::cout << "1. Topup\n";
            std::cout << "2. Withdraw\n";
            std::cout << "3. List Cash Flow (Credit/Debit)\n";
            std::cout << "Enter choice: ";
            if (!(std::cin >> bankChoice)) { clearInputBuffer(); continue; }
            
            double amount;
            if (bankChoice == 1) {
                std::cout << "Enter amount to Topup: ";
                if (std::cin >> amount) {
                    if (buyer.getBankAccountRef().deposit(amount, "Manual Topup")) {
                        std::cout << "Topup successful. New Balance: " << std::fixed << std::setprecision(2) << buyer.getBankAccount().getBalance() << "\n";
                    } else {
                        std::cout << "Topup failed.\n";
                    }
                }
            } else if (bankChoice == 2) {
                std::cout << "Enter amount to Withdraw: ";
                if (std::cin >> amount) {
                    if (buyer.getBankAccountRef().withdraw(amount, "Manual Withdrawal")) {
                        std::cout << "Withdrawal successful. New Balance: " << std::fixed << std::setprecision(2) << buyer.getBankAccount().getBalance() << "\n";
                    } else {
                        std::cout << "Withdrawal failed (Insufficient balance or invalid amount).\n";
                    }
                }
            } else if (bankChoice == 3) {
                int rangeChoice, days=0, months=0;
                std::cout << "List flow for (1=Today, 2=Last Month, 3=All): ";
                if (std::cin >> rangeChoice) {
                    if (rangeChoice == 1) days = 1; 
                    else if (rangeChoice == 2) months = 1; 
                    buyer.getBankAccount().listCashFlow(days, months);
                }
            }
        } else if (choice == 2) {
            std::string itemID;
            int quantity;
            std::cout << "Enter Item ID to purchase: ";
            std::cin >> itemID;
            std::cout << "Enter quantity: ";
            if (std::cin >> quantity) {
                store.purchaseItem(buyer, itemID, quantity);
            }
        } else if (choice == 3) {
            int filterChoice;
            std::string filterStr = "ALL";
            std::cout << "Filter by (1.ALL, 2.PAID, 3.COMPLETED, 4.CANCELLED): ";
            if (std::cin >> filterChoice) {
                if (filterChoice == 2) filterStr = "PAID";
                else if (filterChoice == 3) filterStr = "COMPLETED";
                else if (filterChoice == 4) filterStr = "CANCELLED";
                store.listAllOrders(buyer, filterStr);
            }
        } else if (choice == 4) {
            int kDays;
            std::cout << "Check spending for the last k days. Enter k: ";
            if (std::cin >> kDays) {
                double spending = buyer.checkSpending(kDays, Bank::getInstance().allTransactions);
                std::cout << "Total spending in the last " << kDays << " days: " << std::fixed << std::setprecision(2) << spending << "\n";
            }
        }
    } while (choice != 5);
}

void sellerMenu(Seller& seller, Store& store) {
    int choice;
    do {
        std::cout << "\n--- Seller Menu (" << seller.getUsername() << ") ---\n";
        std::cout << "1. Manage Items\n";
        std::cout << "2. Update Transaction Status\n";
        std::cout << "3. Discover Top K Popular Items (per month)\n";
        std::cout << "4. Discover Loyal Customers (per month)\n";
        std::cout << "5. Logout\n";
        std::cout << "Enter choice: ";
        if (!(std::cin >> choice)) { clearInputBuffer(); continue; }

        if (choice == 1) {
            std::cout << "\n--- Item Management ---\n";
            std::cout << "1. Register New Item\n";
            std::cout << "2. Replenish/Discard Stock\n";
            std::cout << "3. Set Price\n";
            std::cout << "Enter choice: ";
            int itemChoice;
            if (!(std::cin >> itemChoice)) { clearInputBuffer(); continue; }

            if (itemChoice == 1) {
                std::string name, id;
                double price;
                int stock;
                std::cout << "Item ID: "; std::cin >> id; clearInputBuffer();
                std::cout << "Item Name: "; std::getline(std::cin, name);
                std::cout << "Price: "; if (!(std::cin >> price)) { clearInputBuffer(); continue; }
                std::cout << "Initial Stock: "; if (!(std::cin >> stock)) { clearInputBuffer(); continue; }
                seller.registerNewItem(Item(id, name, price, stock, seller.getUserID()));
                std::cout << "Item registered.\n";
            } else if (itemChoice == 2) {
                std::string id; int qty, action;
                std::cout << "Item ID: "; std::cin >> id;
                std::cout << "Quantity: "; std::cin >> qty;
                std::cout << "1. Replenish, 2. Discard: "; std::cin >> action;
                if(qty > 0) seller.manageItemStock(id, qty, action == 1);
            } else if (itemChoice == 3) {
                std::string id; double price;
                std::cout << "Item ID: "; std::cin >> id;
                std::cout << "New Price: "; std::cin >> price;
                seller.setItemPrice(id, price);
            }
        } else if (choice == 2) {
            std::string transID; int statusChoice;
            std::cout << "Transaction ID to update: "; std::cin >> transID;
            std::cout << "New Status (1.PAID, 2.COMPLETED, 3.CANCELLED): "; std::cin >> statusChoice;
            if (statusChoice == 2) store.updateTransactionStatus(transID, TransactionStatus::COMPLETED);
            else if (statusChoice == 3) store.updateTransactionStatus(transID, TransactionStatus::CANCELLED);
            else if (statusChoice == 1) store.updateTransactionStatus(transID, TransactionStatus::PAID);
        } else if (choice == 3) {
            int k;
            std::cout << "Enter k for top k items: ";
            if (std::cin >> k) {
                auto topK = seller.discoverTopKItems(k, Bank::getInstance().allTransactions);
                std::cout << "--- Top " << k << " Items for " << seller.getUsername() << " ---\n";
                for (const auto& p : topK) {
                    std::cout << "Item ID: " << p.first << " | Quantity Sold: " << p.second << "\n";
                }
            }
        } else if (choice == 4) {
            int month, year;
            std::cout << "Enter month (1-12): "; if (!(std::cin >> month)) { clearInputBuffer(); continue; }
            std::cout << "Enter year: "; if (!(std::cin >> year)) { clearInputBuffer(); continue; }
            auto loyal = seller.discoverLoyalCustomer(month, year, Bank::getInstance().allTransactions);
            std::cout << "--- Loyal Customers in " << month << "/" << year << " ---\n";
            for (const auto& p : loyal) {
                    std::cout << "Buyer ID: " << p.first << " | Transactions: " << p.second << "\n";
            }
        }
    } while (choice != 5);
}

void storeMenu(Store& store) {
    int choice;
    do {
        std::cout << "\n--- Store Capabilities (Admin View) ---\n";
        std::cout << "1. List Transactions (k days)\n";
        std::cout << "2. List Paid but Incomplete Transactions\n";
        std::cout << "3. List Most Frequent Item Transactions (m)\n";
        std::cout << "4. List Most Active Users (k per day)\n";
        std::cout << "5. Back to Main Menu\n";
        std::cout << "Enter choice: ";
        if (!(std::cin >> choice)) { clearInputBuffer(); continue; }

        if (choice == 1) {
            int k; std::cout << "Enter k days: "; if (std::cin >> k) store.listTransactions(k);
        } else if (choice == 2) {
            store.listPaidButIncomplete();
        } else if (choice == 3) {
            int m; std::cout << "Enter m items: "; if (std::cin >> m) store.listMostFrequentItemTransactions(m);
        } else if (choice == 4) {
            int k, type;
            std::cout << "Enter k users: "; std::cin >> k;
            std::cout << "1. Buyers, 2. Sellers: "; std::cin >> type;
            store.listMostActiveUsers(k, type == 1);
        }
    } while (choice != 5);
}

void bankMenu(Bank& bank) {
    int choice;
    do {
        std::cout << "\n--- Bank Features (Admin View) ---\n";
        std::cout << "1. List Transactions (Last Week)\n";
        std::cout << "2. List All Bank Customers\n";
        std::cout << "3. List Dormant Accounts\n";
        std::cout << "4. List N Top Users (Today)\n";
        std::cout << "5. Back to Main Menu\n";
        std::cout << "Enter choice: ";
        if (!(std::cin >> choice)) { clearInputBuffer(); continue; }

        if (choice == 1) {
            bank.listTransactionsLastWeek();
        } else if (choice == 2) {
            bank.listAllCustomers();
        } else if (choice == 3) {
            int months; std::cout << "Enter months for dormancy: "; if (std::cin >> months) bank.listDormantAccounts(months);
        } else if (choice == 4) {
            int n; std::cout << "Enter N top users: "; if (std::cin >> n) bank.listNTopUsers(n);
        }
    } while (choice != 5);
}


int main() {
    Store& store = Store::getInstance();
    store.load(BUYERS_FILE, SELLERS_FILE);

    int choice;
    do {
        std::cout << "\n========================================\n";
        std::cout << "  ONLINE STORE & BANKING SIMULATOR\n";
        std::cout << "========================================\n";
        std::cout << "1. Login\n";
        std::cout << "2. Register (Buyer/Seller)\n";
        std::cout << "3. Store Capabilities (Admin View)\n";
        std::cout << "4. Bank Features (Admin View)\n";
        std::cout << "5. Exit & Save\n";
        std::cout << "Enter choice: ";
        if (!(std::cin >> choice)) { 
            clearInputBuffer(); 
            continue; 
        }

        if (choice == 1) {
            std::string user, pass;
            std::cout << "Username: "; std::cin >> user;
            std::cout << "Password: "; std::cin >> pass;
            User* loggedInUser = store.login(user, pass);
            if (loggedInUser) {
                std::cout << "Login successful. Welcome, " << loggedInUser->getUsername() << "!\n";
                if (Buyer* buyer = dynamic_cast<Buyer*>(loggedInUser)) {
                    buyerMenu(*buyer, store);
                } else if (Seller* seller = dynamic_cast<Seller*>(loggedInUser)) {
                    sellerMenu(*seller, store);
                }
            } else {
                std::cout << "Login failed. Invalid credentials.\n";
            }
        } else if (choice == 2) {
            std::string user, pass, type, accID;
            double initialBalance;
            std::cout << "Username: "; std::cin >> user;
            std::cout << "Password: "; std::cin >> pass;
            std::cout << "Type (Buyer/Seller): "; std::cin >> type;
            std::cout << "New Bank Account ID: "; std::cin >> accID;
            std::cout << "Initial Bank Balance: "; if (!(std::cin >> initialBalance)) { clearInputBuffer(); continue; }

            if (store.registerUser(user, pass, type, accID, initialBalance)) {
                std::cout << "Registration successful.\n";
            } else {
                std::cout << "Registration failed (Username already taken).\n";
            }
        } else if (choice == 3) {
            storeMenu(store);
        } else if (choice == 4) {
            bankMenu(Bank::getInstance());
        }

    } while (choice != 5);

    store.save(BUYERS_FILE, SELLERS_FILE);
    
    std::cout << "Exiting application. Data saved.\n";
    return 0;
}
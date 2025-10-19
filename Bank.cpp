#include "Bank.h"
#include <iostream>
#include <time.h>      
#include <algorithm>
#include <iomanip>
#include <cmath>

Bank& Bank::getInstance() {
    static Bank instance; 
    return instance;
}

void Bank::addAccount(const BankAccount& account) {
    accounts[account.getOwnerUserID()] = account;
}

BankAccount* Bank::getAccount(const std::string& ownerUserID) {
    auto it = accounts.find(ownerUserID);
    return (it != accounts.end()) ? &(it->second) : nullptr;
}

bool Bank::executeTransaction(const std::string& buyerID, const std::string& sellerID, double amount, const std::string& reason) {
    BankAccount* buyerAcc = getAccount(buyerID);
    BankAccount* sellerAcc = getAccount(sellerID);

    if (!buyerAcc || !sellerAcc || amount <= 0) return false;

    if (buyerAcc->withdraw(amount, "Purchase: " + reason)) {
        sellerAcc->deposit(amount, "Sale: " + reason);
        return true;
    }
    return false;
}

void Bank::addTransactionRecord(const Transaction& transaction) { 
    allTransactions[transaction.getTransactionID()] = transaction; 
}

void Bank::listTransactionsLastWeek() const {
    std::cout << "--- Bank Transactions (Last Week) ---\n";

    time_t now = time(nullptr);
    time_t cutoff = now - (7 * 24 * 3600);

    for (const auto& pair : allTransactions) {
        const auto& trans = pair.second;
        if (stringToTime(trans.getTransactionDate()) >= cutoff) {
            std::cout << "[" << trans.getTransactionDate() << "] ID: " << trans.getTransactionID() 
                      << " | Amount: " << std::fixed << std::setprecision(2) << trans.getTotalAmount() << " | Status: " << statusToString(trans.getStatus()) << "\n";
        }
    }
}

void Bank::listAllCustomers() const {
    std::cout << "--- All Bank Customers ---\n";
    for (const auto& pair : accounts) {
        std::cout << "User ID: " << pair.first << " | Account ID: " << pair.second.getAccountID() 
                  << " | Balance: " << std::fixed << std::setprecision(2) << pair.second.getBalance() << "\n";
    }
}

void Bank::listDormantAccounts(int months) const {
    std::cout << "--- Dormant Accounts (No transaction in " << months << " months) ---\n";
    
    time_t now = time(nullptr);
    time_t cutoff = now - (months * 30 * 24 * 3600); 

    for (const auto& pair : accounts) {
        const auto& acc = pair.second;
        bool isDormant = true;
        
        for (const auto& cf : acc.getCashFlow()) {
            if (stringToTime(cf.first) >= cutoff) {
                isDormant = false;
                break;
            }
        }
        if (isDormant) {
            std::cout << "User ID: " << pair.first << " | Account ID: " << acc.getAccountID() << "\n";
        }
    }
}

void Bank::listNTopUsers(int n) const {
    std::cout << "--- Top " << n << " Users by Transaction Count (Today) ---\n";
    std::map<std::string, int> userTransactionCount;
    std::string today = getCurrentDateTime().substr(0, 10);

    for (const auto& pair : allTransactions) {
        const auto& trans = pair.second;
        if (trans.getTransactionDate().substr(0, 10) == today) {
            userTransactionCount[trans.getBuyerID()]++;
            userTransactionCount[trans.getSellerID()]++;
        }
    }

    std::vector<std::pair<std::string, int>> topUsers(userTransactionCount.begin(), userTransactionCount.end());
    std::sort(topUsers.begin(), topUsers.end(), [](const auto& a, const auto& b){
        return a.second > b.second;
    });

    for (int i = 0; i < std::min((int)topUsers.size(), n); ++i) {
        std::cout << i+1 << ". User ID: " << topUsers[i].first << " | Transactions: " << topUsers[i].second << "\n";
    }
}

void Bank::save(const std::string& filename) const {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        std::cerr << "ERROR: Could not open " << filename << " for saving Bank data.\n";
        return;
    }
    
    ofs << accounts.size() << "\n";
    for (const auto& pair : accounts) {
        pair.second.serialize(ofs);
    }

    ofs << allTransactions.size() << "\n";
    for (const auto& pair : allTransactions) {
        pair.second.serialize(ofs);
    }
    ofs.close();
    std::cout << "Bank data saved successfully to " << filename << ".\n";
}

void Bank::load(const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cerr << "WARNING: Could not open " << filename << " for loading Bank data. Starting fresh.\n";
        return;
    }

    accounts.clear();
    allTransactions.clear();

    size_t count;
    
    if (ifs >> count) {
        ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        for (size_t i = 0; i < count; ++i) {
            BankAccount acc = BankAccount::deserialize(ifs);
            if (!acc.getAccountID().empty()) {
                accounts[acc.getOwnerUserID()] = acc;
            }
        }
    }

    if (ifs >> count) {
        ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        for (size_t i = 0; i < count; ++i) {
            Transaction trans = Transaction::deserialize(ifs);
            if (!trans.getTransactionID().empty()) {
                allTransactions[trans.getTransactionID()] = trans;
            }
        }
    }
    ifs.close();
    std::cout << "Bank data loaded successfully from " << filename << ".\n";
}
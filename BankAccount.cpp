#include "BankAccount.h"

BankAccount::BankAccount() : balance(0.0) {}
BankAccount::BankAccount(const std::string& id, const std::string& ownerId, double initialBalance) 
    : accountID(id), ownerUserID(ownerId), balance(initialBalance) {}

bool BankAccount::deposit(double amount, const std::string& reason) {
    if (amount <= 0) return false;
    balance += amount;
    cashFlow.push_back({getCurrentDateTime(), amount});
    return true;
}

bool BankAccount::withdraw(double amount, const std::string& reason) {
    if (amount <= 0 || balance < amount) return false;
    balance -= amount;
    cashFlow.push_back({getCurrentDateTime(), -amount});
    return true;
}

void BankAccount::listCashFlow(int days, int months) const {
    std::cout << "  Cash Flow for Account " << accountID << ":\n";
    time_t now = time(nullptr);
    time_t cutoff = now - (days * 24 * 3600) - (months * 30 * 24 * 3600); 

    for (const auto& entry : cashFlow) {
        if (stringToTime(entry.first) >= cutoff) {
            std::cout << "  [" << entry.first << "] Amount: " << std::fixed << std::setprecision(2) << entry.second << "\n";
        }
    }
}

void BankAccount::serialize(std::ostream& os) const {
    os << accountID << "|" << ownerUserID << "|" << std::fixed << std::setprecision(2) << balance << "|" << cashFlow.size() << "\n";
    for (const auto& cf : cashFlow) {
        os << cf.first << "|" << std::fixed << std::setprecision(2) << cf.second << "\n";
    }
}

BankAccount BankAccount::deserialize(std::istream& is) {
    BankAccount acc;
    std::string line;
    
    if (std::getline(is, line)) {
        std::stringstream ss(line);
        std::string sBalance, sCount;
        size_t cfCount = 0;
        
        if (std::getline(ss, acc.accountID, '|') &&
            std::getline(ss, acc.ownerUserID, '|') &&
            std::getline(ss, sBalance, '|') &&
            std::getline(ss, sCount)) 
        {
            try {
                acc.balance = std::stod(sBalance);
                cfCount = std::stoul(sCount);
            } catch (...) {
                return BankAccount();
            }
            
            for (size_t i = 0; i < cfCount; ++i) {
                std::string entryLine;
                if (std::getline(is, entryLine)) {
                    std::stringstream entrySS(entryLine);
                    std::string dateTime, sAmount;
                    if (std::getline(entrySS, dateTime, '|') && std::getline(entrySS, sAmount)) {
                        try {
                            acc.cashFlow.push_back({dateTime, std::stod(sAmount)});
                        } catch (...) {
                            std::cerr << "Error deserializing cash flow amount.\n";
                        }
                    }
                }
            }
        }
    }
    return acc;
}
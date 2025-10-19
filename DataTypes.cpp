#include "DataTypes.h"

std::string statusToString(TransactionStatus status) {
    switch (status) {
        case TransactionStatus::PAID: return "PAID";
        case TransactionStatus::COMPLETED: return "COMPLETED";
        case TransactionStatus::CANCELLED: return "CANCELLED";
    }
    return "UNKNOWN";
}

TransactionStatus stringToStatus(const std::string& statusStr) {
    if (statusStr == "PAID") return TransactionStatus::PAID;
    if (statusStr == "COMPLETED") return TransactionStatus::COMPLETED;
    if (statusStr == "CANCELLED") return TransactionStatus::CANCELLED;
    return TransactionStatus::PAID;
}

std::string getCurrentDateTime() {
    time_t now = time(nullptr);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return buf;
}

time_t stringToTime(const std::string& timeStr) {
    std::tm t{};
    std::istringstream ss(timeStr);
    ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) return 0;
    return std::mktime(&t);
}

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
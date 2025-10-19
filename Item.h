#ifndef ITEM_H
#define ITEM_H

#include "DataTypes.h"

class Item {
private:
    std::string itemID;
    std::string name;
    double price;
    int stock;
    std::string sellerID;

public:
    Item();
    Item(const std::string& id, const std::string& name, double price, int stock, const std::string& sellerId);

    std::string getItemID() const { return itemID; }
    std::string getName() const { return name; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }
    std::string getSellerID() const { return sellerID; }

    void setPrice(double newPrice);
    void replenishStock(int quantity);
    bool decreaseStock(int quantity);

    void serialize(std::ostream& os) const;
    static Item deserialize(std::istream& is);
};

#endif // ITEM_H
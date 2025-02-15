#include "user_controller.hpp"

neroshop::UserController::UserController(QObject *parent) : QObject(parent)
{
    // Should user not be initialized until they are logged in or?
    ////user = std::make_unique<neroshop::Seller>();
}

neroshop::UserController::~UserController() {
    std::cout << "user controller deleted\n";
}
//----------------------------------------------------------------
void neroshop::UserController::listProduct(const QString& product_id, int quantity, double price, const QString& currency, const QString& condition, const QString& location) {
    if (!_user)
        throw std::runtime_error("neroshop::User is not initialized");
    auto seller = dynamic_cast<neroshop::Seller *>(_user.get());
    seller->list_item(product_id.toStdString(), quantity, 
        price, currency.toStdString(), condition.toStdString(), location.toStdString());
    emit productsCountChanged();
}
//----------------------------------------------------------------
void neroshop::UserController::delistProduct(const QString& product_id) {
    if (!_user) throw std::runtime_error("neroshop::User is not initialized");
    auto seller = dynamic_cast<neroshop::Seller *>(_user.get());
    seller->delist_item(product_id.toStdString());
    emit productsCountChanged();
}
//----------------------------------------------------------------
void neroshop::UserController::delistProducts(const QStringList& product_ids) {
    if (!_user) throw std::runtime_error("neroshop::User is not initialized");
    auto seller = dynamic_cast<neroshop::Seller *>(_user.get());
    for(const auto& product_id : product_ids) {
        seller->delist_item(product_id.toStdString());//std::cout << product_id.toStdString() << " has been delisted\n";// <- may be "undefined"
    }
    emit productsCountChanged(); // Only emit the signal once we're done delisting all the products
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void neroshop::UserController::addToCart(const QString& product_id, int quantity) {
    if (!_user) throw std::runtime_error("neroshop::User is not initialized");
    _user->add_to_cart(product_id.toStdString(), quantity);
    emit cartQuantityChanged(); // TODO: emit this when removing an item from the cart as well
}
//----------------------------------------------------------------
//void removeFromCart(const QString& product_id, int quantity) {}
//----------------------------------------------------------------
//----------------------------------------------------------------
void neroshop::UserController::rateItem(const QString& product_id, int stars, const QString& comments)
{
    if (!_user) throw std::runtime_error("neroshop::User is not initialized");
    auto seller = dynamic_cast<neroshop::Seller *>(_user.get());

    std::string signature = seller->get_wallet()->sign_message(comments.toStdString(), monero_message_signature_type::SIGN_WITH_SPEND_KEY);
    std::cout << "Signature: \033[33m" << signature << "\033[0m" << std::endl;
    std::string verified = (seller->get_wallet()->verify_message(comments.toStdString(), signature)) ? "true" : "false";
    std::cout << "Is verified: " << ((verified == "true") ? "\033[32m" : "\033[31m") << verified << "\033[0m" << std::endl;
    
    _user->rate_item(product_id.toStdString(), stars, comments.toStdString(), signature);//signature.toStdString());
}
//----------------------------------------------------------------
void neroshop::UserController::rateSeller(const QString& seller_id, int score, const QString& comments)
{
    if (!_user) throw std::runtime_error("neroshop::User is not initialized");
    auto seller = dynamic_cast<neroshop::Seller *>(_user.get());

    std::string signature = seller->get_wallet()->sign_message(comments.toStdString(), monero_message_signature_type::SIGN_WITH_SPEND_KEY);
    std::cout << "Signature: \033[33m" << signature << "\033[0m" << std::endl;
    std::string verified = (seller->get_wallet()->verify_message(comments.toStdString(), signature)) ? "true" : "false";
    std::cout << "Is verified: " << ((verified == "true") ? "\033[32m" : "\033[31m") << verified << "\033[0m" << std::endl;
    
    _user->rate_seller(seller_id.toStdString(), score, comments.toStdString(), signature);//signature.toStdString());
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void neroshop::UserController::uploadAvatar(const QString& filename) {
    if (!_user)
        throw std::runtime_error("neroshop::User is not initialized");
    _user->upload_avatar(filename.toStdString());
}
//----------------------------------------------------------------
bool neroshop::UserController::exportAvatar() {
    if (!_user)
        throw std::runtime_error("neroshop::User is not initialized");
    return _user->export_avatar();
}
//----------------------------------------------------------------
//----------------------------------------------------------------    
neroshop::User * neroshop::UserController::getUser() const {
    return _user.get();
}
//----------------------------------------------------------------
neroshop::Seller * neroshop::UserController::getSeller() const {
    return static_cast<neroshop::Seller *>(_user.get());
}
//----------------------------------------------------------------
QString neroshop::UserController::getID() const {
    if (!_user)
        throw std::runtime_error("neroshop::User is not initialized");
    return QString::fromStdString(_user->get_id());
}
//----------------------------------------------------------------
int neroshop::UserController::getProductsCount() const {
    if (!_user)
        throw std::runtime_error("neroshop::User is not initialized");
    auto seller = dynamic_cast<neroshop::Seller *>(_user.get());
    return seller->get_products_count();
}
//----------------------------------------------------------------
int neroshop::UserController::getReputation() const {
    if (!_user) throw std::runtime_error("neroshop::User is not initialized");
    auto seller = dynamic_cast<neroshop::Seller *>(_user.get());
    return seller->get_reputation();
}
//----------------------------------------------------------------
//----------------------------------------------------------------
int neroshop::UserController::getCartQuantity() const {
    if (!_user) throw std::runtime_error("neroshop::User is not initialized");
    return _user->get_cart()->get_quantity();//(_user->get_id());
}
//----------------------------------------------------------------
//----------------------------------------------------------------
QVariantList neroshop::UserController::getInventory() const {
    if (!_user) throw std::runtime_error("neroshop::User is not initialized");
    neroshop::db::Sqlite3 * database = neroshop::get_database();
    if(!database) throw std::runtime_error("database is NULL");
    
    std::string command = "SELECT DISTINCT * FROM listings JOIN products ON products.uuid = listings.product_id JOIN images ON images.product_id = listings.product_id WHERE seller_id = $1 GROUP BY images.product_id;";
    /*switch(sortType) {
        case InventorySorting::SortNone:
            std::cout << "SortNone (" << SortNone << ") selected\n";
            command = "SELECT DISTINCT * FROM listings JOIN products ON products.uuid = listings.product_id JOIN images ON images.product_id = listings.product_id WHERE seller_id = $1 GROUP BY images.product_id;";
            break;
        case InventorySorting::SortByDateOldest:
            std::cout << "SortByDateOldest (" << SortByDateOldest << ") selected\n";
            command = "SELECT DISTINCT * FROM listings JOIN products ON products.uuid = listings.product_id JOIN images ON images.product_id = listings.product_id WHERE seller_id = $1 GROUP BY images.product_id ORDER BY date ASC;";
            break;
        case InventorySorting::SortByDateRecent:
            std::cout << "SortByDateRecent (" << SortByDateRecent << ") selected\n";
            command = "SELECT DISTINCT * FROM listings JOIN products ON products.uuid = listings.product_id JOIN images ON images.product_id = listings.product_id WHERE seller_id = $1 GROUP BY images.product_id ORDER BY date DESC;";
            break;*/                    
        /*case InventorySorting::SortByName:
            std::cout << "SortByName (" << SortByName << ") selected\n";
            command = "SELECT DISTINCT * FROM listings JOIN products ON products.uuid = listings.product_id JOIN images ON images.product_id = listings.product_id WHERE seller_id = $1 GROUP BY images.product_id;";
            break;*/
        /*case InventorySorting::SortByQuantity:
            std::cout << "SortByQuantity (" << SortByQuantity << ") selected\n";
            command = "SELECT DISTINCT * FROM listings JOIN products ON products.uuid = listings.product_id JOIN images ON images.product_id = listings.product_id WHERE seller_id = $1 GROUP BY images.product_id;";
            break;*/
        /*case InventorySorting::SortByPrice:
            std::cout << "SortByPrice (" << SortByPrice << ") selected\n";
            command = "SELECT DISTINCT * FROM listings JOIN products ON products.uuid = listings.product_id JOIN images ON images.product_id = listings.product_id WHERE seller_id = $1 GROUP BY images.product_id;";
            break;*/
        /*case InventorySorting::SortByProductCode:
            std::cout << "SortByProductCode (" << SortByProductCode << ") selected\n";
            command = "SELECT DISTINCT * FROM listings JOIN products ON products.uuid = listings.product_id JOIN images ON images.product_id = listings.product_id WHERE seller_id = $1 GROUP BY images.product_id;";
            break;*/
        /*case InventorySorting::SortByCategory:
            std::cout << "SortByCategory (" << SortByCategory << ") selected\n";
            command = "SELECT DISTINCT * FROM listings JOIN products ON products.uuid = listings.product_id JOIN images ON images.product_id = listings.product_id WHERE seller_id = $1 GROUP BY images.product_id;";
            break;*/
        /*case InventorySorting::SortByCondition:
            std::cout << "SortByCondition (" << SortByCondition << ") selected\n";
            command = "SELECT DISTINCT * FROM listings JOIN products ON products.uuid = listings.product_id JOIN images ON images.product_id = listings.product_id WHERE seller_id = $1 GROUP BY images.product_id;";
            break;*/
        /*case InventorySorting:: :
            std::cout << "Sort? (" << ? << ") selected\n";
            command = "SELECT DISTINCT * FROM listings JOIN products ON products.uuid = listings.product_id JOIN images ON images.product_id = listings.product_id WHERE seller_id = $1 GROUP BY images.product_id;";
            break;*/
        /*default:
            std::cout << "default: SortNone (" << SortNone << ") selected\n";
            command = "SELECT DISTINCT * FROM listings JOIN products ON products.uuid = listings.product_id JOIN images ON images.product_id = listings.product_id WHERE seller_id = $1 GROUP BY images.product_id;";
            break;            
    }*/
    sqlite3_stmt * stmt = nullptr;
    // Prepare (compile) statement
    if(sqlite3_prepare_v2(database->get_handle(), command.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        neroshop::print("sqlite3_prepare_v2: " + std::string(sqlite3_errmsg(database->get_handle())), 1);
        return {};
    }
    // Bind user_id to TEXT
    std::string user_id = _user->get_id();
    if(sqlite3_bind_text(stmt, 1, user_id.c_str(), user_id.length(), SQLITE_STATIC) != SQLITE_OK) {
        neroshop::print("sqlite3_bind_text (arg: 1): " + std::string(sqlite3_errmsg(database->get_handle())), 1);
        sqlite3_finalize(stmt);
        return {};
    }    
    // Check whether the prepared statement returns no data (for example an UPDATE)
    if(sqlite3_column_count(stmt) == 0) {
        neroshop::print("No data found. Be sure to use an appropriate SELECT statement", 1);
        return {};
    }
    
    QVariantList inventory_array;
    // Get all table values row by row
    while(sqlite3_step(stmt) == SQLITE_ROW) {
        QVariantMap inventory_object; // Create an object for each row
        for(int i = 0; i < sqlite3_column_count(stmt); i++) {
            std::string column_value = (sqlite3_column_text(stmt, i) == nullptr) ? "NULL" : reinterpret_cast<const char *>(sqlite3_column_text(stmt, i));////if(sqlite3_column_text(stmt, i) == nullptr) {throw std::runtime_error("column is NULL");}
            ////std::cout << column_value  << " (" << i << ")" << std::endl;//std::cout << sqlite3_column_name(stmt, i) << std::endl;
            // listings table
            if(i == 0) inventory_object.insert("listing_uuid", QString::fromStdString(column_value));
            if(i == 1) inventory_object.insert("product_id", QString::fromStdString(column_value));
            if(i == 2) inventory_object.insert("seller_id", QString::fromStdString(column_value));
            if(i == 3) inventory_object.insert("quantity", QString::fromStdString(column_value).toInt());
            if(i == 4) inventory_object.insert("price", QString::fromStdString(column_value).toDouble());
            if(i == 5) inventory_object.insert("currency", QString::fromStdString(column_value));
            if(i == 6) inventory_object.insert("condition", QString::fromStdString(column_value));
            if(i == 7) inventory_object.insert("location", QString::fromStdString(column_value));
            if(i == 8) inventory_object.insert("date", QString::fromStdString(column_value));
            // products table
            if(i == 9) inventory_object.insert("product_uuid", QString::fromStdString(column_value)); 
            if(i == 10) inventory_object.insert("product_name", QString::fromStdString(column_value)); 
            if(i == 11) inventory_object.insert("product_description", QString::fromStdString(column_value)); 
            if(i == 12) inventory_object.insert("weight", QString::fromStdString(column_value).toDouble()); 
            if(i == 13) inventory_object.insert("product_attributes", QString::fromStdString(column_value)); 
            if(i == 14) inventory_object.insert("product_code", QString::fromStdString(column_value)); 
            if(i == 15) inventory_object.insert("product_category_id", QString::fromStdString(column_value).toInt()); 
            // images table
            //if(i == 16) inventory_object.insert("image_id", QString::fromStdString(column_value)); 
            //if(i == 17) inventory_object.insert("product_uuid", QString::fromStdString(column_value)); 
            if(i == 18) inventory_object.insert("product_image_file", QString::fromStdString(column_value)); 
            //if(i == 19) inventory_object.insert("product_image_data", QString::fromStdString(column_value));        
        }
        inventory_array.append(inventory_object);
    }
    
    sqlite3_finalize(stmt);

    return inventory_array;    
}
//----------------------------------------------------------------
QVariantList neroshop::UserController::getInventoryInStock() const {
    return {};
}
//----------------------------------------------------------------
QVariantList neroshop::UserController::getInventoryByDate() const {
    if (!_user) throw std::runtime_error("neroshop::User is not initialized");
    neroshop::db::Sqlite3 * database = neroshop::get_database();
    if(!database) throw std::runtime_error("database is NULL");
    
    std::string command = "SELECT DISTINCT * FROM listings JOIN products ON products.uuid = listings.product_id JOIN images ON images.product_id = listings.product_id WHERE seller_id = $1 GROUP BY images.product_id ORDER BY date ASC;";
    sqlite3_stmt * stmt = nullptr;
    // Prepare (compile) statement
    if(sqlite3_prepare_v2(database->get_handle(), command.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        neroshop::print("sqlite3_prepare_v2: " + std::string(sqlite3_errmsg(database->get_handle())), 1);
        return {};
    }
    // Bind user_id to TEXT
    std::string user_id = _user->get_id();
    if(sqlite3_bind_text(stmt, 1, user_id.c_str(), user_id.length(), SQLITE_STATIC) != SQLITE_OK) {
        neroshop::print("sqlite3_bind_text (arg: 1): " + std::string(sqlite3_errmsg(database->get_handle())), 1);
        sqlite3_finalize(stmt);
        return {};
    }    
    // Check whether the prepared statement returns no data (for example an UPDATE)
    if(sqlite3_column_count(stmt) == 0) {
        neroshop::print("No data found. Be sure to use an appropriate SELECT statement", 1);
        return {};
    }
    
    QVariantList inventory_array;
    // Get all table values row by row
    while(sqlite3_step(stmt) == SQLITE_ROW) {
        QVariantMap inventory_object; // Create an object for each row
        for(int i = 0; i < sqlite3_column_count(stmt); i++) {
            std::string column_value = (sqlite3_column_text(stmt, i) == nullptr) ? "NULL" : reinterpret_cast<const char *>(sqlite3_column_text(stmt, i));////if(sqlite3_column_text(stmt, i) == nullptr) {throw std::runtime_error("column is NULL");}
            ////std::cout << column_value  << " (" << i << ")" << std::endl;//std::cout << sqlite3_column_name(stmt, i) << std::endl;
            // listings table
            if(i == 0) inventory_object.insert("listing_uuid", QString::fromStdString(column_value));
            if(i == 1) inventory_object.insert("product_id", QString::fromStdString(column_value));
            if(i == 2) inventory_object.insert("seller_id", QString::fromStdString(column_value));
            if(i == 3) inventory_object.insert("quantity", QString::fromStdString(column_value).toInt());
            if(i == 4) inventory_object.insert("price", QString::fromStdString(column_value).toDouble());
            if(i == 5) inventory_object.insert("currency", QString::fromStdString(column_value));
            if(i == 6) inventory_object.insert("condition", QString::fromStdString(column_value));
            if(i == 7) inventory_object.insert("location", QString::fromStdString(column_value));
            if(i == 8) inventory_object.insert("date", QString::fromStdString(column_value));
            // products table
            if(i == 9) inventory_object.insert("product_uuid", QString::fromStdString(column_value)); 
            if(i == 10) inventory_object.insert("product_name", QString::fromStdString(column_value)); 
            if(i == 11) inventory_object.insert("product_description", QString::fromStdString(column_value)); 
            if(i == 12) inventory_object.insert("weight", QString::fromStdString(column_value).toDouble()); 
            if(i == 13) inventory_object.insert("product_attributes", QString::fromStdString(column_value)); 
            if(i == 14) inventory_object.insert("product_code", QString::fromStdString(column_value)); 
            if(i == 15) inventory_object.insert("product_category_id", QString::fromStdString(column_value).toInt()); 
            // images table
            //if(i == 16) inventory_object.insert("image_id", QString::fromStdString(column_value)); 
            //if(i == 17) inventory_object.insert("product_uuid", QString::fromStdString(column_value)); 
            if(i == 18) inventory_object.insert("product_image_file", QString::fromStdString(column_value)); 
            //if(i == 19) inventory_object.insert("product_image_data", QString::fromStdString(column_value));        
        }
        inventory_array.append(inventory_object);
    }
    
    sqlite3_finalize(stmt);

    return inventory_array;   
}
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
bool neroshop::UserController::isUserLogged() const {
    return (_user.get() != nullptr);
}
//----------------------------------------------------------------
// to allow seller to use user functions: dynamic_cast<Seller *>(user)

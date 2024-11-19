#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <fstream>
#include <queue>

using namespace std;

struct User
{
    string username;
    string password;
    bool isAdmin;

    User() : username(""), password(""), isAdmin(false) {}

    User(string username, string password, bool isAdmin = false)
        : username(username), password(password), isAdmin(isAdmin) {}
};

struct ProductInfo
{
    string name;
    float price;
    string category;
    int stock;
    string description;
    float rating;
};

struct TrieNode
{
    unordered_map<char, TrieNode *> children;
    vector<int> productIds;
    bool isEndOfWord;

    TrieNode()
    {
        isEndOfWord = false;
    }
};

class Trie
{
private:
    TrieNode *root;

public:
    Trie()
    {
        root = new TrieNode();
    }

    void insert(string name, int productId)
    {
        TrieNode *node = root;
        for (char c : name)
        {
            if (node->children.find(c) == node->children.end())
            {
                node->children[c] = new TrieNode();
            }
            node = node->children[c];
        }
        node->isEndOfWord = true;
        node->productIds.push_back(productId);
    }

    vector<int> search(string name)
    {
        TrieNode *node = root;
        for (char c : name)
        {
            if (node->children.find(c) == node->children.end())
            {
                return {};
            }
            node = node->children[c];
        }
        return node->isEndOfWord ? node->productIds : vector<int>{};
    }

    bool deleteProduct(string name, int productId)
    {
        return deleteHelper(root, name, 0, productId);
    }

    bool deleteHelper(TrieNode *node, string name, int depth, int productId)
    {
        if (!node)
            return false;
        if (depth == name.size())
        {
            if (node->isEndOfWord)
            {
                auto it = find(node->productIds.begin(), node->productIds.end(), productId);
                if (it != node->productIds.end())
                {
                    node->productIds.erase(it);
                }
                return node->productIds.empty();
            }
        }
        else
        {
            char c = name[depth];
            if (deleteHelper(node->children[c], name, depth + 1, productId))
            {
                node->children.erase(c);
                return node->children.empty() && !node->isEndOfWord;
            }
        }
        return false;
    }
};

struct Product
{
    int id;
    ProductInfo info;
    Product *next;
    Product *prev;

    Product(int id, string name, float price, string category, int stock, string description, float rating)
    {
        this->id = id;
        this->info.name = name;
        this->info.price = price;
        this->info.category = category;
        this->info.stock = stock;
        this->info.description = description;
        this->info.rating = rating;
        next = prev = nullptr;
    }
};

stack<Product *> undoStack;
struct ProductCompare
{
    bool operator()(const Product *p1, const Product *p2)
    {
        return p1->info.rating < p2->info.rating;
    }
};

class ProductList
{
private:
    Product *head;
    Product *tail;

public:
    ProductList()
    {
        head = tail = nullptr;
    }

    void insertProduct(int id, string name, float price, string category, int stock, string description, float rating)
    {
        Product *newProduct = new Product(id, name, price, category, stock, description, rating);
        if (!head)
        {
            head = tail = newProduct;
        }
        else
        {
            tail->next = newProduct;
            newProduct->prev = tail;
            tail = newProduct;
        }
        undoStack.push(newProduct);
    }

    void displayProducts()
    {
        if (!head)
        {
            cout << "No products in the catalog!" << endl;
            return;
        }
        Product *temp = head;
        while (temp)
        {
            cout << "ID: " << temp->id << " | Name: " << temp->info.name << " | Price: ₹" << temp->info.price
                 << " | Category: " << temp->info.category << " | Stock: " << temp->info.stock
                 << " | Rating: " << temp->info.rating << "/5" << endl;
            cout << "Description: " << temp->info.description << endl;
            cout << endl;
            temp = temp->next;
        }
    }

    void displayByCategory(string category)
    {
        if (!head)
        {
            cout << "No products in the catalog!" << endl;
            return;
        }
        Product *temp = head;
        bool found = false;
        while (temp)
        {
            if (temp->info.category == category)
            {
                cout << "ID: " << temp->id << " | Name: " << temp->info.name << " | Price: ₹" << temp->info.price
                     << " | Stock: " << temp->info.stock << " | Rating: " << temp->info.rating << "/5" << endl;
                cout << "Description: " << temp->info.description << endl;
                cout << endl
                     << endl;
                found = true;
            }
            temp = temp->next;
        }
        if (!found)
        {
            cout << "No products found in category: " << category << endl;
            cout << endl;
        }
    }

    void deleteProduct(int id)
    {
        Product *temp = head;
        while (temp)
        {
            if (temp->id == id)
            {
                undoStack.push(temp);
                if (temp->prev)
                    temp->prev->next = temp->next;
                if (temp->next)
                    temp->next->prev = temp->prev;
                if (temp == head)
                    head = temp->next;
                if (temp == tail)
                    tail = temp->prev;
                delete temp;
                cout << "Product deleted!" << endl;
                return;
            }
            temp = temp->next;
        }
        cout << "Product not found!" << endl;
    }

    void undoLastOperation()
    {
        if (!undoStack.empty())
        {
            Product *lastAction = undoStack.top();
            undoStack.pop();

            if (lastAction->next || lastAction->prev || lastAction == tail)
            {
                cout << "Undoing product deletion: " << lastAction->info.name << endl;

                if (lastAction->prev)
                {
                    lastAction->prev->next = lastAction;
                }
                else
                {
                    head = lastAction;
                }

                if (lastAction->next)
                {
                    lastAction->next->prev = lastAction;
                }
                else
                {
                    tail = lastAction;
                }
            }
            else
            {
                cout << "Undoing product addition: " << lastAction->info.name << endl;

                if (lastAction == tail)
                {
                    tail = lastAction->prev;
                    if (tail)
                        tail->next = nullptr;
                }
                if (lastAction == head)
                {
                    head = lastAction->next;
                    if (head)
                        head->prev = nullptr;
                }

                delete lastAction;
            }
        }
        else
        {
            cout << "Nothing to undo!" << endl;
        }
    }

    void loadProductsFromFile(const string &filename)
    {
        ifstream infile(filename);
        if (!infile)
        {
            ofstream outfile(filename);
            cout << "File not found. Creating new file: " << filename << endl;
            outfile.close();
            return;
        }

        while (true)
        {
            int id, stock;
            float price, rating;
            string name, category, description;

            if (!(infile >> id))
                break;
            infile.ignore();

            getline(infile, name);
            if (name.empty())
                break;

            if (!(infile >> price))
                break;
            infile.ignore();

            getline(infile, category);
            if (category.empty())
                break;

            if (!(infile >> stock))
                break;
            infile.ignore();

            getline(infile, description);
            if (description.empty())
                break;

            if (!(infile >> rating))
                break;
            infile.ignore();

            insertProduct(id, name, price, category, stock, description, rating);
        }

        infile.close();
        cout << "Products loaded from file: " << filename << endl;
    }

    void saveProductsToFile(const string &filename)
    {
        ofstream outfile(filename);
        if (!outfile)
        {
            cout << "Error opening file: " << filename << endl;
            return;
        }
        Product *temp = head;
        while (temp)
        {
            outfile << temp->id << endl;
            outfile << temp->info.name << endl;
            outfile << temp->info.price << endl;
            outfile << temp->info.category << endl;
            outfile << temp->info.stock << endl;
            outfile << temp->info.description << endl;
            outfile << temp->info.rating << endl;
            temp = temp->next;
        }
        outfile.close();
        cout << "Products saved to file: " << filename << endl;
    }

    bool checkProductAvailability(int id)
    {
        Product *temp = head;
        while (temp)
        {
            if (temp->id == id)
            {
                return temp->info.stock > 0;
            }
            temp = temp->next;
        }
        return false;
    }

    void purchaseProduct(int id, int topurchase)
    {
        Product *temp = head;
        while (temp)
        {
            if (temp->id == id)
            {
                if (temp->info.stock - topurchase > 0)
                {

                    temp->info.stock -= topurchase;
                    cout << "Purchased " << temp->info.name << ". Remaining stock: " << temp->info.stock << endl;
                }
                else
                {
                    cout << "Sorry, " << temp->info.name << " is out of stock!" << endl;
                }
                return;
            }
            temp = temp->next;
        }
        cout << "Product not found!" << endl;
    }
    void displayByRating()
    {

        if (!head)
        {
            cout << "No products in the catalog!" << endl;
            return;
        }

        priority_queue<Product *, vector<Product *>, ProductCompare> maxHeap;

        Product *temp = head;
        while (temp)
        {
            maxHeap.push(temp);
            temp = temp->next;
        }

        while (!maxHeap.empty())
        {
            Product *topProduct = maxHeap.top();
            maxHeap.pop();
            cout << "ID: " << topProduct->id << " | Name: " << topProduct->info.name << " | Price: ₹" << topProduct->info.price
                 << " | Category: " << topProduct->info.category << " | Stock: " << topProduct->info.stock
                 << " | Rating: " << topProduct->info.rating << "/5" << endl;
            cout << "Description: " << topProduct->info.description << endl;
            cout << endl
                 << endl;
        }
    }
};

class ECommerceCatalog
{
private:
    ProductList productList;
    Trie productTrie;
    unordered_map<string, User> users;
    string currentUser;

public:
    ECommerceCatalog()
    {
        users["admin"] = User("admin", "admin123", true);
        users["ayush"] = User("ayush", "ayush", true);
        users["rohan"] = User("rohan", "rohan", true);
    }

    void registerUser(string username, string password, bool isAdmin = false)
    {
        users[username] = User(username, password, isAdmin);
        cout << "User registered successfully!" << endl;
    }

    bool login(string username, string password)
    {
        if (users.find(username) != users.end() && users[username].password == password)
        {
            currentUser = username;
            cout << "Logged in as: " << username << endl;
            return true;
        }
        cout << "Invalid username or password!" << endl;
        return false;
    }

    bool isCurrentUserAdmin()
    {
        return users[currentUser].isAdmin;
    }

    void insertProduct(int id, string name, float price, string category, int stock, string description, float rating)
    {
        productList.insertProduct(id, name, price, category, stock, description, rating);
        productTrie.insert(name, id);
    }

    void displayProducts()
    {
        productList.displayProducts();
    }

    void searchProductByName(string name)
    {
        vector<int> productIds = productTrie.search(name);
        if (!productIds.empty())
        {
            cout << "Product(s) found: " << name << " with IDs: ";
            for (int id : productIds)
            {
                cout << id << " ";
            }
            cout << endl;
        }
        else
        {
            cout << "Product not found: " << name << endl;
        }
    }

    void displayProductsByCategory(string category)
    {
        productList.displayByCategory(category);
    }

    void undoLastOperation()
    {
        productList.undoLastOperation();
    }

    void deleteProductById(int id)
    {
        productList.deleteProduct(id);
    }

    void loadProducts(const string &filename)
    {
        productList.loadProductsFromFile(filename);
    }

    void saveProducts(const string &filename)
    {
        productList.saveProductsToFile(filename);
    }

    void purchaseProduct(int id, int topurchase)
    {

        productList.purchaseProduct(id, topurchase);
    }
    void displayByRating()
    {
        productList.displayByRating();
    }
};

void clearscreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pressanykey()
{
    cout << "Press any key to continue...";
    cout.flush();
#ifdef _WIN32
    system("pause >nul");
#else
    system("read -n 1 -s -r -p \"\"");
#endif
    cout << endl;

    clearscreen();
}

void showMenu()
{
    cout << "\n\t\t\t\tE-commerce Product Catalog Menu\n";
    cout << "1. Insert a new product\n";
    cout << "2. Display all products\n";
    cout << "3. Search for a product by name\n";
    cout << "4. Display products by category\n";
    cout << "5. Display products by rating (highest to lowest)\n";
    cout << "6. Delete a product by ID\n";
    cout << "7. Undo last operation\n";
    cout << "8. Purchase a product\n";
    cout << "9. Exit\n";
}

void setupConsole()
{
#ifdef _WIN32
    system("title E-commerce Product Catalog");
    system("cls");
    system("color F0");
#else
    cout << "\033]0;E-commerce Product Catalog\007";
    system("clear");
    cout << "\033[47;30m";

    for (int i = 0; i < 50; ++i)
    {
        cout << string(100, ' ') << endl;
    }
    system("clear");
#endif
}
int main()
{
    ECommerceCatalog catalog;

    string loadFilename = "products.txt";
    string saveFilename = "products.txt";

    catalog.loadProducts(loadFilename);
    setupConsole();
    cout << "\t\t\t\t\t\t" << " ---------------------------" << endl;
    cout << "\t\t\t\t\t\t" << "|                           |" << endl;
    cout << "\t\t\t\t\t\t" << "|        E-COMMERCE         |" << endl;
    cout << "\t\t\t\t\t\t" << "|      PRODUCT CATALOG      |" << endl;
    cout << "\t\t\t\t\t\t" << "|                           |" << endl;
    cout << "\t\t\t\t\t\t" << " ---------------------------" << endl;
    cout << "\t\t\t\t" << "    WELCOME TO THE INTERFACE OF E-COMMERCE PRODUCT CATALOG" << endl
         << endl
         << endl
         << endl
         << endl;

    string username, password;
    cout << "Please login to continue\n";
    while (true)
    {
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;
        if (catalog.login(username, password))
        {
            break;
        }
        cout << "Invalid login, please try again.\n";
    }
    clearscreen();
    int choice, id, stock;
    string name, category, description;
    float price, rating;
    while (true)
    {
        showMenu();
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            int id, stock;
            float price, rating;
            string name, category, description;

            cout << "Enter product ID: ";
            cin >> id;
            cin.ignore();

            cout << "Enter product name: ";
            getline(cin, name);

            cout << "Enter product price: ";
            cin >> price;

            cin.ignore();
            cout << "Enter product category: ";
            getline(cin, category);

            cout << "Enter product stock: ";
            cin >> stock;

            cin.ignore();
            cout << "Enter product description: ";
            getline(cin, description);

            cout << "Enter product rating (0 to 5): ";
            cin >> rating;

            catalog.insertProduct(id, name, price, category, stock, description, rating);
            cout << "Product inserted" << endl;
            pressanykey();
            break;
        }
        case 2:
            catalog.displayProducts();
            cout << endl;
            pressanykey();
            break;
        case 3:
        {
            string name;
            cout << "Enter product name to search: ";
            cin.ignore();
            getline(cin, name);
            catalog.searchProductByName(name);
            pressanykey();
            break;
        }
        case 4:
        {
            string category;
            cout << "Enter category: ";
            cin.ignore();
            getline(cin, category);
            catalog.displayProductsByCategory(category);
            pressanykey();
            break;
        }
        case 5:
        {
            catalog.displayByRating();
            pressanykey();
            break;
        }
        case 6:
        {
            int id;
            cout << "Enter product ID to delete: ";
            cin >> id;
            catalog.deleteProductById(id);
            pressanykey();
            break;
        }
        case 7:
            catalog.undoLastOperation();
            pressanykey();
            break;
        case 8:
        {
            catalog.displayProducts();
            cout << endl;
            int id;
            cout << "Enter product ID to purchase: ";
            cin >> id;
            cout << "Enter Quantity to purchase: ";
            int topurchase;
            cin >> topurchase;
            catalog.purchaseProduct(id, topurchase);
            pressanykey();
            break;
        }

        case 9:
            catalog.saveProducts(saveFilename);
            cout << "Exiting the program and saving data...\n";
            return 0;
        default:
            cout << "Invalid choice! Please try again.\n";
        }
    }

    return 0;
}

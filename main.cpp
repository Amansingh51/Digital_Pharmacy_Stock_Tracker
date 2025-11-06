#include <bits/stdc++.h>
using namespace std;

const string DATA_FILE = "data.txt";
const string TEMP_FILE = "temp.txt";
const char DELIM = '|';
const int LOW_STOCK_THRESHOLD = 10;

struct Medicine {
    string id, name, expiry;
    int quantity = 0;
    double price = 0.0;

    string serialize() const {
        ostringstream oss;
        oss << id << DELIM << name << DELIM
            << quantity << DELIM << fixed << setprecision(2)
            << price << DELIM << expiry;
        return oss.str();
    }
    static bool deserialize(const string &line, Medicine &m) {
        vector<string> p; string cur;
        for (char c : line) {
            if (c == DELIM) { p.push_back(cur); cur.clear(); }
            else cur.push_back(c);
        }
        p.push_back(cur);
        if (p.size() != 5) return false;
        try {
            m.id = p[0]; m.name = p[1];
            m.quantity = stoi(p[2]);
            m.price = stod(p[3]);
            m.expiry = p[4];
        } catch (...) { return false; }
        return true;
    }
};

// Utility functions
static string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

void currentMonthYear(int &mm, int &yy) {
    time_t t = time(nullptr);
    tm *lt = localtime(&t);
    mm = lt->tm_mon + 1; yy = lt->tm_year + 1900;
}

bool parseExpiry(const string &e, int &m, int &y) {
    string s = trim(e);
    int slash = s.find('/');
    if (slash == string::npos) return false;
    try { m = stoi(s.substr(0, slash)); y = stoi(s.substr(slash + 1)); }
    catch (...) { return false; }
    return m >= 1 && m <= 12;
}

bool isExpired(const string &e) {
    int em, ey, cm, cy;
    if (!parseExpiry(e, em, ey)) return false;
    currentMonthYear(cm, cy);
    return (ey < cy) || (ey == cy && em < cm);
}

vector<Medicine> loadAll() {
    ifstream f(DATA_FILE);
    vector<Medicine> v;
    string line;
    while (getline(f, line)) {
        Medicine m;
        if (Medicine::deserialize(trim(line), m)) v.push_back(m);
    }
    return v;
}

bool saveAll(const vector<Medicine> &v) {
    ofstream t(TEMP_FILE);
    for (auto &m : v) t << m.serialize() << "\n";
    t.close();
    remove(DATA_FILE.c_str());
    return rename(TEMP_FILE.c_str(), DATA_FILE.c_str()) == 0;
}

int findIndex(const vector<Medicine> &v, const string &id) {
    for (size_t i = 0; i < v.size(); ++i)
        if (v[i].id == id) return (int)i;
    return -1;
}

string inputLine(const string &p) {
    string s; cout << p; getline(cin, s);
    while (s.empty()) { cout << "Enter again: "; getline(cin, s); }
    return trim(s);
}

int inputInt(const string &p) {
    string s; int x;
    while (true) {
        cout << p; getline(cin, s);
        try { x = stoi(s); return x; } catch (...) { cout << "Invalid.\n"; }
    }
}

double inputDouble(const string &p) {
    string s; double x;
    while (true) {
        cout << p; getline(cin, s);
        try { x = stod(s); return x; } catch (...) { cout << "Invalid.\n"; }
    }
}

// --- Core functions ---
void addMedicine() {
    auto v = loadAll();
    Medicine m;
    m.id = inputLine("ID: ");
    if (findIndex(v, m.id) != -1) { cout << "ID exists!\n"; return; }
    m.name = inputLine("Name: ");
    m.quantity = inputInt("Quantity: ");
    m.price = inputDouble("Price per unit: ");
    m.expiry = inputLine("Expiry (MM/YYYY): ");
    v.push_back(m);
    saveAll(v);
    cout << "Added successfully.\n";
}

void displayStock() {
    auto v = loadAll();
    if (v.empty()) { cout << "No data.\n"; return; }
    double total = 0;
    cout << "\n---------------------------------------------------------------\n";
    cout << left << setw(10) << "ID" << setw(20) << "Name"
         << setw(10) << "Qty" << setw(12) << "Price"
         << setw(12) << "Expiry" << setw(12) << "Status" << "\n";
    cout << "---------------------------------------------------------------\n";
    for (auto &m : v) {
        string st;
        if (isExpired(m.expiry)) st = "Expired";
        else if (m.quantity < LOW_STOCK_THRESHOLD) st = "Low Stock";
        else st = "OK";

        cout << left << setw(10) << m.id << setw(20) << m.name
             << setw(10) << m.quantity << setw(12)
             << fixed << setprecision(2) << m.price
             << setw(12) << m.expiry << setw(12) << st << "\n";

        total += m.price * m.quantity;
    }
    cout << "---------------------------------------------------------------\n";
    cout << "Total Stock Value: Rs. " << total << "\n";
}

void updateStock() {
    auto v = loadAll();
    string id = inputLine("Enter ID to update: ");
    int idx = findIndex(v, id);
    if (idx == -1) { cout << "Not found.\n"; return; }
    v[idx].quantity = inputInt("New quantity: ");
    saveAll(v);
    cout << "Updated.\n";
}

void deleteMedicine() {
    auto v = loadAll();
    string id = inputLine("ID to delete: ");
    int idx = findIndex(v, id);
    if (idx == -1) { cout << "Not found.\n"; return; }
    v.erase(v.begin() + idx);
    saveAll(v);
    cout << "Deleted.\n";
}

void searchMedicine() {
    auto v = loadAll();
    string id = inputLine("Enter ID: ");
    int idx = findIndex(v, id);
    if (idx == -1) { cout << "Not found.\n"; return; }
    auto &m = v[idx];
    cout << "Name: " << m.name << "\nQty: " << m.quantity
         << "\nPrice: " << m.price << "\nExpiry: " << m.expiry
         << "\nStatus: ";
    if (isExpired(m.expiry)) cout << "Expired\n";
    else if (m.quantity < LOW_STOCK_THRESHOLD) cout << "Low Stock\n";
    else cout << "Valid\n";
}

void exportCSV() {
    auto v = loadAll();
    if (v.empty()) { cout << "No data.\n"; return; }

    ofstream f("export.csv");
    time_t now = time(0);
    tm *ltm = localtime(&now);
    f << "Export Date, " << 1900 + ltm->tm_year << "-"
      << 1 + ltm->tm_mon << "-" << ltm->tm_mday << "\n\n";
    f << "ID,Name,Quantity,Price,Expiry,Status\n";

    for (auto &m : v) {
        string st;
        if (isExpired(m.expiry)) st = "Expired";
        else if (m.quantity < LOW_STOCK_THRESHOLD) st = "Low Stock";
        else st = "OK";

        f << m.id << "," << m.name << "," << m.quantity << ","
          << fixed << setprecision(2) << m.price << ","
          << m.expiry << "," << st << "\n";
    }
    f.close();

    cout << "Data exported successfully to export.csv.\n";
    system("start excel export.csv");
}

// --- Main menu ---
int main() {
    while (true) {
        cout << "\n==== DIGITAL PHARMACY STOCK TRACKER ====\n";
        cout << "1. Add Medicine\n2. Display Stock\n3. Search Medicine\n";
        cout << "4. Update Stock\n5. Delete Medicine\n6. Export to CSV\n7. Exit\n";
        cout << "Enter choice: ";

        int c;
        if (!(cin >> c)) { cin.clear(); cin.ignore(10000, '\n'); continue; }
        cin.ignore();

        switch (c) {
            case 1: addMedicine(); break;
            case 2: displayStock(); break;
            case 3: searchMedicine(); break;
            case 4: updateStock(); break;
            case 5: deleteMedicine(); break;
            case 6: exportCSV(); break;
            case 7:
                // Auto Backup Before Exit
                {
                    ifstream src(DATA_FILE, ios::binary);
                    ofstream dst("backup.txt", ios::binary);
                    dst << src.rdbuf();
                    cout << "\nBackup created as backup.txt\n";
                }

                cout << "\n-----------------------------------------\n";
                cout << "Thank you for using Digital Pharmacy Tracker\n";
                cout << "Created by Aman Singh (23BCA10006)\n";
                cout << "-----------------------------------------\n";
                return 0;
            default:
                cout << "Invalid choice.\n";
        }
    }
}

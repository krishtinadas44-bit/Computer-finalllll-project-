#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
using namespace std;

struct Book {
    int id;
    string title, author;
    bool issued;
    int memberId;
};

struct Member {
    int id;
    string name;
    int booksIssued;
};

class User {
protected:
    string name;
public:
    User(string n = "") : name(n) {}
    virtual void panel() = 0;
    virtual ~User() {}
};

class SuperAdmin : public User {
public:
    SuperAdmin() : User("Super Admin") {}
    void panel() override { cout << "Logged in as Super Admin\n"; }
};

class Librarian : public User {
public:
    Librarian() : User("Librarian") {}
    void panel() override { cout << "Logged in as Librarian\n"; }
};

class Visitor : public User {
public:
    Visitor() : User("Visitor") {}
    void panel() override { cout << "Logged in as Visitor\n"; }
};

class Library {
    vector<Book> books;
    vector<Member> members;
    const string bookFile = "library_books.txt";
    const string memberFile = "library_members.txt";

    int findBook(int id) {
        for (int i = 0; i < (int)books.size(); i++)
            if (books[i].id == id) return i;
        return -1;
    }

    int findMember(int id) {
        for (int i = 0; i < (int)members.size(); i++)
            if (members[i].id == id) return i;
        return -1;
    }

    void save() {
        ofstream bf(bookFile), mf(memberFile);
        if (!bf || !mf) {
            cout << "Error: Unable to open data files.\n";
            return;
        }

        for (const Book &b : books)
            bf << b.id << '|' << b.title << '|' << b.author << '|'
               << b.issued << '|' << b.memberId << '\n';

        for (const Member &m : members)
            mf << m.id << '|' << m.name << '|' << m.booksIssued << '\n';
    }

    bool splitBook(const string &line, Book &b) {
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 == string::npos ? 0 : p1 + 1);
        size_t p3 = line.find('|', p2 == string::npos ? 0 : p2 + 1);
        size_t p4 = line.find('|', p3 == string::npos ? 0 : p3 + 1);

        if (p1 == string::npos || p2 == string::npos ||
            p3 == string::npos || p4 == string::npos) return false;

        try {
            b.id = stoi(line.substr(0, p1));
            b.title = line.substr(p1 + 1, p2 - p1 - 1);
            b.author = line.substr(p2 + 1, p3 - p2 - 1);
            b.issued = stoi(line.substr(p3 + 1, p4 - p3 - 1)) != 0;
            b.memberId = stoi(line.substr(p4 + 1));
        }
        catch (...) {
            return false;
        }
        return true;
    }

    bool splitMember(const string &line, Member &m) {
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 == string::npos ? 0 : p1 + 1);
        if (p1 == string::npos || p2 == string::npos) return false;

        try {
            m.id = stoi(line.substr(0, p1));
            m.name = line.substr(p1 + 1, p2 - p1 - 1);
            m.booksIssued = stoi(line.substr(p2 + 1));
        }
        catch (...) {
            return false;
        }
        return true;
    }

    void load() {
        ifstream bf(bookFile), mf(memberFile);
        string line;

        while (getline(bf, line)) {
            Book b;
            if (splitBook(line, b)) books.push_back(b);
        }

        while (getline(mf, line)) {
            Member m;
            if (splitMember(line, m)) members.push_back(m);
        }

        if (books.empty()) {
            books.push_back({1001, "The Alchemist", "Paulo Coelho", false, 0});
            books.push_back({1002, "Wings of Fire", "A.P.J. Abdul Kalam", false, 0});
            books.push_back({1003, "Harry Potter", "J.K. Rowling", false, 0});
            books.push_back({1004, "The Hobbit", "J.R.R. Tolkien", false, 0});
        }

        if (members.empty()) {
            members.push_back({501, "Rahul", 0});
            members.push_back({502, "Ananya", 0});
        }

        save();
    }

    void addBook() {
        Book b;
        b.id = books.empty() ? 1001 : books.back().id + 1;
        b.issued = false;
        b.memberId = 0;

        cout << "Enter title: ";
        getline(cin >> ws, b.title);
        cout << "Enter author: ";
        getline(cin, b.author);

        books.push_back(b);
        save();
        cout << "Book added successfully.\n";
    }

    void listBooks() {
        cout << "\n--- BOOKS ---\n";
        for (const Book &b : books)
            cout << b.id << " | " << b.title << " | " << b.author
                 << " | " << (b.issued ? "Issued" : "Available") << '\n';
    }

    void searchBook() {
        string key;
        cout << "Enter title/author keyword: ";
        getline(cin >> ws, key);

        bool found = false;
        for (const Book &b : books) {
            if (b.title.find(key) != string::npos ||
                b.author.find(key) != string::npos) {
                cout << b.id << " | " << b.title << " | " << b.author
                     << " | " << (b.issued ? "Issued" : "Available") << '\n';
                found = true;
            }
        }
        if (!found) cout << "No matching book found.\n";
    }

    void addMember() {
        Member m;
        m.id = members.empty() ? 501 : members.back().id + 1;
        m.booksIssued = 0;

        cout << "Enter member name: ";
        getline(cin >> ws, m.name);

        members.push_back(m);
        save();
        cout << "Member added. ID: " << m.id << '\n';
    }

    void listMembers() {
        cout << "\n--- MEMBERS ---\n";
        for (const Member &m : members)
            cout << m.id << " | " << m.name
                 << " | Books issued: " << m.booksIssued << '\n';
    }

    bool readNumber(const string &message, int &value) {
        cout << message;
        if (cin >> value) return true;
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input.\n";
        return false;
    }

    void issueBook() {
        int bid, mid;
        if (!readNumber("Enter book ID: ", bid)) return;
        if (!readNumber("Enter member ID: ", mid)) return;

        int b = findBook(bid), m = findMember(mid);
        if (b == -1 || m == -1) {
            cout << "Invalid book or member ID.\n";
            return;
        }
        if (books[b].issued) {
            cout << "Book is already issued.\n";
            return;
        }
        if (members[m].booksIssued >= 3) {
            cout << "Member has reached the 3-book limit.\n";
            return;
        }

        books[b].issued = true;
        books[b].memberId = mid;
        members[m].booksIssued++;
        save();
        cout << "Book issued successfully.\n";
    }

    void returnBook() {
        int bid, mid;
        if (!readNumber("Enter book ID: ", bid)) return;

        int b = findBook(bid);
        if (b == -1) {
            cout << "Book not found.\n";
            return;
        }
        if (!books[b].issued) {
            cout << "Book is not currently issued.\n";
            return;
        }

        if (!readNumber("Enter member ID of the person returning the book: ", mid))
            return;

        if (mid != books[b].memberId) {
            cout << "Member ID does not match the issued record.\n";
            return;
        }

        int m = findMember(mid);
        if (m != -1 && members[m].booksIssued > 0)
            members[m].booksIssued--;

        books[b].issued = false;
        books[b].memberId = 0;
        save();
        cout << "Book returned successfully.\n";
    }

    void report() {
        int available = 0, issued = 0;
        for (const Book &b : books)
            b.issued ? issued++ : available++;

        cout << "\n--- REPORT ---\n";
        cout << "Total books: " << books.size() << '\n';
        cout << "Available: " << available << '\n';
        cout << "Issued: " << issued << '\n';
        cout << "Total members: " << members.size() << '\n';
    }

    void pause() {
        cout << "\nPress Enter to continue...";
        cin.ignore(1000, '\n');
        cin.get();
    }

    void adminMenu() {
        int c;
        do {
            system("cls");
            cout << "===== SUPER ADMIN PANEL =====\n";
            cout << "1. Add Book\n2. List Books\n3. Search Book\n";
            cout << "4. Add Member\n5. List Members\n6. Issue Book\n";
            cout << "7. Return Book\n8. Report\n9. Logout\nChoice: ";

            if (!(cin >> c)) {
                cin.clear();
                cin.ignore(1000, '\n');
                c = 0;
            }

            switch (c) {
                case 1: addBook(); break;
                case 2: listBooks(); break;
                case 3: searchBook(); break;
                case 4: addMember(); break;
                case 5: listMembers(); break;
                case 6: issueBook(); break;
                case 7: returnBook(); break;
                case 8: report(); break;
                case 9: break;
                default: cout << "Invalid choice.\n";
            }

            if (c != 9) pause();
        } while (c != 9);
    }

    void librarianMenu() {
        int c;
        do {
            system("cls");
            cout << "===== LIBRARIAN PANEL =====\n";
            cout << "1. Add Book\n2. List Books\n3. Search Book\n";
            cout << "4. List Members\n5. Issue Book\n6. Return Book\n7. Logout\nChoice: ";

            if (!(cin >> c)) {
                cin.clear();
                cin.ignore(1000, '\n');
                c = 0;
            }

            switch (c) {
                case 1: addBook(); break;
                case 2: listBooks(); break;
                case 3: searchBook(); break;
                case 4: listMembers(); break;
                case 5: issueBook(); break;
                case 6: returnBook(); break;
                case 7: break;
                default: cout << "Invalid choice.\n";
            }

            if (c != 7) pause();
        } while (c != 7);
    }

    void visitorMenu() {
        int c;
        do {
            system("cls");
            cout << "===== VISITOR PANEL =====\n";
            cout << "1. List Available Books\n2. Search Book\n";
            cout << "3. Exit\nChoice: ";

            if (!(cin >> c)) {
                cin.clear();
                cin.ignore(1000, '\n');
                c = 0;
            }

            switch (c) {
                case 1:
                    cout << "\n--- AVAILABLE BOOKS ---\n";
                    for (const Book &b : books)
                        if (!b.issued)
                            cout << b.id << " | " << b.title
                                 << " | " << b.author << '\n';
                    break;
                case 2: searchBook(); break;
                case 3: break;
                default: cout << "Invalid choice.\n";
            }

            if (c != 3) pause();
        } while (c != 3);
    }

    void login() {
        int role, pass;

        do {
            system("cls");
            cout << "===== LIBRARY MANAGEMENT SYSTEM =====\n";
            cout << "1. Super Admin\n2. Librarian\n3. Visitor\n4. Exit\nChoice: ";

            if (!(cin >> role)) {
                cin.clear();
                cin.ignore(1000, '\n');
                role = 0;
            }

            User *user = nullptr;

            if (role == 1) {
                if (!readNumber("Password: ", pass)) continue;
                if (pass != 123) {
                    cout << "Wrong password.\n";
                    pause();
                    continue;
                }
                user = new SuperAdmin();
                user->panel();
                delete user;
                adminMenu();
            }
            else if (role == 2) {
                if (!readNumber("Password: ", pass)) continue;
                if (pass != 456) {
                    cout << "Wrong password.\n";
                    pause();
                    continue;
                }
                user = new Librarian();
                user->panel();
                delete user;
                librarianMenu();
            }
            else if (role == 3) {
                user = new Visitor();
                user->panel();
                delete user;
                visitorMenu();
            }
            else if (role != 4) {
                cout << "Invalid choice.\n";
                pause();
            }
        } while (role != 4);
    }

public:
    Library() { load(); }
    ~Library() { save(); }

    void run() { login(); }
};

int main() {
    Library library;
    library.run();
    return 0;
}

#include <iostream>
#include <filesystem>
#include <chrono>
#include <iomanip>

namespace fs = std::filesystem;
using namespace std;

void print_attributes(const fs::directory_entry& entry) {
    auto path = entry.path();
    cout << "Path: " << path << '\n';

    if (entry.is_regular_file()) {
        cout << "Type: File\n";
    } else if (entry.is_directory()) {
        cout << "Type: Directory\n";
    } else {
        cout << "Type: Other\n";
    }

    auto perms = entry.status().permissions();
    cout << "Permissions: ";
    cout << ((perms & fs::perms::owner_read) != fs::perms::none ? "r" : "-")
         << ((perms & fs::perms::owner_write) != fs::perms::none ? "w" : "-")
         << ((perms & fs::perms::owner_exec) != fs::perms::none ? "x" : "-")
         << '\n';

    auto ftime = fs::last_write_time(entry);
    auto sctp = chrono::system_clock::now() + (ftime - fs::file_time_type::clock::now());
    time_t cftime = chrono::system_clock::to_time_t(sctp);
    cout << "Last modification: " << asctime(localtime(&cftime));

    cout << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: dirsearch <search_name>\n";
        return 1;
    }

    string search_name = argv[1];
    fs::path start_path = fs::current_path();

    try {
        for (const auto& entry : fs::recursive_directory_iterator(start_path)) {
            if (entry.path().filename() == search_name) {
                print_attributes(entry);
            }
        }
    } catch (const fs::filesystem_error& e) {
        cout << e.what() << '\n';
    }

    return 0;
}

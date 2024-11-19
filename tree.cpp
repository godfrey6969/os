#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;
using namespace std;

void printTree(fs::path& path, string& prefix, bool showAll, bool directoriesOnly, bool fullPath) {
    for (auto& entry : fs::directory_iterator(path)) {
        if (!showAll && entry.path().filename().string().front() == '.') {
            continue;
        }
        if (directoriesOnly && !fs::is_directory(entry.status())) {
            continue;
        }
        cout << prefix;
        cout << (fs::is_directory(entry.status()) ? "├── " : "├── ");
        cout << (fullPath ? entry.path().string() : entry.path().filename().string()) << endl;
        if (fs::is_directory(entry.status())) {
            printTree(entry.path(), prefix + "│   ", showAll, directoriesOnly, fullPath);
        }
    }
}
int main(int argc, char* argv[]) {
    bool showAll = false;
    bool directoriesOnly = false;
    bool fullPath = false;
    fs::path rootPath = ".";

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-a") {
            showAll = true;
        } else if (arg == "-d") {
            directoriesOnly = true;
        } else if (arg == "-f") {
            fullPath = true;
        } else {
            rootPath = arg;
        }
    }

    if (!fs::exists(rootPath) || !fs::is_directory(rootPath)) {
        cerr << "Error: Invalid directory path." << endl;
        return 1;
    }

    cout << rootPath.string() << endl;
    printTree(rootPath, "", showAll, directoriesOnly, fullPath);

    return 0;
}
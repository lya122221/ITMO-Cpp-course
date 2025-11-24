#include "argparser.h"
#include "archiveoperation.h"
#include "filesoperation.h"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, const char* argv[]) {
    nargparse::ArgumentParser parser = nargparse::CreateParser("parser");

    bool create_flag = false;
    nargparse::AddFlag(parser, "-c", "--create", &create_flag, "Create archive");
    
    bool list_flag = false;
    nargparse::AddFlag(parser, "-l", "--list", &list_flag, "List archive files");
    
    bool extract_flag = false;
    nargparse::AddFlag(parser, "-x", "--extract", &extract_flag, "Extract files");
    
    bool append_flag = false;
    nargparse::AddFlag(parser, "-a", "--append", &append_flag, "Append file");
    
    bool delete_flag = false;
    nargparse::AddFlag(parser, "-d", "--delete", &delete_flag, "Delete file");
    
    bool concatenate_flag = false;
    nargparse::AddFlag(parser, "-A", "--concatenate", &concatenate_flag, "Concatenate archives");

    char arch_name[128] = {};
    nargparse::AddArgument(parser, "-f", "--file", &arch_name, "Archive name", nargparse::kNargsRequired);

    char files_name[128] = {};
    nargparse::AddArgument(parser, &files_name, "Files name", nargparse::kNargsZeroOrMore);

    if (!nargparse::Parse(parser, argc, argv)) {
        std::cerr << "Failed to parse arguments" << std::endl;
        return -1;
    }

    int files_count = nargparse::GetRepeatedCount(parser, "Files name");

    std::vector<std::string> files(files_count);
    for (int i = 0; i < files_count; i++) {
        char* curr_str = nullptr;
        nargparse::GetRepeated(parser, "Files name", i, &curr_str);
        files[i] = std::string(curr_str);
    }

    if (create_flag) {
        CreateArchive(arch_name, files);
    } else if (list_flag) {
        ListFiles(arch_name);
    } else if (extract_flag) {
        if (files_count == 0) {
            ExtractAll(arch_name);
        } else {
            ExtractFiles(arch_name, files);
        }
    } else if (append_flag && files_count == 1) {
        AppendFile(arch_name, files[0]);
    } else if (delete_flag && files_count == 1) {
        DeleteFile(arch_name, files[0]);
    } else if (concatenate_flag && files_count == 2) {
        MergeArchives(files[0], files[1], arch_name);
    } else {
        nargparse::AddHelp(parser);
        nargparse::PrintHelp(parser);
    }

    nargparse::FreeParser(parser);

    return 0;
}
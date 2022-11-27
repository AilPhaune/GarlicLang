#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include "Utils.h"
#include "Tokens.h"
#include "Parser.h"
#include <cstdio>
#include <cstring>

#define GARLIC_COMPILER_CURRENT_VERSION "v1.0dev"

using namespace std;

static const vector<pair<string, string>> commands = {
    {"o", "Specifies the output file"},
    {"type", "Specifies the output type"},
    {"-help", "Displays this information"},
    {"-version", "Shows the current build version"}
};

string generate_spaces(int len) {
    char* chr = new char[len+1];
    memset(chr, ' ', len);
    chr[len] = '\0';
    string spaces = chr;
    delete[] chr;
    return spaces;
}

#define TARGET_SPACE_LENGTH 30
void print_help_cmd(const char* arg) {
    cout << "Garlic: Usage: " << arg << " [commands] <input files>" << endl;
    cout << "Commands: " << endl;
    auto iterator = commands.begin();
    while(iterator != commands.end()) {
        cout << "\t-" << iterator->first << generate_spaces(TARGET_SPACE_LENGTH - iterator->first.size()) << iterator->second << endl;
        iterator++;
    }
}

struct Arguments {
    string output_file;
    string compile_type;
    vector<string> input_files;        // file with embedded garlic (more or less like php files)
    vector<string> input_raw_files;    // raw garlic code
};

#define STATE_NORMAL    0
#define STATE_WAIT_ARG  1

int main(int argc, const char * const *argv) {
    if(argc <= 1) {
        cout << "[FATAL] Garlic: Invalid arguments." << endl;
        print_help_cmd(argv[0]);
        return 0;
    }

    Arguments args;
    int state = STATE_NORMAL;
    string arg, wait_arg;
    for(int i = 1; i < argc; i++) {
        arg = argv[i];
        if(state == STATE_WAIT_ARG) {
            state = STATE_NORMAL;
            if(wait_arg == "-o") {
                args.output_file = argv[i];
                continue;
            }
            if(wait_arg == "-type") {
                args.compile_type = argv[i];
                continue;
            }
            cout << "[FATAL] Garlic: Unknown command: " << wait_arg << endl;
            return -1;
        }
        if(arg.size() >= 2 && arg[0] == '-' && arg[1] == '-') {
            if(arg == "--version") {
                cout << "Garlic compiler " << GARLIC_COMPILER_CURRENT_VERSION << endl;
                return 0;
            }
            if(arg == "--help") {
                print_help_cmd(argv[0]);
                return 0;
            }
            continue;
        }
        if(arg.size() >= 1 && arg[0] == '-') {
            if(arg.size() >= 5 && arg.substr(0, 5) == "-raw=") {
                if(arg.size() == 5) {
                    cout << "[FATAL] Garlic: Empty file name !" << endl;
                    return -1;
                }
                string file = arg.substr(5);
                args.input_raw_files.push_back(file);
                continue;
            }
            wait_arg = arg;
            state = STATE_WAIT_ARG;
            continue;
        }
        
        args.input_files.push_back(arg);
    }
    if(state == STATE_WAIT_ARG) {
        cout << "[FATAL] Garlic: No value provided to last argument !" << endl;
        return -1;
    }

    if(args.input_files.size() == 0 && args.input_raw_files.size() == 0) {
        cout << "[FATAL] Garlic: no input files provided" << endl;
        return -1;
    }

    if(args.output_file.size() == 0) {
        cout << "[FATAL] Garlic: no output file provided" << endl;
        return -1;
    }

    vector<shared_ptr<GParsingResult>> files;
    size_t normal_files = args.input_files.size();
    size_t total_files = args.input_files.size() + args.input_raw_files.size();
    for(size_t i = 0; i < total_files; i++) {
        string inputfile = (i >= normal_files) ? args.input_raw_files.at(i - normal_files) : args.input_files.at(i);
        // read content from file
        FILE *file = fopen(inputfile.c_str(), "rb");
        fseek(file, 0, SEEK_END);
        auto size = ftell(file);
        char* content = new char[size];
        fseek(file, 0, SEEK_SET);
        fread(content, 1, size, file);
        fclose(file);

        /* DEBUG: cout << "File content:" << content << endl;*/
        
        // generate tokens from file
        GLexer lexer(inputfile, content, size);
        shared_ptr<GLexingResult> lexed = (i >= normal_files) ? lexer.makeRawTokens() : lexer.makeTokens();

        if (lexed->hasError()) {
            cout << lexed->getError()->toString() << endl;
            return -2;
        }
        
        vector<shared_ptr<GToken>> tokens = lexed->getTokens();
        // /* DEBUG:
        size_t it = 0;
        cout << "=== BEGIN TOKENS ===" << endl << "number of tokens: " << tokens.size() << endl;
        while(it < tokens.size()) {
            cout << tokens[it]->value << endl;
            it++;
        }
        cout << "=== END TOKENS ===" << endl;
        
//      */

        // parse the file
        GParser parser(tokens);
        shared_ptr<GParsingResult> ast = parser.parseAST();
        if(ast->error) {
            cout << ast->error->toString() << endl;
            return -2;
        }
        ast->source = inputfile;
        files.push_back(ast);

        cout << GNode::toString(ast->node->prettyPrint(), 4) << endl << endl << endl;

        // release memory
        delete[] content;
    }
}
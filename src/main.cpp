#include <cstdlib>
#include <cstdio>

#include <span>
#include <vector>
#include <string>
#include <string_view>
#include <fstream>

#include "cbor/CBOR.h"
#include "Functions.h"

namespace
{
std::vector<uint8_t> parseBytesOrLoadFile(std::string_view arg)
{
    if (arg.starts_with("0x"))
    {
        return Bytes::parseBytes(arg.substr(2));
    }
    
    std::ifstream file(arg, std::ios::binary);
    file.seekg (0, std::ios::end);
    const auto length = file.tellg();
    file.seekg (0, std::ios::beg);

    std::vector<uint8_t> out(length);
    file.read((char*)out.data(), length);

    return out;
}

int help()
{
    return EXIT_SUCCESS;
}
} // namespace

int main(int argc, const char* argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <command> [options...] <input> [output]\n", argv[0]);
        printf("Call %s --help\n", argv[0]);

        return EXIT_FAILURE;
    }

    const std::string_view command = argv[1];
    const std::vector<std::string_view> args(argv + 2, argv + argc);
    
    if (command == "--help")
    {
        return help();
    }
    else if (command == "--encode" || command == "-e")
    {
    }
    else if (command == "--decode" || command == "-d")
    {
        if (args.size() < 1)
        {
            printf("Usage: borontool --decode [--packed] <INPUT>\n");
            return EXIT_FAILURE;
        }

        bool packed = false;
        if (args.size() == 2 && (args[0] == "--packed" || args[0] == "-p"))
        {
            packed = true;
        }

        const auto bytes = parseBytesOrLoadFile(args.back());
        const auto [error, str] = Boron::decode(bytes, packed ? Boron::StringFormat::PACKED : Boron::StringFormat::INDENTED);
        if (error != CBOR::Error::OK)
        {
            printf("Error: %s\n", CBOR::toString(error));
            return (int)error;
        }
        else
        {
            printf("%s\n", str.c_str());
        }
    }
    else
    {
        printf("command not recognized (%.*s)\n", (int)command.size(), command.data());
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
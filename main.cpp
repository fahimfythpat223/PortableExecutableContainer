#include <iostream> // for terminal input-output streams
#include <vector> // for CommandInterface class
#include <unordered_map> // for CommandInterfaceClass
#include <functional> // for CommandInterfaceClass
#include "headers/global.hpp" // global constants, structures, classes and objects
#include "headers/util.hpp" // miscellaenous functions not associated with a class


/*

This is the standard disk layout for a disk with 10 megabytes of usable storage

block 0 - superblock (block count: 1)
block 1 - 127 - inode table  (block count: 127)
block 128 - 132 block bitmap (block count: 5)
block 133 - inode bitmap (block count: 1)
block 134 - 20, 613 - data region (block count: 20,480)

*/







class CommandInterface
{
    private:

        std::unordered_map<std::string, std::function<void (const std::vector<std::string>&)>> DispatchTable;

        DiskWriterClass DiskWriter;
        DiskParserClass DiskParser;

    public:

        CommandInterface()
        {
            DispatchTable["write-superblock"] = [this](std::vector<std::string> args){this->DiskWriter.WriteSuperBlock();};
            DispatchTable["init"] = [this](std::vector<std::string> args){this->DiskWriter.InitEmptyDisk();};
            DispatchTable["read"] = [this](std::vector<std::string> args){this->DiskParser.ReadDisk();};
            DispatchTable["dump"] = [](std::vector<std::string> args){DumpSpecificBlock(args);};
            DispatchTable["test-allocate"] = [](std::vector<std::string> args){AllocateBlock();};
            DispatchTable["test-mount"] = [](std::vector<std::string> args){TestMount();};
        }

        void CommandDispatch(const std::vector<std::string>& args)
        {
            if(DispatchTable.find(args[1]) != DispatchTable.end()) // 4: checks if command exists or not. If yes, calls function with args
            {
                DispatchTable[args[1]](args);
            }

            else
            {
                std::cerr << "CommandDispatch error: invalid command\n";
                std::exit(1);
            }
        }
};




int main(int argc, char* argv[])
{
    std::vector<std::string> args(argv, argv + argc);

    try
    {
        MountedDisk = std::make_unique<MountedDiskClass>(); // mounts the disk.
    }

    catch(std::exception& e) // in the event of mount errors.
    {
        std::cerr << e.what();
        MountedDisk.reset(); // dismounts the disk
        std::exit(1);
    }

    CommandInterface CommandHandler;
    CommandHandler.CommandDispatch(args);

    return 0;
}
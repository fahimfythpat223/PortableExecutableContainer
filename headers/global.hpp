#pragma once

/*

reminder: any headers used in source files (in this case, global.cpp) must be declared HERE. NOT the source file.
thank you for your time.

*/

#include <cstdint> // for fixed-width integers
#include <vector> // for vectors
#include <memory> // for smart pointers 
#include <fstream> // for file reading/writing
#include <iostream> // for command line I/O streams
#include <cstring> // for memory related functions like memset() and memcpy()


/*

This header defines all global constant and POD structs, and declares global objects and all classes and their members. Their
implementation will be in global.cpp. 

To avoid unnecessarily complicated project structure, every entity in this folder is ordered by dependencies. If one entity
depends on another to function, then the needed entity, the "dependency" must be declared first. Meaning that the entities are
ordered from less-or-no-dependencies to more-dependencies.

*/





/*

constexpr is a special case where you don't need to separate declaration and implementation. They are defined here, in the
global header. They only depend on the cstdint header. So they go first.

*/

constexpr std::uint64_t BLOCK_SIZE = 512; // constexpr makes the code more optimized in most cases

constexpr std::uint64_t BLOCK_NUMBER = 20615; // 10 MB is the base size as of now

constexpr std::uint32_t MAGIC = 0xFEAB1E33; // the magic number of my custom file-system

constexpr bool DEBUG_FLAG = true; // switch to false before creating release builds

/*

The POD structs only depend on the cstdint header for fixed-width integers. Other then that, some depend on constants to
function, so they come after constants.

*/


/*

If you don't want the compiler sonovabitch to add internal padding to binary structures for optimization, the use the pack
instructions below.

*/

#pragma pack(push, 1) // packs the struct so compiler doesn't throw a fit.

struct SuperBlock // size = 54 bytes. All fields are initialized with zero for testing. Remove later when fields are filled
{
    std::uint64_t inode_table_block_count; // 8 bytes | offset 0
    std::uint64_t inode_table_block_start; // 8 byte | offset 8
    std::uint64_t block_bitmap_block_start; // 8 bytes | offset 16
    std::uint64_t block_bitmap_block_count; // 8 bytes | offset 24
    std::uint64_t inode_bitmap_block_start; // 8 bytes | offset 32
    std::uint64_t inode_bitmap_block_count; // 8 bytes | offset 40
    std::uint64_t data_region_block_start; // 8 bytes | offset 48
    std::uint64_t data_region_block_count; // 8 bytes | offset 56
    std::uint32_t magic; // 4 bytes | offset 64
    std::uint8_t version_major; // 1 byte | offset 68
    std::uint8_t version_minor; // 1 byte | offset 69
    std::uint8_t padding[442]{}; // 442 bytes | offset 70
};

#pragma pack(pop) // this line is important as it stops packing lines after you put this instruction in



struct Block // struct for reading/writing/processing any block.
{
    std::uint8_t data[BLOCK_SIZE];
};



struct Inode // standard size 64-bytes as of now. non-padded size = 40
{
    std::uint32_t index; // 4 bytes
    std::uint32_t file_size; // 4 bytes
    std::uint32_t block_pointers[8]; // (4 x 8) = 32 bytes
    std::uint8_t padding[24]{};
};



/*

MountedDiskClass depends on POD structures (SuperBlock as of now) to function, so it is declared third, after declaring
POD structs

*/


class MountedDiskClass
{
    public:

        std::string filename; // assigned value in constructor
        SuperBlock superblock;                           
        std::vector<std::uint8_t> bitmap;
        std::fstream DiskFile;                  

        MountedDiskClass(); // the constructor is responsible for mounting the disk
        ~MountedDiskClass(); // the destructor is responsible for dismounting the disk
};





/*

The global unique pointer is put AFTER the MountedDiskClass declaration, as it depends on MountedDiskClass to function, and
also because it's related to the class, so grouping them together makes sense. But if there are more global objects later,
I might reconsider. Also, you MUST declare global objects here with extern and define them in "global.cpp". 

*/

extern std::unique_ptr<MountedDiskClass> MountedDisk;// this pointer will be the only way all other modules will access the Disk

/*

since it's a global object, the header ("global.hpp") declares it, the source file ("global.cpp") defines it, and the main
function essentially kickstarts it by assigning a newly instantiated MountedDisk to it.

*/








/*

I may or may not dissolve the classes and make the methods utility functions later. The declarations themselves don't depend on
others to function, but the definitions depend on MountedDiskClass and maybe MountedDisk (the global unique pointer)

*/

class DiskWriterClass
{
    public:

        void InitEmptyDisk(); // creates an empty ".disk" file of BLOCK_NUMBER multiplied by BLOCK_SIZE amount of bytes
        void WriteSuperBlock(); // writes the superblock onto the newly instantiated ".disk" file.
};



class DiskParserClass
{
    public:

        void ReadDisk(); // loads the superblock into memory and gives high-level overview of all fields. Used for debugging
};
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>

constexpr std::uint32_t MAGIC = 0xFEAB1E33;

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

int main()
{
    std::ofstream Disk("fresh-floppy.disk", std::ios::out | std::ios::binary);

    if(!Disk.is_open())
    {
        std::cerr << "WriteSuperBlock error: could not open disk for writing superblock\n";
        std::exit(1);
    }

    SuperBlock superblock{127, 1, 128, 5, 133, 1, 134, 20480, MAGIC, 1, 1}; // according to the layout given at the beginning

    std::vector<char> buffer(512, 0); //initialize zeroed buffer

    std::memcpy(buffer.data(), &superblock, sizeof(superblock)); //copy superblock to buffer, ensuring padded 512-byte block 

    Disk.seekp(0); // put-point on offset zero/superblock
    Disk.write(buffer.data(), 512); //write buffer containing padded-block

    Disk.close();

    std::cout << "wrote superblock to disk\n";
    

    return 0;
}
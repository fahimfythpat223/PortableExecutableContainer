#include "headers/global.hpp" // all STL headers used in source file are included in their respective headers
#include "headers/util.hpp"

MountedDiskClass::MountedDiskClass()
{
    this->filename = "floppy.disk"; // assigns the name at runtime

    DiskFile = std::fstream(this->filename, std::ios::binary | std::ios::out | std::ios::in);
    // opens file and caches the file access.

    if(!DiskFile.is_open())
    {
        throw std::runtime_error("MountedDiskClass error: Could not open disk file\n");
    }

    if(DEBUG_FLAG){std::cout << "DEBUG: Disk file opened and file-access cached\n";}

    DiskFile.seekg(0);
    DiskFile.read(reinterpret_cast<char*>(&superblock), sizeof(superblock));

    if(DiskFile.fail())
    {
        throw std::runtime_error("MountedDiskClass error: Disk read failed\n");
    }

    if(superblock.magic != MAGIC) // verify disk through magic number
    {
        throw std::runtime_error("MountedDiskClass error: the disk may be corrupted or invalid. Magic number mismatch. Aborting\n");
    }

    if(DEBUG_FLAG){std::cout << "DEBUG: the size of superblock is: " << sizeof(superblock) << " bytes\n";};    
}

MountedDiskClass::~MountedDiskClass()
{   
    DiskFile.clear(); // clears the stream in case it's in an error state
    DiskFile.seekp(0); // puts pointer to start of superblock
    DiskFile.write(reinterpret_cast<char*>(&superblock), sizeof(superblock)); // flushes superblock to disk

    try
    {
        if(DiskFile.fail())
        {   
            DiskFile.close(); // warning: if this close failed, then it's not logged. Keep that in mind
            throw std::runtime_error("MountedDiskClass error: Disk flush failed. Disk close was attempted\n");
        }

        if(DEBUG_FLAG){std::cout << "DEBUG: superblock flushed to disk\n";}

        DiskFile.close();

        if(DiskFile.fail())
        {
            throw std::runtime_error("MountedDiskClass error: Disk may not have closed\n");
        }

        if(DEBUG_FLAG){std::cout << "DEBUG: The destructor closed the Disk file\n";}
    }

    catch(std::exception& e)
    {
        std::cerr << e.what();
        std::terminate();
    }
}

std::unique_ptr<MountedDiskClass> MountedDisk;

/*

since it's a global object, the header ("global.hpp") declares it, the source file ("global.cpp") defines it, and the main
function essentially kickstarts it by assigning a newly instantiated MountedDisk to it.

*/

void DiskWriterClass::InitEmptyDisk()
{
    //I once faced a bug where for whatever reason, the program couldn' create a file despite the fact that it worked before.
    //I ended up fixing it by adding "std::ios::trunc" as an openmode. I have no clue why that is, but don't remove it.
    //Also, because it worked now, consider adding that whenever you need to CREATE a file that the program writes to
    
    std::fstream WritingDisk(MountedDisk->filename, std::ios::trunc | std::ios::out | std::ios::binary); 

    if(!WritingDisk.is_open())
    {
        std::cerr << "InitEmptyDisk error: could not open disk for initialization\n";
        std::exit(1);
    }

    std::vector<char> buffer(BLOCK_SIZE, 0);

    for (size_t i = 0; i < BLOCK_NUMBER; i++)
    {
        WritingDisk.write(buffer.data(), buffer.size());

    }

    WritingDisk.close();

    std::cout << "initialized empty disk\n";

}

void DiskWriterClass::WriteSuperBlock()
{
    std::ofstream Disk(MountedDisk->filename, std::ios::out | std::ios::binary);

    if(!Disk.is_open())
    {
        std::cerr << "WriteSuperBlock error: could not open disk for writing superblock\n";
        std::exit(1);
    }

    SuperBlock superblock{127, 1, 128, 5, 133, 1, 134, 20480, MAGIC, 1, 1}; // according to the layout given at the beginning

    std::vector<char> buffer(BLOCK_SIZE, 0); //initialize zeroed buffer

    std::memcpy(buffer.data(), &superblock, sizeof(superblock)); //copy superblock to buffer, ensuring padded 512-byte block 

    Disk.seekp(0); // put-point on offset zero/superblock
    Disk.write(buffer.data(), BLOCK_SIZE); //write buffer containing padded-block

    Disk.close();

    std::cout << "wrote superblock to disk\n";
        
}

void DiskParserClass::ReadDisk()
{
    SuperBlock superblock{};

    std::memset(&superblock, 0, sizeof(superblock));

    std::ifstream Disk(MountedDisk->filename, std::ios::binary);

    if(!Disk.is_open())
    {
        std::cerr << "ReadDisk error: could not open disk for reading, file may not exist\n";
        std::exit(1);
    }
            
    Disk.read(reinterpret_cast<char*>(&superblock), sizeof(superblock));
    std::cout << "bytes read: " << Disk.gcount() << "\n\n\n";

    if(superblock.magic != MAGIC)
    {
        std::cerr << "ReadDisk error: disk is either invalid or corrupted. ERROR: MAGIC NUMBER MISMATCH\n";
        std::exit(1);
    }

    /*
            
    the uint8_t type is actually the only fixed-width integer that is actually a char. So you need to cast them
    to an integer if you ever want to work with them.

    */

    std::cout << "major version: " << static_cast<int>(superblock.version_major) << "\n";
    std::cout << "minor version: " << static_cast<int>(superblock.version_minor) << "\n";
    std::cout << "inode table block count: " << superblock.inode_table_block_count << "\n";
    std::cout << "inode table block start: " << superblock.inode_table_block_start << "\n";
    std::cout << "block bitmap block count: " << superblock.block_bitmap_block_count << "\n";
    std::cout << "block bitmap block start: " << superblock.block_bitmap_block_start << "\n";
    std::cout << "inode bitmap block count: " << superblock.inode_bitmap_block_count << "\n";
    std::cout << "inode bitmap block start: " << superblock.inode_bitmap_block_start << "\n";
    std::cout << "data region block start: " << superblock.data_region_block_start << "\n";
    std::cout << "data region block count: " << superblock.data_region_block_count << "\n";
}
#include "headers/util.hpp"

void DumpTunnel(std::string filename, const std::vector<std::uint8_t>& bytes_to_dump) // low-level function dumps any sequence
{                                                                                     // of bytes as is. Used through wrapper
    std::ofstream DumpFile(filename, std::ios::binary | std::ios::trunc);             // DumpStruct()

    // I added "std::ios::trunc" as an openmode. Just in case...
    
    if(!DumpFile.is_open())
    {
        std::cerr << "DumpTunnel error: could not open dump file\n";
        std::exit(1);
    }

    DumpFile.write(reinterpret_cast<const char*>(bytes_to_dump.data()), bytes_to_dump.size());
    DumpFile.flush(); // forces the buffer to flush to disk to avoid silent failure
    DumpFile.close();

    if(!DumpFile) // if writing somehow failed due to things like permission error, it returns false
    {
        std::cerr << "DumpTunnel error: disk write failed" << "\n";
        std::exit(1);
    }

    std::cout << bytes_to_dump.size() << " bytes dumped to " << filename << "\n";
}



void DumpSpecificBlock(const std::vector<std::string>& args)
{
    try
    {
        if(args.size() <= 2)
        {
            std::cerr << "DumpSpecificBlock error: too few arguments\n";
            std::exit(1);
        }

        int block_number(std::stoi(args[2])); // converts argument block number to integer

        std::string DumpFilename = "block_" + std::to_string(block_number) + ".dump";

        std::fstream Disk(MountedDisk->filename, std::ios::binary | std::ios::in);

        Block dump_block;

        if(!Disk.is_open())
        {
            std::cerr << "DumpSpecificBlock error: could not open disk for reading\n";
            std::exit(1);
        }

        if(block_number * BLOCK_SIZE > BLOCK_SIZE * BLOCK_NUMBER)
        {
            std::cerr << "DumpSpecificBLock error: the requested block is out of bounds\n";
            std::exit(1);
        }

        Disk.seekg(block_number * BLOCK_SIZE); // note to self: seekp is for output. seekg is for input
        Disk.read(reinterpret_cast<char*>(&dump_block), sizeof(dump_block));

        DumpStruct(DumpFilename, dump_block);
    }

    catch(std::invalid_argument &e)
    {
        std::cerr << "DumpSpecificBlock error: number of blocks is not an integer\n";
    }

    catch(std::out_of_range &e)
    {
        std::cerr << "DumpSpecificBlock error: number of blocks is outside of integer range\n";
    }

    catch(std::exception &e)
    {
        std::cerr << "DumpSpecificBlock error: unknown exception caught: " << e.what() << "\n";
    }
}



std::uint64_t AllocateBlock()
{
    std::fstream Disk(MountedDisk->filename, std::ios::binary | std::ios::in | std::ios::out);

    if(!Disk.is_open())
    {
        std::cerr << "AllocateBlock error: Disk could not be opened\n";
        std::exit(1);
    }

    SuperBlock superblock;

    Disk.read(reinterpret_cast<char*>(&superblock), sizeof(superblock));

    std::uint8_t bitmap[BLOCK_SIZE * superblock.block_bitmap_block_count]; // computes amount of bytes in the entire bitmap

    Block bitmap_block; // buffer to store individual bitmap blocks in the loop

    for (size_t i = 0; i < superblock.block_bitmap_block_count; i++) // goes through each individual bitmap block according to superblock
    {
        Disk.seekg((superblock.block_bitmap_block_start + i) * BLOCK_SIZE);
        
        /*
        
        computes the offset of individual bitmap blocks and puts the reading pointer to the correct position.
        If it's the first iteration, then the offset would be the same as the start of the block bitmap region
        aka the offset of the first bitmap block       
        
        */

        Disk.read(reinterpret_cast<char*>(&bitmap_block), sizeof(bitmap_block)); // reads the bitmap block into the buffer

        std::memcpy(&bitmap[i * BLOCK_SIZE], &bitmap_block, sizeof(bitmap_block));
        
        /*
        
        copies the block into the correct position in the bitmap array. because:

        bitmap size = size of each block x number of bitmap blocks present

        ...it will calculate the next empty index.

        The first block = 0 x block size = index 0
        the second block = 1 x block size = index 512 (the next empty position after the last byte of the first block)
        
        ...so on and so forth

        */
    }

    unsigned int byte_index;
    unsigned int bit_index;

    /*
    
    the loop needs to be reworked once dynamic disk size is implemented. Just a heads up
    
    */


    for(byte_index = 0; byte_index < BLOCK_SIZE * superblock.block_bitmap_block_count; ++byte_index)
    {
        for(bit_index = 0; bit_index < 8; ++bit_index)
        {
            if((bitmap[byte_index] & (1 << bit_index)) == 0) // ALWAYS put parenthesis around bitwise conditions.
            {
                bitmap[byte_index] |= (1 << bit_index);
                return superblock.data_region_block_start + (byte_index * 8 + bit_index);
            }
        }
    }

    Disk.close();
    return -1;
}

void TestMount()
{
    std::cout << "program executed\n\n\n";
}
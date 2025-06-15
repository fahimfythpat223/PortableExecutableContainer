#pragma once

#include "global.hpp" // global.hpp happens to contain most of the STL header inclusions used in the utility functions, so
/*                    // any STL headers declared here are unique to the utilities.
*/




void DumpTunnel(std::string filename, const std::vector<std::uint8_t>& bytes_to_dump); // low-level function dumps any sequence
/*                                                                                     // of bytes as is. Used through wrapper                                                                                     // DumpStruct()
*/


/*

DumpStruct() and by extension DumpSpecificBlock() are dependent on DumpTunnel() to function, so they are placed after DumpTunnel.

DumpStruct() is a template function, meaning it MUST be declared and defined in the same file, because a template is more so
a function generator as opposed to a function in and of itself.
*/


template<typename T> void DumpStruct(const std::string& filename, const T& input_struct) // wrapper for DumpTunnel().
{                                                                                        // Dumps any structure to a file
    static_assert(std::is_trivially_copyable<T>::value, "DumpStruct static error: input struct must be a POD and trivially copyable");

    std::vector<std::uint8_t> BufferToDump(sizeof(input_struct));
    std::memcpy(BufferToDump.data(), &input_struct, sizeof(input_struct));

    DumpTunnel(filename, BufferToDump);
}



void DumpSpecificBlock(const std::vector<std::string>& args); // parses a ".disk" file and dumps a specific block



std::uint64_t AllocateBlock(); // will probably move over to become a method to MountedDiskClass();

void TestMount(); // prints a message and nothing else.
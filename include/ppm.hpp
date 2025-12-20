#ifndef PPM_HPP
#define PPM_HPP
#include <string>
#include <vector>
#include <cstdint>

class PPM{
public:
    static void write_p6(const std::string& filename,std::size_t width,std::size_t height,const std::vector<std::uint8_t>& rgb);
};

#endif
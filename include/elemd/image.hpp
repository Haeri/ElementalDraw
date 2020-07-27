#ifndef ELEMD_IMAGE_HPP
#define ELEMD_IMAGE_HPP

#include <elemd/elemental_draw.hpp>

#include <map>
#include <string>

namespace elemd
{
    class ELEMD_API image
    {
    public:
        static image* create(std::string file_path);

        unsigned char* get_data();
        int get_width();
        int get_height();
        int get_channels();

        void destroy();

    protected:
        static std::map<std::string, image*> _image_index;

        int _width = -1;
        int _height = -1;
        int _components = -1;

        unsigned char* _data = nullptr;

        image() = default;
        virtual ~image() = default;
    };

} // namespace elemd

#endif // ELEMD_IMAGE_HPP
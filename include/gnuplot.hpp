#ifndef DRONE_GNUPLOT_HPP
#define DRONE_GNUPLOT_HPP

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <unistd.h>
#include <unistdio.h>

namespace drone::gnuplot::style {

    using gnu_line = enum {
        GNU_LINE_CONTINUOUS,
        GNU_LINE_DASHED
    };

    class gnu_style {
    public:

    private:
        gnu_line line;
    };
}

namespace drone::gnuplot {



    class gnu_object3d {


    private:
        std::vector<std::string> file_names;
        std::vector<std::vector<size_t>> vertex_order;

        style::gnu_style line_style;
    };


    class gnuplot {

    };

    //class gnurenderer;
}

#endif //DRONE_GNUPLOT_HPP

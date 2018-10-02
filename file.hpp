#pragma once

namespace Sko {
    struct FileHeader {
        unsigned char
            signature[4],
            version[2];
        unsigned
            seed,
            data_length;
        FileHeader() :
            signature("sko"),
            seed(0),
            data_length(0) {}
    };
    struct FileRow {
        unsigned char
            assigned[5],
            state[5],
            user[5];
    };
}
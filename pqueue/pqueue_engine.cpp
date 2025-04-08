#include "pqueue_engine.h"

namespace persistent_queue {
    File_Marker::File_Marker(unsigned long position):
        position{position}
    {}

    Chunk::Chunk(unsigned long position):
        position{position}
    {}

}

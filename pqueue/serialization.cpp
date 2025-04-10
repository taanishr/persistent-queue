#include "pqueue_engine.h"

namespace persistent_queue {
    void to_json(json& j, const File_Marker& file_marker) {
        j["position"] = file_marker.get_position();
        j["deleted"] = file_marker.is_deleted();
    }

    void from_json(const json& j, File_Marker& file_marker) {
        file_marker.position = j.at("position").get<unsigned long>();
        file_marker.deleted = j.at("deleted").get<bool>();
    }


    void to_json(json& j, const Chunk& chunk) {
        j["position"] = chunk.get_position();
        j["file_markers"] = json(chunk.file_markers());
        j["oldest_item_file_marker"] = std::distance(chunk.file_markers().rbegin(), chunk.oldest_item_file_marker());
 
    }

    void from_json(const json& j, Chunk& chunk) {
        chunk.position = j.at("position").get<unsigned long>();
        chunk.fms = j.at("file_markers").get<decltype(chunk.fms)>();
        chunk.oldest_item_fm = chunk.fms.rbegin() + j.at("oldest_item_file_marker");
    }


}




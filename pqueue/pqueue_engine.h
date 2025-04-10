#include <fstream>
#include <nlohmann/detail/conversions/to_json.hpp>
#include <optional>
#include <vector>
#include <iostream>
#include <deque>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <type_traits>

namespace persistent_queue {    
    struct File_Marker {
        unsigned long position;
        bool deleted;

        File_Marker(unsigned long position);
    };

    struct Chunk {
        unsigned long position;
        std::vector<File_Marker> fms;
        std::vector<File_Marker>::reverse_iterator oldest_item_fm;
        Chunk(unsigned long position);
    };

    template <typename T>
    struct Default_Serializer {
        static void serialize(std::ostream& os, const T& value) {
            os << value << ',';
        }

        T deserialize(std::istream& is) {
            T value;
            is >> value;
            return value;
        }
    };

    template <
            typename T, 
            typename Container = std::deque<T>,
            typename Serializer = Default_Serializer<T>>
    class Engine {
    public:
        Engine(std::filesystem::path filename);

        void open_file(std::filesystem::path filename);

        using size_type = unsigned long;
        size_type buffer_size = 64;

        void enqueue(const T& elem);
        std::optional<T> dequeue(); 

        void flush();
        void load();

        size_type size();
        size_type saved_size();
    
        static void to_json(nlohmann::json& j, const Engine& engine); 

        static void from_json(nlohmann::json& j, Engine& engine); 
        
        // NOT YET IMPLEMENTED
        std::filesystem::path save_metadata();
        void load_metadata(std::filesystem::path path);
    private:
        Container enqueue_buffer; 
        Container dequeue_buffer;
        std::filesystem::path filename;
        std::fstream file;
        std::deque<Chunk> file_chunks;
        Serializer serializer;

        size_type sz;
        size_type saved_sz;
    };


    template <typename T, typename Container, typename Serializer>
    Engine<T, Container, Serializer>::Engine(std::filesystem::path filename):
        filename{filename},
        sz{0},
        saved_sz{0}
    {
        open_file(filename);
    }

    template <typename T, typename Container, typename Serializer>
    void Engine<T, Container, Serializer>::open_file(std::filesystem::path filename)
    {
        file.close();
        file.open(filename, std::ios::in | std::ios::out | std::ios::app);
    }

    template <typename T, typename Container, typename Serializer>
    Engine<T, Container, Serializer>::size_type Engine<T, Container, Serializer>::size()
        { return sz; }

    template <typename T, typename Container, typename Serializer>
    Engine<T, Container, Serializer>::size_type Engine<T, Container, Serializer>::saved_size()
        { return saved_sz; }


    template <typename T, typename Container, typename Serializer> 
    void Engine<T, Container, Serializer>::enqueue(const T& elem) {
        enqueue_buffer.push_front(elem);
        ++sz;

        if (enqueue_buffer.size() >= buffer_size)
            flush();
    }

    template <typename T, typename Container, typename Serializer>
    std::optional<T> Engine<T, Container, Serializer>::dequeue() {
        std::optional<T> value_container;

        T value;

        if (dequeue_buffer.size() == 0) 
            load();
       
        if (dequeue_buffer.size() > 0) { 
            value = dequeue_buffer.back();
            dequeue_buffer.pop_back();
            *value_container = value;
        }

        return value_container;
    }

    template <typename T, typename Container, typename Serializer> 
    void Engine<T, Container, Serializer>::flush() {
        auto it = enqueue_buffer.begin();
        size_type file_position = file.tellg();
        Chunk file_chunk {file_position};
        for (int i = 0; it != enqueue_buffer.end() && i < buffer_size; ++it, ++i) {
            file_chunk.fms.push_back(File_Marker{file_position});
            serializer.serialize(file, *it);
            ++saved_sz;
            file_position = file.tellg();
        }
        file_chunk.oldest_item_fm = file_chunk.fms.rbegin();
        file_chunks.push_back(file_chunk);
        enqueue_buffer.clear();
    }

    template <typename T, typename Container, typename Serializer> 
    void Engine<T, Container, Serializer>::load() {
        if (file_chunks.size() == 0) {
            for (int i = 0; i < enqueue_buffer.size(); ++i)
                dequeue_buffer.push_back(enqueue_buffer[i]);

            enqueue_buffer.clear();
        }else {
            Chunk first_chunk = file_chunks.front();
            file_chunks.pop_front();
            auto oldest_item_fm = first_chunk.oldest_item_fm;

            while (oldest_item_fm != first_chunk.fms.rend()) {
                if (!oldest_item_fm->deleted) {
                    file.seekg(oldest_item_fm->position);
                    T value;
                    value = serializer.deserialize(file);
                    dequeue_buffer.push_front(value);
                    oldest_item_fm->deleted = true;
                    ++oldest_item_fm;
                }
            }
        }
    }

    template <typename T, typename Container, typename Serializer>
    void Engine<T, Container, Serializer>::to_json(nlohmann::json& j, const Engine& engine) {
        // flush data to filestream (do not save buffers)
        engine.flush();

        // user responsible for restoring serializer
        j["filename"] =  engine.filename.c_str();
        j["file_chunks"] = nlohmann::json(engine.file_chunks);
        j["size"] = engine.size();
        j["saved_size"] = engine.saved_size();
    };

    template <typename T, typename Container, typename Serializer>
    void Engine<T, Container, Serializer>::from_json(nlohmann::json& j, Engine& engine) {
        std::filesystem::path filename {j.at("filename").get<std::string>()};

        engine.filename = filename;
        engine.open_file(filename);
        engine.file_chunks = j.at("file_chunks").get<decltype(engine.file_chunks)>(); 
        engine.sz = j.at("size").get<Engine<T, Container, Serializer>::size_type>();
        engine.saved_sz = j.at("saved_size").get<Engine<T, Container, Serializer>::size_type>();
    }

    template <typename T, typename Container, typename Serializer>
    std::filesystem::path Engine<T, Container, Serializer>::save_metadata() {
        std::filesystem::path
    }

};

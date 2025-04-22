#include <fstream>
#include <nlohmann/detail/conversions/to_json.hpp>
#include <optional>
#include <vector>
#include <iostream>
#include <deque>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <type_traits>

using json = nlohmann::json;

namespace persistent_queue {    
    struct File_Marker {
        unsigned long position;
        bool deleted;

        File_Marker():
            position{0},
            deleted{false}
        {}

        File_Marker(unsigned long position);

        unsigned long get_position() const { return position; }
        bool is_deleted() const { return deleted; }
    };

    struct Chunk {
        unsigned long position;
        std::vector<File_Marker> fms;
        std::vector<File_Marker>::reverse_iterator oldest_item_fm;

        Chunk():
            position{0}
        {}

        Chunk(unsigned long position);
   
        unsigned long get_position() const { return position; }
        const std::vector<File_Marker>& file_markers() const { return fms; }
        std::vector<File_Marker>::const_reverse_iterator oldest_item_file_marker() const { return oldest_item_fm; }
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
        Engine() = default;

        Engine(std::filesystem::path filename);

        void open_file(std::filesystem::path filename);

        using size_type = unsigned long;
        size_type buffer_size = 64;

        void enqueue(const T& elem);
        std::optional<T> dequeue(); 

        void flush();
        void load();

        const Container& input_buffer() const;
        void set_input_buffer(Container&& input_buffer);


        const Container& output_buffer() const;
        void set_output_buffer(Container&& input_buffer);

        std::filesystem::path output_file() const;
        
        const std::deque<Chunk>& chunks() const;
        void set_chunks(std::deque<Chunk>&& chunks);

        size_type size() const;
        void set_size(size_type new_size);

        size_type saved_size() const;
        void set_saved_size(size_type new_saved_size);
        
        // NOT YET IMPLEMENTED
        std::filesystem::path save_metadata();
        void load_metadata(std::filesystem::path path);
    private:
        Container in_buffer; 
        Container out_buffer;
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
        this->filename = filename;
        file.open(filename, std::ios::in | std::ios::out | std::ios::app);
    }

    template <typename T, typename Container, typename Serializer>
    Engine<T, Container, Serializer>::size_type Engine<T, Container, Serializer>::size() const
        { return sz; }

    template <typename T, typename Container, typename Serializer>
    void Engine<T, Container, Serializer>::set_size(size_type new_size)
        { sz = new_size; };

    template <typename T, typename Container, typename Serializer>
    Engine<T, Container, Serializer>::size_type Engine<T, Container, Serializer>::saved_size() const
        { return saved_sz; }

    template <typename T, typename Container, typename Serializer>
    void Engine<T, Container, Serializer>::set_saved_size(size_type new_saved_size)
        { saved_sz = new_saved_size; };

    template <typename T, typename Container, typename Serializer>
    const Container& Engine<T, Container, Serializer>::input_buffer() const {
        return in_buffer;
    }

    template <typename T, typename Container, typename Serializer>
    void Engine<T, Container, Serializer>::set_input_buffer(Container&& input_buffer) {
        this->in_buffer = std::forward<Container>(input_buffer);
    }


    template <typename T, typename Container, typename Serializer>
    const Container& Engine<T, Container, Serializer>::output_buffer() const {
        return out_buffer;
    }

    template <typename T, typename Container, typename Serializer>
    void Engine<T, Container, Serializer>::set_output_buffer(Container&& output_buffer) {
        this->out_buffer = std::forward<Container>(output_buffer);
    }

    template <typename T, typename Container, typename Serializer>
    std::filesystem::path Engine<T,Container,Serializer>::output_file() const {
        return filename;
    }
   
    template <typename T, typename Container, typename Serializer>
    const std::deque<Chunk>& Engine<T,Container,Serializer>::chunks() const {
        return file_chunks;
    }

    template <typename T, typename Container, typename Serializer>
    void Engine<T,Container,Serializer>::set_chunks(std::deque<Chunk>&& chunks) {
        file_chunks = std::forward<std::deque<Chunk>>(chunks);
    }

    void to_json(json& j, const File_Marker& file_marker);

    void from_json(const json& j, File_Marker& file_marker); 

    void to_json(json& j, const Chunk& chunk); 

    void from_json(const json& j, Chunk& chunk); 

    template <typename T, typename Container, typename Serializer>
    void to_json(json& j, const Engine<T,Container,Serializer>& engine) {
        // user responsible for restoring serializer
        j["filename"] =  engine.output_file().c_str();
        j["input_buffer"] = json(engine.input_buffer());
        j["output_buffer"] = json(engine.output_buffer());
        j["file_chunks"] = json(engine.chunks());
        j["saved_size"] = engine.saved_size();
    };

    template <typename T, typename Container, typename Serializer>
    void from_json(const json& j, Engine<T,Container,Serializer>& engine) {
        std::filesystem::path filename {j.at("filename").get<std::string>()};

        engine.open_file(filename);
        engine.set_chunks(j.at("file_chunks").get<std::deque<Chunk>>());
        engine.set_input_buffer(j.at("input_buffer").get<Container>());
        engine.set_output_buffer(j.at("output_buffer").get<Container>());
        engine.set_size(0);
        engine.set_saved_size(j.at("saved_size").get<std::size_t>());
    }



    template <typename T, typename Container, typename Serializer> 
    void Engine<T, Container, Serializer>::enqueue(const T& elem) {
        in_buffer.push_front(elem);
        ++sz;

        if (in_buffer.size() >= buffer_size)
            flush();
    }

    template <typename T, typename Container, typename Serializer>
    std::optional<T> Engine<T, Container, Serializer>::dequeue() {
        std::optional<T> value_container;

        T value;

        if (out_buffer.size() == 0) 
            load();
       
        if (out_buffer.size() > 0) { 
            value = out_buffer.back();
            out_buffer.pop_back();
            *value_container = value;
        }

        return value_container;
    }

    template <typename T, typename Container, typename Serializer> 
    void Engine<T, Container, Serializer>::flush() {
        auto it = in_buffer.begin();
        size_type file_position = file.tellg();
        Chunk file_chunk {file_position};
        for (int i = 0; it != in_buffer.end() && i < buffer_size; ++it, ++i) {
            file_chunk.fms.push_back(File_Marker{file_position});
            serializer.serialize(file, *it);
            ++saved_sz;
            file_position = file.tellg();
        }
        file_chunk.oldest_item_fm = file_chunk.fms.rbegin();
        file_chunks.push_back(file_chunk);
        in_buffer.clear();
    }

    template <typename T, typename Container, typename Serializer> 
    void Engine<T, Container, Serializer>::load() {
        if (file_chunks.size() == 0) {
            for (int i = 0; i < in_buffer.size(); ++i)
                out_buffer.push_back(in_buffer[i]);

            in_buffer.clear();
        }else {
            Chunk first_chunk = file_chunks.front();
            file_chunks.pop_front();
            auto oldest_item_fm = first_chunk.oldest_item_fm;

            while (oldest_item_fm != first_chunk.fms.rend()) {
                if (!oldest_item_fm->deleted) {
                    file.seekg(oldest_item_fm->position);
                    T value;
                    value = serializer.deserialize(file);
                    out_buffer.push_front(value);
                    oldest_item_fm->deleted = true;
                    ++oldest_item_fm;
                }
            }
        }
    }

    template <typename T, typename Container, typename Serializer>
    std::filesystem::path Engine<T, Container, Serializer>::save_metadata() {
        flush();

        std::filesystem::path metadata_path = std::filesystem::current_path();
        metadata_path = metadata_path / "metadata.txt";
        std::fstream metadata_file;
        metadata_file.open(metadata_path, std::ios_base::out | std::ios_base::trunc);
        json j {*this};
        metadata_file << j;

        return metadata_path;
    }

    template <typename T, typename Container, typename Serializer>
    void Engine<T, Container, Serializer>::load_metadata(std::filesystem::path filepath) {
        std::ifstream metadata {filepath};
        json data = json::parse(metadata);
        std::cout << data << '\n';
        Engine<T, Container, Serializer> new_engine = data[0].get<Engine<T, Container, Serializer>>();
        std::swap(*this, new_engine);
    }



};

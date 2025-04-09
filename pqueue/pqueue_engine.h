#include <fstream>
#include <optional>
#include <vector>
#include <iostream>
#include <deque>
#include <nlohmann/json.hpp>

namespace persistent_queue {    
    struct File_Marker {
        unsigned long position;
        bool deleted;

        File_Marker(unsigned long position);
    };

    struct Chunk {
        unsigned long position;
        std::vector<File_Marker> fms;
        std::vector<File_Marker>::iterator oldest_item_fm;
        bool isEmpty();
        Chunk(unsigned long position);
    };

    template <typename T, typename Container = std::deque<T>>
    class Engine {
    public:
        Engine(std::fstream& file);

        using size_type = unsigned long;
        size_type buffer_size = 64;

        void enqueue(const T& elem);
        std::optional<T> dequeue(); 

        void flush();
        void load();

        size_type size();
    private:
        Container enqueue_buffer; 
        Container dequeue_buffer;
        std::fstream& file;
        std::deque<Chunk> file_chunks;

        size_type saved_sz;
        size_type sz;
    };


    template <typename T, typename Container>
    Engine<T, Container>::Engine(std::fstream& file):
        file{file},
        sz{0}
        {}

    template <typename T, typename Container>
    Engine<T, Container>::size_type Engine<T, Container>::size()
        { return sz; }

    template <typename T, typename Container> 
    void Engine<T, Container>::enqueue(const T& elem) {
        enqueue_buffer.push_front(elem);

        if (enqueue_buffer.size() >= buffer_size)
            flush();
    }

    template <typename T, typename Container>
    std::optional<T> Engine<T, Container>::dequeue() {
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

    template <typename T, typename Container> 
    void Engine<T, Container>::flush() {
        auto it = enqueue_buffer.begin();
        size_type file_position = file.tellg();
        Chunk file_chunk {file_position};
        for (int i = 0; it != enqueue_buffer.end() && i < buffer_size; ++it, ++i) {
            file_chunk.fms.push_back(File_Marker{file_position});
            file_chunk.oldest_item_fm = file_chunk.fms.end() - 1;
            file << *it << ',';
            ++saved_sz;
            file_position = file.tellg();
        }
        file_chunks.push_back(file_chunk);
        enqueue_buffer.clear();
    }

    template <typename T, typename Container> 
    void Engine<T, Container>::load() {
        if (file_chunks.size() == 0) {
            for (int i = 0; i < enqueue_buffer.size(); ++i) {
                dequeue_buffer.push_front(enqueue_buffer[i]);
            }
            enqueue_buffer.clear();
        }else {
            Chunk first_chunk = file_chunks.front();
            file_chunks.pop_front();
            auto oldest_item_fm = first_chunk.oldest_item_fm;

            while (oldest_item_fm != first_chunk.fms.begin()) {
                if (!oldest_item_fm->deleted) {
                    file.seekg(oldest_item_fm->position);
                    T value;
                    file >> value;
                    dequeue_buffer.push_front(value);
                    oldest_item_fm->deleted = true;
                    --oldest_item_fm;
                }
            }

            if (!oldest_item_fm->deleted) {
                file.seekg(oldest_item_fm->position);
                T value;
                file >> value;
                dequeue_buffer.push_front(value);
                oldest_item_fm->deleted = true;
                first_chunk.oldest_item_fm = first_chunk.fms.end();
            }
        }
    }
};

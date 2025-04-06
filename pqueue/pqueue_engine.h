#include <fstream>
#include <optional>
#include <vector>
#include <iostream>

namespace persistent_queue {    
    struct File_Marker {
        unsigned long position;
        bool deleted;

        File_Marker(unsigned long position);
    };

    // work flow:
    // enqueue/dequeue (wrapper around data structure) 
    // flush (flushes all in memory stuff to database manually (should be automatic though)
    // serialize (converts data structure into in memory stuff);
    // buffer (gets current buffer (in memory data structure)
    template <typename T, typename Container = std::deque<T>>
    class Engine {
        public:
            // eventualy; switch to using file chunk_size
            // prefix auto generated by object 
            // keep track of chunks vector (all file strings)
            // have map of chunk strings and fstreams
            // delete when chunks become empty
            Engine(std::fstream& file);

            using size_type = unsigned long;
            size_type buffer_size = 64;
            size_type chunk_size = 1024;

            void enqueue(const T& elem);

//            template <typename Batch_Container>
//            void enqueueBatch(const Batch_Container& batch);

            std::optional<T> dequeue();
//            template <typename Batch_Container>
//            Batch_Container dequeueBatch(size_t count);
            std::optional<T> peek() const;

            // flush to file
            void flush();
            void load();

            const Container& buffer() const;

            size_type size(); // size of pqueue
            bool isEmpty();
            size_type capacity(); // remaining buffer space

            void clear();
        private:
            Container memory_buffer;
            std::fstream file;
            std::vector<File_Marker> file_markers;
            std::vector<File_Marker>::iterator last_file_marker;
            size_type sz;
            size_type saved_sz;

            void serialize(Container& batch);
            std::optional<T> dequeueFromBuffer();
            std::optional<T> dequeueFromFile();
    };

    template<typename Container, typename T>
    Engine<Container, T>::Engine(std::fstream& file):
        memory_buffer{Container()}, 
        file{std::move(file)},
        sz{0},
        saved_sz{0},
        last_file_marker(file_markers.end())    
    {}

    template <typename T, typename Container>
    void Engine<T, Container>::enqueue(const T& elem) {
        auto begin = memory_buffer.begin();
        std::cout << "Inserting" << elem << '\n';
        memory_buffer.insert(begin, elem);
        sz++;
        flush();
    }

    template <typename T, typename Container>
    std::optional<T> Engine<T, Container>::dequeue() {
        std::optional<T> elem;
        if (saved_sz > 0) {
            elem = dequeueFromFile();
        } else {
            elem = dequeueFromBuffer(); 
        }
        return elem;
    }

    template <typename T, typename Container>
    std::optional<T> Engine<T, Container>::dequeueFromFile() {
        std::optional<T> elem{};

        if (last_file_marker == file_markers.end())
            return; 

        file.seekg(last_file_marker->position);
        file >> elem;

        file.seekg(0);

        last_file_marker->deleted = true;

        while (last_file_marker != file_markers.begin()) {
            --last_file_marker;
            if (!last_file_marker->deleted)
                break;
        }

        if (last_file_marker->deleted) 
            last_file_marker = file_markers.end();

        --saved_sz;
        --sz;

        return elem;
    }

    template <typename T, typename Container>
    std::optional<T> Engine<T, Container>::dequeueFromBuffer() {
        std::optional<T> elem;

        if (memory_buffer.size() == 0)
            return;

        elem = memory_buffer.back();
        memory_buffer.popBack();

        return elem;
    }

    template <typename T, typename Container>
    void Engine<T, Container>::flush() {
        for (auto it = memory_buffer.rbegin(); it != memory_buffer.rend(); ++it) {
            std::cout << *it << '\n';
            file << *it << ',';
            file_markers.push_back(File_Marker{file.tellg()});
            last_file_marker = file_markers.end()-1;
        }

        memory_buffer.clear();
    }

    template<typename T, typename Container> 
    typename Engine<T, Container>::size_type Engine<T, Container>::size() {
        return sz;
    }

    template<typename T, typename Container>
    bool Engine<T, Container>::isEmpty() {
        return sz == 0; 
    }

    template<typename T, typename Container>
    typename Engine<T, Container>::size_type Engine<T, Container>::capacity() {
        return buffer_size - sz;
    }
};

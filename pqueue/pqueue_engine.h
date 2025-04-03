#include <fstream>
#include <optional>

namespace persistent_queue {    
    // work flow:
    // enqueue/dequeue (wrapper around data structure) 
    // flush (flushes all in memory stuff to database manually (should be automatic though)
    // serialize (converts data structure into in memory stuff);
    // buffer (gets current buffer (in memory data structure)
    template <typename T, typename Container = std::deque<T>>
    class Engine {
        public:
            Engine(std::fstream& file);

            using size_type = unsigned long;
            size_type buffer_size = 64;

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
            std::vector<unsigned long> file_object_markers;
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
        saved_sz{0}
    {}

    template <typename T, typename Container>
    void Engine<T, Container>::enqueue(const T& elem) {
        auto begin = memory_buffer.begin();
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

        if (file_object_markers.size() == 0)
            return;

        unsigned long last_object_marker = file_object_markers[file_object_markers.size()-1];

        file.seekg(last_object_marker);
        file >> elem;

        file.seekg(0);


        --saved_sz;
        --sz;
    }

    template <typename T, typename Container>
    std::optional<T> Engine<T, Container>::dequeueFromBuffer() {

    }


//    template <typename T, typename Container>
//    template <typename Batch_Container>
//    void Engine<T, Container>::enqueueBatch(const Batch_Container& batch) {
//        for (auto curr = batch.begin(); curr != batch.end(); ++curr) {
//            auto begin = memory_buffer.begin();
//            memory_buffer.insert(begin, *curr);
//        }
//
//        flush();
//    }




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

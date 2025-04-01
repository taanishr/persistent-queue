#include <fstream>


namespace persistent_queue {    
    // work flow:
    // enqueue/dequeue (wrapper around data structure) 
    // flush (flushes all in memory stuff to database manually (should be automatic though)
    // serialize (converts data structure into in memory stuff);
    // buffer (gets current buffer (in memory data structure)
    template <typename T, typename Container = std::deque<T>>
    class Engine {
        public:
            Engine(std::ofstream& output_file);

            using size_type = unsigned long;
            size_type buffer_size = 64;

            void enqueue(const T& elem);

            template <typename Batch_Container>
            void enqueueBatch(const Batch_Container& batch);

            T dequeue();
            template <typename Batch_Container>
            Batch_Container dequeueBatch(size_t count);
            T peek() const;

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
            std::ofstream output_file;
            size_type sz;


            void serialize(Container& batch);
    };

    template<typename Container, typename T>
    Engine<Container, T>::Engine(std::ofstream& output_file):
        memory_buffer{Container()}, 
        output_file{std::move(output_file)},
        sz{0}
    {}

    template <typename T, typename Container>
    void Engine<T, Container>::enqueue(const T& elem) {
        auto begin = memory_buffer.begin();
        memory_buffer.insert(begin, elem);
        flush();
    }

    template <typename T, typename Container>
    template <typename Batch_Container>
    void Engine<T, Container>::enqueueBatch(const Batch_Container& batch) {
        for (auto curr = batch.begin(); curr != batch.end(); ++curr) {
            auto begin = memory_buffer.begin();
            memory_buffer.insert(begin, *curr);
        }

        flush();
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

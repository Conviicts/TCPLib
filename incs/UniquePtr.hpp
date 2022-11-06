#pragma once

namespace TCP {
    template<class T>
    class unique_ptr;

    template <class T>
    class unique_ptr {
        public:
            unique_ptr(T *ptr = NULL):ptr(ptr) {}
            virtual ~unique_ptr() throw() { delete ptr; }

            T *release() throw() {
                T *r = ptr;
                ptr = NULL;
                return r;
            }

            void reset(T *nptr = NULL) throw() {
                delete ptr;
                ptr = nptr;
            }

            T *get() const { return ptr; }
            T &operator*() { return *ptr; }
            T *operator->() { return ptr; }
            operator bool() const { return (ptr); }

        private:
            T *ptr;
            unique_ptr<T> &operator=(const unique_ptr<T> &);
            unique_ptr(const unique_ptr<T> &);
    };
}

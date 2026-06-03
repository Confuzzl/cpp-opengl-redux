#include "app/app.h"

#include "util.h"

#include "gl/bo_heap.h"

int main() {
  try {
    // app();
    // VBOHeap heap{};
    // const auto a = heap.allocate(1);
    // const auto b = heap.allocate(1);
    // heap.deallocate(a);
    // heap.deallocate(b);
    // heap.allocate(100);
    // heap.print();

    app().run();
  } catch (const std::runtime_error &e) {
    print_err("RUNTIME ERROR: {}", e.what());
  }
}

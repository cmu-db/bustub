#include <cstdlib>
#include <exception>
#include "backward.hpp"

class TerminateHandler {
 public:
  TerminateHandler() {
    std::set_terminate([]() {
      backward::StackTrace st;
      st.load_here(32);
      backward::Printer p;
      p.object = true;
      p.snippet = false;
      p.color_mode = backward::ColorMode::automatic;
      p.address = true;
      p.print(st, stderr);
      std::abort();
    });
  }
};

TerminateHandler th;

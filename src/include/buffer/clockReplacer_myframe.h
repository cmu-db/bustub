//
// Created by zhe on 7/1/21.
//

#ifndef BUSTUB_CLOCKREPLACER_MYFRAME_H
#define BUSTUB_CLOCKREPLACER_MYFRAME_H

#include <cstdint>
class clockReplacer_myframe {
 private:
  bool is_in_CR;
  bool ref_flag;

 public:
  clockReplacer_myframe(bool,bool);
  void updateRefFlag(bool);
  void updateIsInCR(bool);
  bool isFrameInCR();
  bool getRefFlag();
};

#endif  // BUSTUB_CLOCKREPLACER_MYFRAME_H

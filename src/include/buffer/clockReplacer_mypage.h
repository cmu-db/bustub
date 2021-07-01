//
// Created by zhe on 7/1/21.
//

#ifndef BUSTUB_CLOCKREPLACER_MYPAGE_H
#define BUSTUB_CLOCKREPLACER_MYPAGE_H

#include <cstdint>
class clockReplacer_mypage {
 private:
  bool ref_flag;
  int32_t frame_id;

 public:
  clockReplacer_mypage(bool,int32_t);
  void falsify_ref_flag();
  int32_t getFrameId();
};

#endif  // BUSTUB_CLOCKREPLACER_MYPAGE_H

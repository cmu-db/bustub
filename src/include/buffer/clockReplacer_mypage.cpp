//
// Created by zhe on 7/1/21.
//

#include "clockReplacer_mypage.h"

clockReplacer_mypage::clockReplacer_mypage(bool ref, int32_t frame_id){
  this -> ref_flag = ref;
  this -> frame_id = frame_id;
}
void clockReplacer_mypage::falsify_ref_flag(){
  this -> ref_flag = false;
}
int32_t clockReplacer_mypage::getFrameId(){
  return this->frame_id;
}


//
// Created by zhe on 7/1/21.
//

#include "clockReplacer_myframe.h"

// Public constructor
clockReplacer_myframe::clockReplacer_myframe(bool ref, bool inCR){
  this -> is_in_CR = inCR;
  this -> ref_flag = ref;
}

// Public methods
void clockReplacer_myframe::updateRefFlag(bool flag) {
  this -> ref_flag = flag;
}

void clockReplacer_myframe::updateIsInCR(bool flag) {
  is_in_CR = flag;
}

bool clockReplacer_myframe::isFrameInCR() {
  return is_in_CR;
}
bool clockReplacer_myframe::getRefFlag() {
  return ref_flag;
}
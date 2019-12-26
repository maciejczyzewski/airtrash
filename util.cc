#include "util.h"

str remove_to(str S, str del, int shift) {
  return S.substr(S.find(del) + del.size() + shift, S.size());
}

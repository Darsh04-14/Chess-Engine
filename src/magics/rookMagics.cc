#include <stdlib.h>
#include <time.h>

#include <iomanip>
#include <iostream>
#include <unordered_map>
using namespace std;

typedef unsigned long long ULL;

void printBitboard(ULL bitboard) {
  for (int i = 7; i >= 0; --i) {
    for (int j = 0; j < 8; ++j) std::cout << bool(bitboard & (1ULL << (i * 8 + j)));
    std::cout << "\n";
  }
  std::cout << "\n";
}

int getBits(int square) {
  if (square == 0 || square == 7 || square == 56 || square == 63) return 12;
  if (square % 8 == 0 || square % 8 == 7 || square / 8 == 0 || square / 8 == 7) return 11;
  return 10;
}

ULL getRookMaskValue(int square, ULL mask) {
  int u = square, d = square, l = square, r = square;
  ULL movementMask = 0;
  mask |= (1ULL << square);
  while (u + 8 < 64 && (mask & (1LL << u))) {
    u += 8;
    movementMask |= (1ULL << u);
  }
  while (d >= 0 && (mask & (1LL << d))) {
    d -= 8;
    movementMask |= (1ULL << d);
  }
  while (l - 1 >= square / 8 * 8 && (mask & (1LL << l))) {
    l -= 1;
    movementMask |= (1ULL << l);
  }
  while (r + 1 <= square / 8 * 8 + 7 && (mask & (1LL << r))) {
    r += 1;
    movementMask |= (1ULL << r);
  }

  return movementMask;
}

ULL getRookMaskKey(int square, ULL b = 0) {
  ULL bot = 255, left = 72340172838076673;
  ULL top = (bot << 56), right = left << 7;
  ULL mask = (bot << ((square / 8) * 8)) | (left << (square % 8));
  mask ^= (1ULL << square);
  if (!(right & (1ULL << square))) mask &= ~right;
  if (!(left & (1ULL << square))) mask &= ~left;
  if (!(top & (1ULL << square))) mask &= ~top;
  if (!(bot & (1ULL << square))) mask &= ~bot;

  int n = getBits(square);
  for (int i = 0, j = 0; i < n; ++i) {
    ULL bit = ((b >> i) & 1);
    while (!(mask & (1ULL << j))) ++j;
    mask ^= bit << j++;
  }

  return mask;
}

// Function and table are from https://www.chessprogramming.org/Looking_for_Magics

ULL getRandom64Bit() {
  ULL u1, u2, u3, u4;
  u1 = (ULL)(rand()) & 0xFFFF;
  u2 = (ULL)(rand()) & 0xFFFF;
  u3 = (ULL)(rand()) & 0xFFFF;
  u4 = (ULL)(rand()) & 0xFFFF;
  return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

int RBits[64] = {12, 11, 11, 11, 11, 11, 11, 12, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10,
                 10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10,
                 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 12, 11, 11, 11, 11, 11, 11, 12};

int main() {
  srand(time(NULL));
  for (int sq = 0; sq < 64; ++sq) {
    while (true) {
      ULL i = getRandom64Bit() & getRandom64Bit() & getRandom64Bit();
      unordered_map<ULL, ULL> mp;
      ULL n = (1ULL << getBits(sq)) - 1;
      int j = 0;
      for (; j <= n; ++j) {
        ULL rookMaskKey = getRookMaskKey(sq, j);
        ULL rookMaskValue = getRookMaskValue(sq, rookMaskKey);
        ULL mpKey = (rookMaskKey * i) >> (64 - RBits[sq]);

        if (mp.find(mpKey) != mp.end() && mp[mpKey] != rookMaskValue)
          break;
        else
          mp[mpKey] = rookMaskValue;
      }
      if (j > n) {
        std::cout << i << " " << mp.size() << "\n";
        for (auto i : mp) cout << i.first << " " << i.second << "\n";
        break;
      }
    }
  }
  return 0;
}

#include <stdlib.h>
#include <time.h>

#include <iomanip>
#include <iostream>
#include <map>
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

int BBits[64] = {6, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7, 5, 5, 5, 5, 7, 9, 9, 7, 5, 5,
                 5, 5, 7, 9, 9, 7, 5, 5, 5, 5, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6};

int getBits(int square) { return BBits[square]; }

const ULL diagonals[15] = {
    0x1,
    0x102,
    0x10204,
    0x1020408,
    0x102040810,
    0x10204081020,
    0x1020408102040,
    0x102040810204080,
    0x204081020408000,
    0x408102040800000,
    0x810204080000000,
    0x1020408000000000,
    0x2040800000000000,
    0x4080000000000000,
    0x8000000000000000,
};

const ULL rdiagonals[15] = {
    0x100000000000000,
    0x201000000000000,
    0x402010000000000,
    0x804020100000000,
    0x1008040201000000,
    0x2010080402010000,
    0x4020100804020100,
    0x8040201008040201,
    0x80402010080402,
    0x804020100804,
    0x8040201008,
    0x80402010,
    0x804020,
    0x8040,
    0x80,
};

ULL getBishopMaskValue(int square, ULL mask) {
  int r = square / 8, c = square % 8;
  ULL movementMask = 0;

  int offsets[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

  for (int i = 0; i < 4; ++i) {
    int newRow = r + offsets[i][0], newCol = c + offsets[i][1];
    while (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
      int targetIndex = newRow * 8 + newCol;
      movementMask |= (1ULL << targetIndex);

      if (!((mask >> targetIndex) & 1)) break;
      newRow += offsets[i][0];
      newCol += offsets[i][1];
    }
  }

  return movementMask;
}

ULL getBishopMaskKey(int square, ULL b = 0) {
  ULL bot = 255, left = 72340172838076673;
  ULL top = (bot << 56), right = left << 7;
  int r = square / 8, c = square % 8;
  ULL mask = diagonals[r + c] | rdiagonals[7 - r + c];
  mask ^= (1ULL << square);
  mask &= ~(bot | top | left | right);
  int n = getBits(square);
  for (int i = 0, j = 0; i < n; ++i) {
    ULL bit = ((b >> i) & 1);
    while (!(mask & (1ULL << j))) ++j;
    mask ^= bit << j++;
  }

  return mask;
}

// Function and both tables are from https://www.chessprogramming.org/Looking_for_Magics

ULL getRandom64Bit() {
  ULL u1, u2, u3, u4;
  u1 = (ULL)(rand()) & 0xFFFF;
  u2 = (ULL)(rand()) & 0xFFFF;
  u3 = (ULL)(rand()) & 0xFFFF;
  u4 = (ULL)(rand()) & 0xFFFF;
  return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

int main() {
  // ULL key = getBishopMaskKey(42, 0x7F);
  // printBitboard(key);
  // printBitboard(getBishopMaskValue(42, key));
  srand(time(NULL));
  for (int sq = 0; sq < 64; ++sq) {
    while (true) {
      ULL i = getRandom64Bit() & getRandom64Bit() & getRandom64Bit();
      unordered_map<ULL, ULL> mp;
      ULL n = (1ULL << BBits[sq]) - 1;
      int j = 0;
      for (; j <= n; ++j) {
        ULL bishopMaskKey = getBishopMaskKey(sq, j);
        ULL bishopMaskValue = getBishopMaskValue(sq, bishopMaskKey);
        ULL mpKey = (bishopMaskKey * i) >> (64 - BBits[sq]);

        if (mp.find(mpKey) != mp.end() && mp[mpKey] != bishopMaskValue)
          break;
        else
          mp[mpKey] = bishopMaskValue;
      }
      if (j > n) {
        std::cout << i << "\n";
        for (auto i : mp) cout << i.first << " " << i.second << "\n";
        break;
      }
    }
  }
  return 0;
}

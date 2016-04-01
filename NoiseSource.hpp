/*
  NoiseSource.hpp, Inject random data into a buffer
  Copyright (C) 2016 David Merrell

  BufferSummary is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  BufferSummary is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <vector>
#include <nupic/types/Types.hpp>

using namespace std;
using namespace nupic;

  class NoiseSource
  {
    public:
    NoiseSource() {};
    ~NoiseSource() {};
    NoiseSource(const unsigned int InputSize);
    void InsertNoise(vector<unsigned int> &DataVector, int NoiseLevel);
    void RandomizeArray();
    UInt Rand(void);
    void SRand(UInt seed);

    private:
    int ArraySize;
    unsigned int RandState = 54321;
    int *NoiseArray;
    unsigned long NextState = 1;


  };

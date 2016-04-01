/*
  NoiseSource.cpp, Inject random data into a buffer
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
#include <cstdlib>      // std::rand, std::srand
#include "NoiseSource.hpp"

	// always produces the same pseudo-random sequence
NoiseSource::NoiseSource(const unsigned int InputSize)
{
  int i;

  NoiseArray = new int[InputSize];
  ArraySize = InputSize;
  SRand(2542);
  for(i=0; i < ArraySize; i++)
    NoiseArray[i] = i;
  for(i=0; i < 20; i++)
    RandomizeArray();
}

void NoiseSource::RandomizeArray(void)
{
  int i, j, k;

  for(i=0; i < ArraySize; i++)
  {
    j = Rand() % ArraySize;
    k = NoiseArray[i];
    NoiseArray[i] = NoiseArray[j];
    NoiseArray[j] = k;
  } // for
}

void NoiseSource::InsertNoise(vector<unsigned int> &DataVector, int InNoiseLevel)
{
  int i, NoiseLevel;

  NoiseLevel= (InNoiseLevel > ArraySize)?ArraySize:InNoiseLevel;
  for(i=0; i < NoiseLevel; i++)
    DataVector[NoiseArray[i]] = 1;
}

	// hokey old Unix rng
	// RAND_MAX assumed to be 32767
UInt NoiseSource::Rand(void)
{
  NextState = (UInt) ((long long) NextState * 1103515245LL + 12345LL);
  return((UInt)(NextState/65536) % 32768);
}

void NoiseSource::SRand(UInt seed)
{
  NextState = seed;
}


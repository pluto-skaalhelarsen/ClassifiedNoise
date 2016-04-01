/* ---------------------------------------------------------------------
 * Numenta Platform for Intelligent Computing (NuPIC)
 * Copyright (C) 2013-2015, Numenta, Inc.  Unless you have an agreement
 * with Numenta, Inc., for a separate license for this software code, the
 * following terms and conditions apply:
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Affero Public License for more details.
 *
 * You should have received a copy of the GNU Affero Public License
 * along with this program.  If not, see http://www.gnu.org/licenses.
 *
 * http://numenta.org/licenses/
 * ---------------------------------------------------------------------
 */

#include <iostream>
#include <vector>
#include <algorithm>    // std::generate
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand
#include <cmath> 	// pow

#include "CmdLineParse.hpp"
#include "BufferSummary.hpp"
#include "NoiseSource.hpp"
#include "SpParamFile.hpp"
#include "Paragon.hpp"

#include "nupic/algorithms/SpatialPooler.hpp"
#include "nupic/algorithms/Cells4.hpp"
#include "nupic/os/Timer.hpp"

using namespace std;
using namespace nupic;
using nupic::algorithms::spatial_pooler::SpatialPooler;
using nupic::algorithms::Cells4::Cells4;

	// function generator:
	// Genreates about 200 ones in a 10k buffer returning random (binary) numbers from {0,1}
UInt RandState = 0;

int RandomNumber01 ()
{
  int ret = rand();
  if(ret < 85899346)
    return (rand() & 1);
  else
    return(0);
}


int main(int argc, const char * argv[])
{
const UInt DIM = 2048; // number of columns in SP, TP
const UInt DIM_INPUT = 10000;
const UInt EPOCHS = pow(10, 4); // number of iterations (calls to SP/TP compute() )

const UInt RNGSEQUENCECOUNT = 100;	//number of buffers in the rng sequence

  UInt Epochs = EPOCHS;
  UInt EpochsTwo = EPOCHS;
  UInt RngSequenceCount = RNGSEQUENCECOUNT;

  CmdLineParse *CmdLine = new CmdLineParse(argc, argv);
  RandState = CmdLine->Vars("RngSeed");
  srand(RandState);
  if(CmdLine->Vars("Epochs"))
    Epochs = CmdLine->Vars("Epochs");
  if(CmdLine->Vars("RngSequenceCount"))
    RngSequenceCount = CmdLine->Vars("RngSequenceCount");

  vector<UInt> inputDim = {DIM_INPUT};
  vector<UInt> colDim = {DIM};

  // generate random input
  vector<UInt> input(DIM_INPUT);
  vector<UInt> outSP(DIM); // active array, output of SP/TP

  Paragon Classifier(RngSequenceCount, DIM);
  NoiseSource ns = NoiseSource(DIM_INPUT);
  BufferSummary SpSummary = BufferSummary();

  SpatialPoolerParams Params = SpatialPoolerParams(CmdLine->Names("SpParams"));
//  Params.EchoParams();

  SpatialPooler sp(
        inputDim,
        colDim,
        Params.ParamUInt("potentialRadius"),
        Params.ParamReal("potentialPct"),
        Params.ParamBool("globalInhibition"),
        Params.ParamReal("localAreaDensity"),
        Params.ParamUInt("numActiveColumnsPerInhArea"),
        Params.ParamUInt("stimulusThreshold"),
        Params.ParamReal("synPermInactiveDec"),
        Params.ParamReal("synPermActiveInc"),
        Params.ParamReal("synPermConnected"),
        Params.ParamReal("minPctOverlapDutyCycles"),
        Params.ParamReal("minPctActiveDutyCycles"),
        Params.ParamUInt("dutyCyclePeriod"),
        Params.ParamReal("maxBoost"),
        Params.ParamInt("seed"),
        Params.ParamUInt("spVerbosity"),
        Params.ParamBool("wrapAround"));

	// Start a stopwatch timer
  Timer stopwatch(true);

	// ***** Run the learning sequence *****
  for (UInt e = 0; e < Epochs; e++) {
    if((e % RngSequenceCount) == 0)
      srand(RandState);
    generate(input.begin(), input.end(), RandomNumber01);

    fill(outSP.begin(), outSP.end(), 0);
unsigned int *crap = outSP.data();
    sp.compute(input.data(), true, outSP.data());
    sp.stripUnlearnedColumns(outSP.data());
// Stroustrup says the .data() call makes a copy of the data, and that shouldn't be relied on
//    after a non-constant call has been made on the class. (Section 20.3.7, p589)

    if(CmdLine->Flags("--display_while_learning"))
    if((e % (10*RngSequenceCount)) < RngSequenceCount)
     {
        cout << "----------------- ========= ------------------ Epoch " << e << endl;
	// SP
      SpSummary.SetupBuffer(outSP);
      cout << "Pattern sequence number = " << (e%RngSequenceCount) << endl;
      cout << "Non-zero elements of final SP output (location : length) " << endl;
      while(SpSummary.PrepareNextRunMsg())
        cout << SpSummary.Msg << endl;
      }
  }
	// ***** separate the sequences *****
  cout << endl;
  cout << "======================..................=======================............" << endl;
  cout << "======================..................=======================............" << endl;
  cout << endl;

	// ***** Initialize the classifier *****
  srand(RandState);
  for (UInt e = 0; e < RngSequenceCount; e++) {
    generate(input.begin(), input.end(), RandomNumber01);
    fill(outSP.begin(), outSP.end(), 0);
    sp.compute(input.data(), false, outSP.data());	// no learning in sp
    sp.stripUnlearnedColumns(outSP.data());
    Classifier.PatternSet[e] = new SparsePlutoVector(outSP);
  }

	// ***** Run the classifier sequence *****
  srand(RandState);
  for (UInt e = 0; e < RngSequenceCount; e++) {
    generate(input.begin(), input.end(), RandomNumber01);
    ns.InsertNoise(input, CmdLine->Vars("NoiseLevel"));

//.............
#if 0
      SpSummary.SetupBuffer(input);
      cout << "Non-zero elements of input (location : length) " << endl;
      while(SpSummary.PrepareNextRunMsg())
        cout << SpSummary.Msg << endl;
#endif
//..............

    fill(outSP.begin(), outSP.end(), 0);
    sp.compute(input.data(), false, outSP.data());	// no learning in sp
    sp.stripUnlearnedColumns(outSP.data());

	// print classification result
    if(CmdLine->Flags("--classify"))
    {
//for(int i=0; i < Classifier.PatternSet[e]->Ndx.size(); i++)
//  cout << i << " loc= " << Classifier.PatternSet[e]->Ndx[i] << endl;

      Classifier.Classify(outSP);
      cout << "Pattern sequence number = " << e << endl;
      for(int i=0; i < Classifier.MatchProbability.size(); i++)
        cout << i << ": " << Classifier.MatchProbability[i]  << endl;
      cout << "Match " << Classifier.MaxNdx << " with probability "
           << Classifier.MaxProbability << endl;
    }


	// print entire final arrays
    if(CmdLine->Flags("--dump_final"))
    {
      cout << "Pattern sequence number = " << e << endl;
      cout << "SP=" << outSP << endl;
    }
	// print a list of non-zero runs
    if(CmdLine->Flags("--sp_summary"))
    {
      SpSummary.SetupBuffer(outSP);
      cout << "Pattern sequence number = " << e << endl;
      cout << "Non-zero elements of final SP output (location : length) " << endl;
      while(SpSummary.PrepareNextRunMsg())
        cout << SpSummary.Msg << endl;
    }
  }

  stopwatch.stop();
  cout << "Total elapsed time = " << stopwatch.getElapsed() << " seconds" << endl;

  return 0;
}

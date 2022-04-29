/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "docopt.h"

#define FRAME_LEN   0.030 /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

static const char USAGE[] = R"(
get_pitch - Pitch Estimator 

Usage:
    get_pitch [options] <input-wav> <output-txt>
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -h, --help  Show this screen
    -c FLOAT, --c_thr=FLOAT         Umbral Central Clipping [default: 0.0003]
    -p FLOAT, --pot=FLOAT       Potencia (Umbral decisión voiced)  [default: -25]
    -a FLOAT, --r1r0=FLOAT    r1/r0    (Umbral decisión voiced)  [default: 0.65]
    -m FLOAT, --rmaxnorm=FLOAT  rmaxnorm (Umbral decisión voiced) [default: 0.425]
    
    --version   Show the version of the project

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the estimation:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";
// -s INT, --samples=INT           Samples del filtro de mediana [default: 3]
int main(int argc, const char *argv[]) {
	/// \TODO 
	///  Modify the program syntax and the call to **docopt()** in order to
	///  add options and arguments to the program.
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
        {argv + 1, argv + argc},	// array of arguments, without the program name
        true,    // show help if requested
        "2.0");  // version string

	std::string input_wav = args["<input-wav>"].asString();
	std::string output_txt = args["<output-txt>"].asString();
  float c_thr = stof(args["--c_thr"].asString());
  float umb_pot = stof(args["--pot"].asString());
  float umb_r1r0 = stof(args["--r1r0"].asString());
  float umb_rmaxnorm = stof(args["--rmaxnorm"].asString());
  //unsigned int samples = stoi(args["--samples"].asString());
  // Read input sound file
  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0) {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate,umb_pot,umb_r1r0,umb_rmaxnorm, PitchAnalyzer::HAMMING, 50, 500);

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.
      for (unsigned int i = 0; i < x.size(); i++)
  {
    if (abs(x[i]) < c_thr){
       x[i] = 0; 
    }
  }
  
  // Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.

/*
for (unsigned int i = 0; i < f0.size()-2; i++){
    vector<float> aux;
    aux.push_back(f0[i]);
    aux.push_back(f0[i+1]);
    aux.push_back(f0[i+2]);
    std::sort (aux.begin(), aux.end());
    f0[i] = aux[1];
  }
*/

for (unsigned int i = 1; i < f0.size()-1; i++){
    vector<float> aux;
    aux.push_back(f0[i-1]);
    aux.push_back(f0[i]);
    aux.push_back(f0[i+1]);
    std::sort (aux.begin(), aux.end());
    f0[i] = aux[1];
  }




  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good()) {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX) 
    os << *iX << '\n';
  os << 0 << '\n';//pitch at t=Dur

  return 0;
}

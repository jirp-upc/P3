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
    -s INT, --samples=INT           Samples del filtro de mediana [default: 3]
    --version   Show the version of the project

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the estimation:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

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
  unsigned int samples = stoi(args["--samples"].asString());
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
  PitchAnalyzer analyzer(n_len, rate, PitchAnalyzer::HAMMING, 50, 500);

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.
printf("Preprocessing:\n");
      for (unsigned int i = 0; i < x.size(); i++)
  {
    printf("x[i] = %f",x[i]);
    if (abs(x[i]) < c_thr){
       x[i] = 0; 
       printf("\t (x[i]<%f) -> x[i] = 0", c_thr);
    }
    printf("\n");
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

if(samples > f0.size()){ //Si se comete error poner igual o más muestras de filtro de mediana que las de sample, no se realiza.
    samples = f0.size();
  }


if(samples%2 != 0 && samples>1){
  samples += -1;
}else if (samples < 0){
  samples = 1;
}

   for (unsigned int i = 0; i < (f0.size()-samples+1); i++){
    vector<float> med_sample;
      for(unsigned int j = 0; j<samples;j++){
        med_sample.push_back(f0[i+j]);
      }
    std::sort (med_sample.begin(), med_sample.end());
   if(samples%2==0){ //Número de muestras par, se hace promedio de las dos centrales
      f0[i] = (med_sample[(int) samples/2]);
   }else{            //Número de muestras impar, se coge central
      f0[i] = med_sample[(int) samples/2];
   }
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

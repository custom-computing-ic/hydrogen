#include <PerfModel.hpp>
#include <iomanip>
#include <sstream>
#include <fstream>
PerfModel::PerfModel(Implementation *param)
{
  /* Default paramaters that worked well for the Naieve CPU Matrix multiply
   * O(n^3)
   */
  this->imp = param;
  this->alpha = 4e-18;
  this->iterations = 1e2;
  this->numberOfFeatures = 4;
  this->order = 3; 
}
PerfModel::PerfModel(Implementation *param, double alpha, uint64_t it, int feat,int order)
{
  this->imp = param;
  this->alpha = alpha;
  this->iterations = it;
  this->numberOfFeatures = feat;
  this->order = order;
  
}

int PerfModel::CreateModel(int maxsize,std::function<double(int)> f) {
  std::cout << "Creating Performance Model." << std::endl;
  std::vector<double> tmp;
  for (int i = 0; i <= order; i++) {
    tmp.push_back(1.0);
  }
  features.push_back(tmp);
  milliseconds.push_back(1);  
  std::cout << "\t*Generating test data\n\t\t [";
  fflush(stdout);
  for(int i = 0; i < maxsize; i+=5) {
     tmp.clear();
    
     if ( i > 0) {
       for (int j = 0; j <= order; j++) {
        tmp.push_back(pow(i,j));
       }
       features.push_back(tmp);
       double cache_warm_up = f(i);
       double test1 = f(i);
       double test2 = f(i);
       double test3 = f(i);
       double ave = (test1 + test2 + test3) / 3.0;
       milliseconds.push_back(ave);    
     }
     if ( i % (maxsize/10) == 0 ) {
       std::cout << ".";
       fflush(stdout);
//       std::cout << "\nInput of size [" << i << "] Took: " << milliseconds.back() << "ms\n";
     }
  }
  std::cout << "]\n";

  std::cout << "\t*Starting Linear Regression\n\t\t [";
  fflush(stdout);
  this->LinearRegression();
  std::cout << "]" << std::endl;
  return 0;
}

void PerfModel::LinearRegression() {
  this->numberOfDataPoints = milliseconds.size();
  //initialize weights set to 0;
  for (int i = 0; i < numberOfFeatures; i++) {
    weights.push_back(0.0f);
  }
  for (uint64_t iter = 0; iter < iterations; iter++) {
    std::vector<double> newWeights;
      if (iter % (iterations /10)  == 0) {
        std::cout << ".";
        fflush(stdout);
      }
//      std::cout << "****Iteration " << iter << "****\n";
    for(int f = 0; f < numberOfFeatures; f++) {
      double currentWeight = weights[f];
      double costDerivative(0);
      for(int m = 0; m < numberOfDataPoints; m++) {
        double Y = milliseconds[m];
        double H(0);
        for(int i = 0; i < numberOfFeatures; i ++) {
          H+= weights[i]*features[m][i];

        }
//        std::cout << "H: " << H << "\tY: " << Y <<  "\tF: " << features[m][0] << "\t" 
//                  << features[m][1];
        costDerivative += (H-Y)*(features[m][f]);
      }
  //    if (iter % 100 == 0)
 //       std::cout << "\tW[" << f << "]" <<": " << weights[f] << std::endl;

//      std::cout << "-----------------------------------------------------\n";
//      std::cout << " currentWeight: " << currentWeight << "\tcostDerivative: " << costDerivative << "\n";
//      std::cout << "-----------------------------------------------------\n";

      newWeights.push_back(currentWeight - alpha*costDerivative);
    }
    for (int f = 0; f < numberOfFeatures; f++) {
      weights[f] = newWeights[f];
    }
  }

}
double PerfModel::QueryModel(double input) {
  double projectedCost = 0.0;
  std::vector<double> input_data;
  std::cout << std::setprecision(2);
  for (int i = 0; i <= order; i++) {
    input_data.push_back(pow(input,i));
  }
  for (int i = 0; i < numberOfFeatures; i++) {
 //   std::cout << weights[i];
    if ( i > 0)
//      std::cout  << "x^" << i;
    if (i < numberOfFeatures -1)
//      std::cout << " + ";
    projectedCost += weights[i] * input_data[i];
  }
//  std::cout << "\t = ";
  return projectedCost;
}


int PerfModel::SaveToDisk(const std::string& name) {
  std::cout << "Saving this model to: " << name << "_" 
            << imp->getLibName() << "_" << imp->getFuncName() <<".mdl\n";
  //TODO: Serialize relevant data to disk.

  return 0;
}

int PerfModel::SaveToDisk() {
  std::string libName = imp->getLibName();
  std::string funcName = imp->getFuncName();
  std::cout << "Saving this model to: "  << libName 
            << "_" << funcName <<".mdl\n";
  
  //TODO: Serialize relevant data to disk.
  std::ofstream saveFile;
  saveFile.open(libName+"_"+funcName+".mdl",std::ofstream::out);
  boost::archive::text_oarchive output(saveFile);
  output << *this;
  saveFile.close();
  return 0;
}





int PerfModel::LoadFromDisk(const std::string& name) {
  std::cout << "Loading this model from: " << name << "_" 
            << imp->getLibName() << "_" << imp->getFuncName() <<".mdl\n";
  //TODO: Serialize relevant data to disk.

  return 0;
}

int PerfModel::LoadFromDisk(PerfModel& pm) {
  std::string libName = imp->getLibName();
  std::string funcName = imp->getFuncName();
  std::cout << "Loading this model from: "  << imp->getLibName() 
            << "_" << imp->getFuncName() <<".mdl\n";
  //TODO: Serialize relevant data to disk.
  std::ifstream saveFile;
  saveFile.open(libName+"_"+funcName+".mdl",std::ofstream::in);
  boost::archive::text_iarchive input(saveFile);
  input >> pm;
  saveFile.close(); 
  return 0;
}

